#ifndef PARCEL_GENERATOR_DFS_MAZE
#define PARCEL_GENERATOR_DFS_MAZE

#include "../parcel.h"      // and gridTransform.h and parcelGenParameters.h thereby
#include "../mapHelpers.h"  // and ascotherianTileMap.h thereby
#include "../ascoGenContext.h"

/* DFSMaze.h
*
*   Parcel generator for AscoSpel v3
*
*   Description:
*       A Labyrinth of TILE_BLOCKAGEs drawn using a recursive backtracking algorithm;
*       very basic and not very interesting
*
* written January 2022
*/

// ========== DATA STRUCT ==========

//  No data struct

// ==========   IDEATOR   ==========

void DFSMazeIdeator(struct parcel *parcel);

// ==========   REALIZER  ==========

void DFSMazeRealizer(void *context, struct parcel *parcel);


#endif
