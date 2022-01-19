#ifndef PARCEL_GENERATOR_TEST_LAKE
#define PARCEL_GENERATOR_TEST_LAKE

#include "../parcel.h"      // and gridTransform.h and parcelGenParameters.h thereby
#include "../mapHelpers.h"  // and ascotherianTileMap.h thereby
#include "../ascoGenContext.h"

/* testLake.h
*
*   Parcel generator for AscoSpel v3
*
*   Description:
*       Test parcel for a rabidly resizing rect
*
* written January 2022
*/

// ========== DATA STRUCT ==========

//  No data struct

// ==========   IDEATOR   ==========

void testLakeIdeator(struct parcel *parcel);

// ==========   REALIZER  ==========

void testLakeRealizer(void *context, struct parcel *parcel);


#endif
