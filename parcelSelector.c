
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

    // L-case vertical
    static const struct recursorGridSignature lengthLVerticalSignature = {
        1, 2,
        (enum parcelShapes[2]){L_SHAPE, I_SHAPE},
        (cellPopulatorFunctionPtr[2]){RECURSE, RECURSE},
        (unsigned int[2]){0, 1},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (int[2]){-1, -2},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    };

    // L-case horizontal
    static const struct recursorGridSignature lengthLHorizontalSignature = {
        2, 1,
        (enum parcelShapes[2]){I_SHAPE, L_SHAPE},
        (cellPopulatorFunctionPtr[2]){RECURSE, RECURSE},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (int[2]){-1, -2},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    };


    // I-case horizontal (boooring)
    static const struct recursorGridSignature lengthIHorizontalSignature = {
        2, 1,
        (enum parcelShapes[2]){I_SHAPE, I_SHAPE},
        (cellPopulatorFunctionPtr[2]){RECURSE, RECURSE},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (int[2]){-1, -2},
        {0, 0, 1, 0},
        {0, 0, 0, 0}
    };

    // I-case vertical upward
    static const struct recursorGridSignature lengthIVerticalUpSignature = {
        1, 2,
        (enum parcelShapes[2]){L_SHAPE, L_SHAPE},
        (cellPopulatorFunctionPtr[2]){RECURSE, RECURSE},
        (unsigned int[2]){3, 2},
        (unsigned int[2]){0, 1},
        (unsigned int[2]){0, 0},
        (int[2]){-2, -1},
        {1, 0, 0, 0},
        {0, 0, 0, 0}
    };

    // I-case vertical downward
    static const struct recursorGridSignature lengthIVerticalDownSignature = {
        1, 2,
        (enum parcelShapes[2]){L_SHAPE, L_SHAPE},
        (cellPopulatorFunctionPtr[2]){RECURSE, RECURSE},
        (unsigned int[2]){0, 3},
        (unsigned int[2]){0, 1},
        (unsigned int[2]){0, 0},
        (int[2]){-1, -2},
        {0, 0, 1, 0},
        {0, 0, 0, 0}
    };



// ========== VOID INCURSION SIGNATURES ==========

    // L-case upward (void atop)
    static const struct recursorGridSignature voidLUpSignature = {
        1, 2,
        (enum parcelShapes[2]){V_SHAPE, L_SHAPE},
        (cellPopulatorFunctionPtr[2]){&voidBubbleIdeator, RECURSE},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (unsigned int[2]){0, 0},
        (int[2]){TERMINAL, -1},
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
        (int[2]){-1, TERMINAL},
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
        (int[2]){TERMINAL, -1},
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
        (int[4]){-1, -2, TERMINAL, TERMINAL},
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
        (int[4]){-1, -2, TERMINAL, TERMINAL},
        {2, 0, 3, 0},
        {0, 0, 0, 0}
    };



// ========== PUZZLE LEDGE SIGNATURES ==========

    // TODO

// ========== FLANGE SIGNATURES ==========

    // TODO

// ========== BRIDGE SIGNATURES ==========

    // TODO





// Runs the lengthening grids (code exists in duplicate below, so we factor it out up here)
// TODO we need better selections between the lengtheners, not random.
static void lengtheningHelper(struct parcel *parcel){
    if(parcel->shape == L_SHAPE){

        recursorGridIdeator(parcel, ( rand() % 2 ) ? &lengthLVerticalSignature : &lengthLHorizontalSignature);

    } else if(parcel->shape == I_SHAPE) {

        recursorGridIdeator(parcel, ( rand() % 2 ) ? &lengthIVerticalUpSignature : &lengthIVerticalDownSignature);

    } else {
        baseCaseIdeator(parcel);    // Fail safely-ish
    }
}



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
                lengtheningHelper(parcel);
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
    // Bottoming out procedure:


    // a) If we have more than one obligate, lengthen (repeat recursion with a pattern with more than one critical path unit)
    if(parcel->parameters.obligatesCount > 1){
        lengtheningHelper(parcel);
        return;
    }


    // b) If we have exactly one obligate, run the ideator therewith associated.
    if(parcel->parameters.obligatesCount == 1){
        // The below line is tied with 2019's triple pointer for the most C thing I've ever written
        void (*ideatorPointer)(struct parcel *) = (void(*)(struct parcel *))(parcel->parameters.obligates[0].ideatorFunction);
        (*ideatorPointer)(parcel);
        return;
    }

    // c) If we have no obligates, TODO
    DFSMazeIdeator(parcel);

    
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

    */
}
