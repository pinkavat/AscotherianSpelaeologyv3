#ifndef PARCEL_GENERATOR_NPC_ENCOUNTER
#define PARCEL_GENERATOR_NPC_ENCOUNTER

#include "../parcel.h"      // and gridTransform.h and parcelGenParameters.h thereby
#include "../mapHelpers.h"  // and ascotherianTileMap.h thereby
#include "../ascoGenContext.h"

/* NPCEncounter.h
*
*   Parcel generator for AscoSpel v3
*
*   Description:
*       A basic NPC encounter, to test the metadata hooks on
*
* written January 2022
*/

// ========== DATA STRUCT ==========

//  No data struct

// ==========   IDEATOR   ==========

void NPCEncounterIdeator(struct parcel *parcel);

// ==========   REALIZER  ==========

void NPCEncounterRealizer(void *context, struct parcel *parcel);


#endif
