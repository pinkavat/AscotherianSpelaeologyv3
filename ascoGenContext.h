#ifndef ASCO_GEN_CONTEXT
#define ASCO_GEN_CONTEXT

/* ascoGenContext.h
*
*   Context struct for parcelTree realization; incorporates tilemap pointer
*
* written January 2022
*/

#include "ascotherianTileMap.h"

struct ascoGenContext {
    struct ascoTileMap *map;
    // TODO maxdepth
};

#endif
