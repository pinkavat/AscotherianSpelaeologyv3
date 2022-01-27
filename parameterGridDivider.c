
#include "parameterGridDivider.h"
// parameterGridDivider.c
// See header for details


void divideParametersByGrid(struct parcelGenParameters *sourceParameters, const struct recursorGridSignature *signature, struct parcel *children){
    // TODO code outline
    /*
        If child is TERMINAL, copy gate/path, deepen; blank other fields
        If child is PRESET, copy from source
        If child is SHORTCUT_PATH, TODO ???
        
        If child is FLANGE, TODO ???

        If child is CRITICAL_PATH, divide serially among all such children.

        Build subarrays.....?
        Generate shortcut and flange parameter sets for division even if there are no children to divide them among...?
    */

    // TODO stopgap temp stuff
    for(int i = 0; i < signature->width * signature->height; i++){
        children[i].parameters.gateWidth = sourceParameters->gateWidth;
        children[i].parameters.pathWidth = sourceParameters->pathWidth;
        children[i].parameters.recursionDepth = sourceParameters->recursionDepth + 1;
        children[i].parameters.patternProbabilities = sourceParameters->patternProbabilities;
    }
}
