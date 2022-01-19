#ifndef PARCEL_GENERATOR_LOCK_SMASH
#define PARCEL_GENERATOR_LOCK_SMASH

#include "../parcel.h"      // and gridTransform.h and parcelGenParameters.h thereby
#include "../mapHelpers.h"  // and ascotherianTileMap.h thereby
#include "../ascoGenContext.h"

/* lockSmash.h
*
*   Parcel generator for AscoSpel v3
*
*   Description:
*       Basic "Smashable Rock" field for Rock-smash locking
*
* written January 2022
*/

// ========== DATA STRUCT ==========

//  No data struct

// ==========   IDEATOR   ==========

void lockSmashIdeator(struct parcel *parcel);

// ==========   REALIZER  ==========

void lockSmashRealizer(void *context, struct parcel *parcel);


#endif
