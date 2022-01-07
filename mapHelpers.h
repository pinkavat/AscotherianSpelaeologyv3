#ifndef MAP_HELPERS
#define MAP_HELPERS

#include "ascotherianTileMap.h"
#include "gridTransform.h"

/* mapHelpers.h
*
*   Convenient helper functions for drawing ascoTile data subject to transforms
*
* written December 2021
*/


// Fill the indicated rectangular region of the map with the indicated cell data
// Subjects the region to a grid transform (i.e. the region provided is in the transform's local space, which is converted to the global space of the map
// by this process)
//  a, b, w, and h define the rectangular region of the grid that will be iterated over; top-left x, top-left y, width, and height, respectively.
//  these parameters are in the local space of gridTransform t.
void fillRect(struct ascoTileMap *map, struct ascoCell *cell, struct gridTransform *t, int a, int b, int w, int h);


// As above, except adjusts the cell's variant and rotation based on its tiling type (e.g. for an MS-tiling cliff, the corners of the rect are concave corners, and the edges
// are straight tiles).
// TODO cliffs and water tile inwards; do other MS tiles tile outwards? WILL WE NEED A FLIP OPTION?
// If "fillCore" param is true, center of rect will be filled with CENTER variant tiles; otherwise function will not modify center of rect.
// TODO define behaviours for non-MS types; TODO define too-small edge cases
void fillRectAuto(struct ascoTileMap *map, struct ascoCell *cell, struct gridTransform *t, int a, int b, int w, int h, unsigned int fillCore);


// TODO ledge/stair deployer (direction-sensitive terminated rect handler)

#endif
