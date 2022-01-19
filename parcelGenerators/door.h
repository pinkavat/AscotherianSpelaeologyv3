#ifndef PARCEL_GENERATOR_DOOR
#define PARCEL_GENERATOR_DOOR

#include "../parcel.h"      // and gridTransform.h and parcelGenParameters.h thereby
#include "../mapHelpers.h"  // and ascotherianTileMap.h thereby
#include "../ascoGenContext.h"

/* door.h
*
*   Parcel generator for AscoSpel v3
*
*   Description:
*       This parcel contains a door to another map.
*       TODO: this implies a warp hook in the context. Develop and document.
*
*       TODO: this is an EDGE DOOR ONLY, not an interior door. THESE MUST BE ON THE MAP EDGE, AND ASSUME THAT THEY THEMSELVES
*       ARE SURROUNDED ON THE DOOR SIDE BY CLIFF (there's a cheeky trick we pull with the door tile placement OUT-OF-BOUNDS of the parcel)
*
* written January 2022
*/

// ========== DATA STRUCT ==========

//  No data struct

// ==========   IDEATOR   ==========

void doorIdeator(struct parcel *parcel);

// ==========   REALIZER  ==========

void doorRealizer(void *context, struct parcel *parcel);


#endif
