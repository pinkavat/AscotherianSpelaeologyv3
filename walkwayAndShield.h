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

void realizeWalkwayAndShield(struct ascoTileMap *map, struct parcel *parcel /*TODO gates*/);

#endif
