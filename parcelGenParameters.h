#ifndef PARCEL_GEN_PARAMETERS
#define PARCEL_GEN_PARAMETERS

#include "recursorPatternSelector.h"

/* parcelGenParameters.h
*
*   Defines generation parameters handed down through the recursion process of AscoSpel generation;
*   allows the decider to decide which recursive grid pattern or base case to assign to any given parcel.
*
*   In AscoSpel v2, this file also outlined a .c file containing code for dividing parameters up. Since this
*   functionality is now bound about with the grid recursor pattern notation by dint of grouping, dividing
*   parameters is now the recursor's responsibility.
*
* written January 2022
*/

struct parcelGenParameters {

    int recursionDepth;                                     // The number of recursive steps taken to arrive at this point

    int gateWidth;                                          // The width of gates generated in this parcel
    int pathWidth;                                          // The width of walkway paths generated in this parcel
 
    struct patternProbabilityTable *patternProbabilities;   // Pointer to the probability table used by the selector to decide which recursive pattern
                                                            // to use for this parcel, if any.
};

#endif
