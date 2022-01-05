#ifndef PARCEL_GENERATOR_BASE_CASE
#define PARCEL_GENERATOR_BASE_CASE

#include "../parcel.h"      // and gridTransform.h and parcelGenParameters.h thereby
#include "../mapHelpers.h"  // and ascotherianTileMap.h thereby

/* baseCase.h
*
*   Parcel generator for AscoSpel v3
*
*   Description:
*       The Base Case parcel is a completely empty, flat floor, and is used as a fallback for the generator.
*
* written January 2022
*/

// ========== DATA STRUCT ==========

//  No data struct

// ==========   IDEATOR   ==========

void baseCaseIdeator(struct parcel *parcel);

// ==========   REALIZER  ==========

void baseCaseRealizer(void *context, struct parcel *parcel);


#endif
