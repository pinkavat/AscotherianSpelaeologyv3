
#include "parcelSelector.h"
// parcelSelector.c
// See header for details

void selectAndApplyParcelGenerator(struct parcel *parcel){

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

        if(rand() % 2){
            // Corner black signature
            enum parcelShapes shapes[4] = {L_SHAPE, V_SHAPE, L_SHAPE, L_SHAPE};
            unsigned int rotations[4] = {0, 0, 3, 0};
            unsigned int flipHs[4] = {0, 0, 1, 0};
            unsigned int flipVs[4] = {0, 0, 0, 0};
            //int pathGroups[4] = {1, 0, 1, 1};
            cellPopulatorFunctionPtr popFuncs[4] = {&selectAndApplyParcelGenerator, &voidBubbleIdeator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator};
            enum parameterDivisionTypes divTypes[4] = {CRITICAL_PATH, TERMINAL, CRITICAL_PATH, CRITICAL_PATH};

            struct recursorGridSignature gridSig = {
                2, 2,
                shapes,
                popFuncs,
                rotations,
                flipHs,
                flipVs,
                divTypes,
                //pathGroups,
                //1,
                //0,
                {0, 3, 0, 0},   // TODO TODO TODO NEVER FORGET TO CHANGE THE GATE SOURCE THINGIES!!!!!!!!!!!!!!!!!
                {0, 0, 0, 0}
            };
    
            recursorGridIdeator(parcel, &gridSig);

        } else {

            // Ledge loop small
            enum parcelShapes shapes[4] = {TL_SHAPE, L_SHAPE, TL_SHAPE, L_SHAPE};
            unsigned int rotations[4] = {0, 1, 3, 2};
            unsigned int flipHs[4] = {0, 1, 1, 1};
            unsigned int flipVs[4] = {0, 0, 0, 0};
            //int pathGroups[4] = {1, 1, 1, 1};
            enum parameterDivisionTypes divTypes[4] = {FLANGE_REWARD, FLANGE_PUZZLE, TERMINAL, FLANGE_PUZZLE};
            cellPopulatorFunctionPtr popFuncs[4] = {&ledgeIdeator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator};


            struct recursorGridSignature gridSig = {
                2, 2,
                shapes,
                popFuncs,
                rotations,
                flipHs,
                flipVs,
                divTypes,
                //pathGroups,
                //1,
                //0,
                {0, 2, 0, 0},   // TODO TODO TODO NEVER FORGET TO CHANGE THE GATE SOURCE THINGIES!!!!!!!!!!!!!!!!!
                {0, 0, 0, 0}
            };
    
            recursorGridIdeator(parcel, &gridSig);

        }

    }
}
