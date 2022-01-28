#ifndef PARCEL_GEN_PARAMETERS
#define PARCEL_GEN_PARAMETERS

#include "recursorPatternSelector.h"

/* parcelGenParameters.h
*
*   Defines generation parameters handed down through the recursion process of AscoSpel generation;
*   allows the selector to decide which recursive grid pattern or base case to assign to any given parcel.
*
*   In AscoSpel v2, this file also outlined a .c file containing code for dividing parameters up. Since this
*   functionality is now bound about with the grid recursor pattern notation by dint of grouping, dividing
*   parameters is now the recursor's responsibility.
*
* written January 2022
*/

// A forced feature; a single element of a list. The selector will guarantee that these features will be ideated/realized within the parcel
// whose parameters they are part of or that parcel's children, in the order in which they are found in the list. 
struct obligate {
    void (*ideatorFunction)(void *);  // Pointer to the ideator function used to generate the obligate feature (cast by selector to avoid circular dependency)
    // TODO data
};


// Parcel Generation parameters: each parcel possesses one of these structs.
struct parcelGenParameters {

    int recursionDepth;                                     // The number of recursive steps taken to arrive at this point

    int gateWidth;                                          // The width of gates generated in this parcel
    int pathWidth;                                          // The width of walkway paths generated in this parcel
 
    struct patternProbabilityTable *patternProbabilities;   // Pointer to the probability table used by the selector to decide which recursive pattern
                                                            // to use for this parcel, if any.

    struct obligate *obligates;                             // Heap-resident array of obligatory features (see above)
    unsigned int obligatesCount;                            // Size of the above array
};

#endif
