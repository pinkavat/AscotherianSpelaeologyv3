#ifndef PARCEL_GENERATOR_LAKE
#define PARCEL_GENERATOR_LAKE

#include "../parcel.h"      // and gridTransform.h and parcelGenParameters.h thereby
#include "../mapHelpers.h"  // and ascotherianTileMap.h thereby
#include "../ascoGenContext.h"

/* lake.h
*
*   Parcel generator for AscoSpel v3
*
*   Description:
*       Generates a square lake that resizes rabidly.
*       Thanks to the "waterworld" expansion assumptions (that the postprocessor will clean up adjoining water bodies) this parcel is unique in that it
*       zeroes out its own walkway, leading to direct contact with the incoming parcel.
*
* written January 2022
*/

// ========== DATA STRUCT ==========

//  No data struct

// ==========   IDEATOR   ==========

void lakeIdeator(struct parcel *parcel);

// ==========   REALIZER  ==========

void lakeRealizer(void *context, struct parcel *parcel);


#endif
