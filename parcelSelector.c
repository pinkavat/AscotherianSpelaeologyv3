
#include "parcelSelector.h"
// parcelSelector.c
// See header for details

void selectAndApplyParcelGenerator(struct parcel *parcel){

    // TODO demo testing code

    if(parcel->parameters.recursionDepth >= 2 || parcel->shape != L_SHAPE){
        // Base out
        //testLakeIdeator(parcel);
        //baseCaseIdeator(parcel);
        //lockSmashIdeator(parcel);
        if(rand() % 2) {
            DFSMazeIdeator(parcel);
        } else {
            testLakeIdeator(parcel);
        }
    } else {
        // Recurse with grid
        // Sample grid signature from above
        /*
        enum parcelShapes shapes[4] = {L_SHAPE, L_SHAPE, L_SHAPE, L_SHAPE};
        unsigned int rotations[4] = {3, 0, 2, 1};
        //unsigned int rotations[4] = {1, 2, 0, 3};
        unsigned int flipHs[4] = {0, 0, 0, 0};
        unsigned int flipVs[4] = {0, 0, 0, 0};
        cellPopulatorFunctionPtr popFuncs[4] = {&baseCaseIdeator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator};
        */
        
        /*
        enum parcelShapes shapes[4] = {I_SHAPE, L_SHAPE, L_SHAPE, L_SHAPE};
        unsigned int rotations[4] = {0, 0, 0, 1};
        unsigned int flipHs[4] = {0, 0, 1, 0};
        unsigned int flipVs[4] = {0, 0, 0, 0};
        int pathGroups[4] = {1, 1, 1, 1};
        cellPopulatorFunctionPtr popFuncs[4] = {&selectAndApplyParcelGenerator, &baseCaseIdeator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator};
        */
         
        enum parcelShapes shapes[4] = {L_SHAPE, V_SHAPE, L_SHAPE, L_SHAPE};
        unsigned int rotations[4] = {0, 0, 3, 0};
        unsigned int flipHs[4] = {0, 0, 1, 0};
        unsigned int flipVs[4] = {0, 0, 0, 0};
        int pathGroups[4] = {1, 0, 1, 1};
        cellPopulatorFunctionPtr popFuncs[4] = {&selectAndApplyParcelGenerator, &voidBubbleIdeator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator};


        struct recursorGridSignature gridSig = {
            2, 2,
            shapes,
            popFuncs,
            rotations,
            flipHs,
            flipVs,
            pathGroups,
            1,
            0,
            {0, 3, 0, 0},   // TODO TODO TODO NEVER FORGET TO CHANGE THE GATE SOURCE THINGIES!!!!!!!!!!!!!!!!!
            {0, 0, 0, 0}
        };
        

        /*
        enum parcelShapes shapes[2] = {L_SHAPE, L_SHAPE};
        unsigned int rotations[2] = {0, 1};
        unsigned int flipHs[2] = {1, 0};
        unsigned int flipVs[2] = {0, 0};
        cellPopulatorFunctionPtr popFuncs[2] = {&selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator};
    
 
        struct recursorGridSignature gridSig = {
            2, 1,
            shapes,
            popFuncs,
            rotations,
            flipHs,
            flipVs,
            {0, 0},
            { 0, 0}
        };
        */

        recursorGridIdeator(parcel, &gridSig);
    }
}
