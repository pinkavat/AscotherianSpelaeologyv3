
#include "parcelSelector.h"
// parcelSelector.c
// See header for details

// TODO PERHAPS MIGRATE ALL THESE SIGNATURES TO THEIR OWN FUNCTIONS (which could adjust for shape too)
// TODO we really need a better representative strategy for concision's sake; as strings, or something...
//  (though hooray for statically-stored compound literals!)

/* TODO temp reference gridsig struct
struct recursorGridSignature {
    int width, height;                                  // The dimensions of the grid
    enum parcelShapes *shapes;                          // 2D row-major grid of parcel shapes of the above dimensions
    cellPopulatorFunctionPtr *populatorFunctions;       // 2D row-major grid of functions of the above dimensions; each function is invoked to generate
    unsigned int *rotations;                            // 2D row(etc...), contains the rotations of the corresponding parcels.
    unsigned int *flipHs;                               // Likewise, for horizontal flips
    unsigned int *flipVs;                               // Likewise, for vertical flips         (see gridTransform.h if in doubt)
    enum parameterDivisionTypes *parameterDivisionType; // How the grid solver should assign this parcel's parameters (see above)
    int gateSourceIndices[4];                           // If the shape of the grid includes gate n, element n of this array is the index of the
    int gateIsFork[4];
};

*/

// pop-function contraction for signature defs
#define RECURSE &selectAndApplyParcelGenerator

// ========== LENGTHENING SIGNATURES ==========

    // TODO

// ========== VOID INCURSION SIGNATURES ==========

    // L-case upward (void atop)
    static const struct recursorGridSignature voidLUpSignature = {
        1, 2,
        (enum parcelShapes[2]){V_SHAPE, L_SHAPE},
        (cellPopulatorFunctionPtr[2]){&voidBubbleIdeator, RECURSE},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (enum parameterDivisionTypes[2]){TERMINAL, CRITICAL_PATH},
        {1, 1, 0, 0},
        {0, 0, 0, 0}
    };

    // L-case rightward (void on right)
    static const struct recursorGridSignature voidLRightSignature = {
        2, 1,
        (enum parcelShapes[2]){L_SHAPE, V_SHAPE},
        (cellPopulatorFunctionPtr[2]){RECURSE, &voidBubbleIdeator},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (enum parameterDivisionTypes[2]){CRITICAL_PATH, TERMINAL},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };

    // I-case (void atop)
    static const struct recursorGridSignature voidISignature = {
        1, 2,
        (enum parcelShapes[2]){V_SHAPE, I_SHAPE},
        (cellPopulatorFunctionPtr[2]){&voidBubbleIdeator, RECURSE},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (enum parameterDivisionTypes[2]){TERMINAL, CRITICAL_PATH},
        {1, 0, 1, 0},
        {0, 0, 0, 0}
    };



// ========== FORK SIGNATURES ==========

    // TODO

// ========== STRUCTURAL LEDGE SIGNATURES ==========

    // L-case
    static const struct recursorGridSignature SLedgeLSignature = {
        2, 2,
        (enum parcelShapes[4]){L_SHAPE, L_SHAPE, TLS_SHAPE, TI_SHAPE},
        (cellPopulatorFunctionPtr[4]){RECURSE, RECURSE, &ledgeIdeator, &baseCaseIdeator},
        (unsigned int[4]){3, 0, 2, 1},
        (unsigned int[4]){0, 0, 1, 0},
        (unsigned int[4]){0, 0, 0, 0},
        (enum parameterDivisionTypes[4]){CRITICAL_PATH, CRITICAL_PATH, TERMINAL, TERMINAL},
        {2, 3, 0, 0},
        {0, 0, 0, 0}
    };


    // I-case
    static const struct recursorGridSignature SLedgeISignature = {
        2, 2,
        (enum parcelShapes[4]){L_SHAPE, L_SHAPE, TLS_SHAPE, TLT_SHAPE},
        (cellPopulatorFunctionPtr[4]){RECURSE, RECURSE, &ledgeIdeator, &baseCaseIdeator},
        (unsigned int[4]){3, 0, 2, 3},
        (unsigned int[4]){0, 0, 1, 1},
        (unsigned int[4]){0, 0, 0, 0},
        (enum parameterDivisionTypes[4]){CRITICAL_PATH, CRITICAL_PATH, TERMINAL, TERMINAL},
        {2, 0, 3, 0},
        {0, 0, 0, 0}
    };



// ========== PUZZLE LEDGE SIGNATURES ==========

    // TODO

// ========== FLANGE SIGNATURES ==========

    // TODO

// ========== BRIDGE SIGNATURES ==========

    // TODO







void selectAndApplyParcelGenerator(struct parcel *parcel){

    // 1) Perform a brief depth check to avoid accidental infinite recursion
    // 2) For now, invoking selection on anything other than an L or an I is malformed and results in bottoming out
    if(parcel->parameters.recursionDepth < ABSOLUTE_MAXIMUM_RECURSIVE_DEPTH && 
        (parcel->shape == L_SHAPE || parcel->shape == I_SHAPE)){

        // 2) Decide whether to recurse or not, and, if so, what to recurse to
        switch(selectPattern(parcel->parameters.patternProbabilities, parcel->parameters.recursionDepth)){
            case PATTERN_TERMINATE:
                // Bottom out (fall through)
            break;
            case PATTERN_LENGTHEN:
                // TODO
            break;
            case PATTERN_VOID_INCURSION:
                if(parcel->shape == L_SHAPE){
                    recursorGridIdeator(parcel, &voidLUpSignature); // TODO differentiate between side void and top void
                } else {
                    recursorGridIdeator(parcel, &voidISignature);
                }    
            return;
            case PATTERN_FORK:
                // TODO
            break;
            case PATTERN_LEDGE_STRUCTURAL:
                if(parcel->shape == L_SHAPE){
                    recursorGridIdeator(parcel, &SLedgeLSignature);
                } else {
                    recursorGridIdeator(parcel, &SLedgeISignature);
                }    
            return;
            case PATTERN_LEDGE_PUZZLE:
                // TODO
            break;
            case PATTERN_FLANGE_FALSE:
                // TODO
            break;
            case PATTERN_FLANGE_REWARD:
                // TODO
            break;
            case PATTERN_BRIDGE:
                // TODO
            break;
        }
    }

    // 3) If we've reached this point we're attempting to "bottom out"
    // TODO obligate check
    switch(rand() % 3){
        case 0: testLakeIdeator(parcel);    break;
        case 1: DFSMazeIdeator(parcel);     break;
        case 2: lockSmashIdeator(parcel);   break;
    } 
    
    /*

    THOIGHTS ALGO

        Here's an outline:
        1) If we're over maximum fractal depth, attempt to bottom out. (invert param -- bottom out at zero?) (two params tracking in both directions?)
        2) Decide whether to RECURSE or BOTTOM OUT.
        ATTEMPT TO BOTTOM OUT is as follows:
            1) If we have more than one obligate, recurse again and repeat.                 (lengthening)
            2) If we have exactly one obligate, become the base case associated therewith.  (satisfaction)
            3) If we have no obligates:
                4) If we have no potentials, become the base case fallback.                 (failsafe)
                5) Otherwise, select a potential and become the associated base case.       (selection)




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
            enum parcelShapes shapes[4] = {TLS_SHAPE, L_SHAPE, TI_SHAPE, L_SHAPE};
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

    */
}
