#ifndef PARCEL_GENERATOR_ICE_PUZZLE
#define PARCEL_GENERATOR_ICE_PUZZLE

#include "../parcel.h"      // and gridTransform.h and parcelGenParameters.h thereby
#include "../mapHelpers.h"  // and ascotherianTileMap.h thereby
#include "../ascoGenContext.h"

/* icePuzzle.h
*
*   Parcel generator for AscoSpel v3
*
*   Description:
*       The AscoSpel v3 version of the basic shuffled-line based slippery-surface puzzle generator.
*
*   TODO FUTURE ME, IF YOU'RE READING THIS TO POACH THE ALGORITHM:
*       REMEMBER ICEFALL CAVE -- where PITS LEAD TO SPECIAL ICE LANES. ADD THIS
*
* written February 2022
*/

// ========== DATA STRUCT ==========

//  No data struct

// ==========   IDEATOR   ==========

void icePuzzleIdeator(struct parcel *parcel);

// ==========   REALIZER  ==========

void icePuzzleRealizer(void *context, struct parcel *parcel);


#endif
