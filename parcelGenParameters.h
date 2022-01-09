#ifndef PARCEL_GEN_PARAMETERS
#define PARCEL_GEN_PARAMETERS

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

    unsigned int recursionDepth;    // The number of recursive steps taken to arrive at this point

 
    // TODO something better than "challenge rating" ("type theory"?)
    // TODO reward notation
    // TODO lock bitsets

    // TODO global params stored / accessed from here (cave biome/type, prevailing mechanic, etc. All later, for the selector's use)

};

#endif
