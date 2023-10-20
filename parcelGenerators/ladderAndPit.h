#ifndef PARCEL_GENERATOR_LADDER_AND_PIT
#define PARCEL_GENERATOR_LADDER_AND_PIT

#include "../parcel.h"      // and gridTransform.h and parcelGenParameters.h thereby
#include "../mapHelpers.h"  // and ascotherianTileMap.h thereby
#include "../ascoGenContext.h"

/* ladderAndPit.h
*
*   Parcel generator for AscoSpel v3
*
*   Description:
*       A set of parcel generators sharing a common core function, for placing ladders and pits, which connect to other maps.
*       TODO: this implies a warp hook in the context. Develop and document.
*
* written January 2022
*/

// ========== DATA STRUCT ==========

//  No data struct

// ==========   IDEATORS   ==========

// Upward-leading ladder
void ladderUpIdeator(struct parcel *parcel);

// Downward-leading ladder
void ladderDownIdeator(struct parcel *parcel);

// Pit leading down (one-way descent)
void pitIdeator(struct parcel *parcel);

// Landing site for a pit (pit leads here)
void pitLandingIdeator(struct parcel *parcel);

// ==========   REALIZERS  ==========

void ladderAndPitRealizer(void *context, struct parcel *parcel);


#endif
