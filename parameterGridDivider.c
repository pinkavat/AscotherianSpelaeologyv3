
#include "parameterGridDivider.h"
// parameterGridDivider.c
// See header for details



// TODO document
static void divideParametersParallel(struct parcelGenParameters *sourceParameters, struct parcelGenParameters *destParameters, unsigned int count){
    // TODO temp stopgap code
    for(int i = 0; i < count; i++){
        destParameters[i] = *sourceParameters;
    }
}

// TODO document
static void divideParametersSerial(struct parcelGenParameters *sourceParameters, struct parcelGenParameters *destParameterPtrs[], unsigned int count){
    for(int i = 0; i < count; i++){
        (*(destParameterPtrs[i])).recursionDepth = sourceParameters->recursionDepth + 1;
        (*(destParameterPtrs[i])).gateWidth = sourceParameters->gateWidth;
        (*(destParameterPtrs[i])).pathWidth = sourceParameters->pathWidth;
    }
}




void divideParametersByGrid(struct parcelGenParameters *sourceParameters, struct recursorGridSignature *signature, struct parcel *children){

    // 0) Establish how many children we're dealing with
    int numChildren = signature->width * signature->height;

    // 1) Prepare child parameter reference arrays for each path group
    struct parcelGenParameters *criticalParameterPtrs[signature->numCritPaths][numChildren];    // assume numChildren as a maximum bound
    struct parcelGenParameters *shortcutParameterPtrs[signature->numShortcutPaths][numChildren];
    int numCriticalParcels[signature->numCritPaths];
    for(int i = 0; i < signature->numCritPaths; i++) numCriticalParcels[i] = 0;
    int numShortcutParcels[signature->numShortcutPaths];
    for(int i = 0; i < signature->numShortcutPaths; i++) numShortcutParcels[i] = 0;


    // 2) Scan children; link child parameters into reference arrays and establish how many paths there are in each parcel
    for(int i = 0; i < numChildren; i++){
        int group = signature->pathGroups[i];
        if(group > 0){
            // Critical-path child
            criticalParameterPtrs[group - 1][numCriticalParcels[group - 1]] = &(children[i].parameters);
            numCriticalParcels[group - 1]++;
        } else if (group < 0){
            // Shortcut-path child
            group = 0 - group;
            shortcutParameterPtrs[group - 1][numShortcutParcels[group - 1]] = &(children[i].parameters);
            numShortcutParcels[group - 1]++;
        } else {
            // Group-zero parcels are VOID and get basic parameters.
            // TODO: flesh out with further param details...?
            // TODO: do void parcels even need parameters? They don't rewrite or have gates.
        }
    }


    // 3) Divide source parameter into critical master and shortcut master
    struct parcelGenParameters criticalMasterParameters = *sourceParameters; // TODO 
    struct parcelGenParameters shortcutMasterParameters = *sourceParameters; // TODO


    // 4) Divide these master parameters in parallel between all their paths
    struct parcelGenParameters criticalPathParameters[signature->numCritPaths];
    divideParametersParallel(&criticalMasterParameters, criticalPathParameters, signature->numCritPaths);
    struct parcelGenParameters shortcutPathParameters[signature->numShortcutPaths];
    divideParametersParallel(&shortcutMasterParameters, shortcutPathParameters, signature->numShortcutPaths);


    // 5) Divide the path parameters serially among all child parameters in that path and finalize
    for(int i = 0; i < signature->numCritPaths; i++){
        divideParametersSerial(&(criticalPathParameters[i]), criticalParameterPtrs[i], numCriticalParcels[i]);
    }
    for(int i = 0; i < signature->numShortcutPaths; i++){
        divideParametersSerial(&(shortcutPathParameters[i]), shortcutParameterPtrs[i], numShortcutParcels[i]);
    }

}
