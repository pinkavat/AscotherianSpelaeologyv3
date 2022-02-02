#ifndef WALKWAY_AND_SHIELD
#define WALKWAY_AND_SHIELD

#include "ascotherianTileMap.h"
#include "parcel.h"
#include "mapHelpers.h"

/*walkwayAndShield.h
*
*   Code for drawing a parcel's walkway and shield blobs.
*   Invoked by the parcel's parent, once the parcel has successfully realized
*
* written January 2022
*/

// Takes the context map, the parcel whose walkway and shield this is, and the gate of said parcel (inner gate) plus the gate imposed by its neighbor (outer gate)
// The last two parameters define whether the burden of L-clobbering is passed onto the walkway.
void realizeWalkwayAndShield(struct ascoTileMap *map, struct parcel *parcel, struct gate *innerGate, struct gate *outerGate, int topBarrier, int bottomBarrier);

#endif
