#ifndef PARCEL_GENERATOR_VOID_BUBBLE
#define PARCEL_GENERATOR_VOID_BUBBLE

#include "../parcel.h"      // and gridTransform.h and parcelGenParameters.h thereby
#include "../mapHelpers.h"  // and ascotherianTileMap.h thereby
#include "../ascoGenContext.h"

/* voidBubble.h
*
*   Parcel generator for AscoSpel v3
*
*   Description:
*       "Void Bubbles" are potential incursions of the map edge into the recursive map shape.
*       They realize initially as blobs of inaccessible floor, which is resolved by the post-processor.
*       Void bubbles are ALWAYS V-shaped.
*
* written January 2022
*/

// ========== DATA STRUCT ==========

//  No data struct

// ==========   IDEATOR   ==========

void voidBubbleIdeator(struct parcel *parcel);

// ==========   REALIZER  ==========

void voidBubbleRealizer(void *context, struct parcel *parcel);


#endif
