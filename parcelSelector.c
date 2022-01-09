
#include "parcelSelector.h"
// parcelSelector.c
// See header for details

void selectAndApplyParcelGenerator(struct parcel *parcel){

    // TODO demo testing code

    if(parcel->parameters.recursionDepth >= 1){
        // Base out
        //testLakeIdeator(parcel);
        baseCaseIdeator(parcel);
    } else {
        // Recurse with grid
        // Sample grid signature from above
        enum parcelShapes shapes[4] = {L_SHAPE, L_SHAPE, L_SHAPE, L_SHAPE};
        unsigned int rotations[4] = {3, 0, 2, 1};
        unsigned int flipHs[4] = {0, 0, 0, 0};
        unsigned int flipVs[4] = {0, 0, 0, 0};
    
        //cellPopulatorFunctionPtr popFuncs[4] = 
            //{&selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator};
        cellPopulatorFunctionPtr popFuncs[4] = {&baseCaseIdeator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator};
    
        struct recursorGridSignature gridSig = {
            2, 2,
            shapes,
            popFuncs,
            rotations,
            flipHs,
            flipVs
        };

        recursorGridIdeator(parcel, &gridSig);
    }
}
