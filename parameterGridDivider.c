#include <string.h> // For memcpy


#include "parameterGridDivider.h"
// parameterGridDivider.c
// See header for details

#include <stdio.h>  // TODO debug
void divideParametersByGrid(struct parcelGenParameters *sourceParameters, const struct recursorGridSignature *signature, struct parcel *children){

    // Establish number of children
    int childCount = signature->width * signature->height;

    // Backing array for storing references to critical path members
    struct parcel *critChildPtrs[childCount];
    int numCritChildren = 0;

    // Initial pass over all children: Deepen recursion, copy path and gate widths, copy pattern prob table
    for(int i = 0; i < childCount; i++){

        children[i].parameters.gateWidth = sourceParameters->gateWidth;
        children[i].parameters.pathWidth = sourceParameters->pathWidth;
        children[i].parameters.recursionDepth = sourceParameters->recursionDepth + 1;
        children[i].parameters.patternProbabilities = sourceParameters->patternProbabilities;
        children[i].parameters.obligates = NULL;
        children[i].parameters.obligatesCount = 0;

        // Identify child type and treat
        if(signature->parameterDivisionTypes[i] < 0){
            // Critical Path child, add reference to list
            critChildPtrs[(-1) - signature->parameterDivisionTypes[i]] = &(children[i]);   // TODO no bounds check; assume well-formed safety.
            numCritChildren++;
        }
        /*
        switch(signature->parameterDivisionTypes[i]){
            case TERMINAL:
                // TODO
            break;
            case SHORTCUT_PATH:
                // TODO
            break;
            case FLANGE_PUZZLE:
                // TODO
            break;
            case FLANGE_REWARD:
                // TODO
            break;
            case PRESET:
                // TODO
            break;
        }*/
    }

    // Serially divide obligates among critical-path children
    int j = 0;
    for(int i = 0; i < numCritChildren; i++){
        // 1) Determine how many obligates this child is getting
        int numObligates = (sourceParameters->obligatesCount / numCritChildren) + ((i < ((sourceParameters->obligatesCount) % numCritChildren)) ? 1 : 0);
        
        // 2) Allocate space for obligate array
        struct parcel *child = critChildPtrs[i];
        child->parameters.obligates = (void *)malloc(sizeof(struct obligate) * numObligates);
        child->parameters.obligatesCount = numObligates;

        // 3) Perform copy
        struct obligate *obligatesStart = &(sourceParameters->obligates[j]);
        memcpy(child->parameters.obligates, obligatesStart, numObligates * sizeof(struct obligate));
        j += numObligates;
    }


    // TODO code outline
    /*
        If child is TERMINAL, copy gate/path, deepen; blank other fields
        If child is PRESET, copy from source
        If child is SHORTCUT_PATH, TODO ??? No multiples exist!
        
        If child is FLANGE, TODO ??? May multiples exist?

        If child is CRITICAL_PATH, divide serially among all such children. (place in prebuilding division array)

        Build subarrays.....?
        Generate shortcut and flange parameter sets for division even if there are no children to divide them among...?
    */

}
