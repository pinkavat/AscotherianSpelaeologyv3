#ifndef ASCOTHERIAN_TILE_MAP
#define ASCOTHERIAN_TILE_MAP

#include <stdio.h>
#include <stdlib.h>

/* ascotherianTileMap.h
*
*   Standard for unifying all my procedural-generation and rendering projects for the Ascotherian World under one 
*   global tile system because I'm sick and tired of rewriting "Marching Squares" every ten minutes.
*
*   TODO: mapfile IO
*   TODO: find some way of excising the debug data from a deployment header?
*   TODO: add a "debug flag" to the tiles, such that they are highlighted in debug print???
*   TODO: map validation?: some tiles declare membership in a tiling type but don't have meaningful tiles for certain variants (e.g. no convex stairs (yet))
*
* written December 2021 as part of AscoSpel v3
* used to be header-only until I remembered what "static" meant
*/


// ========== STRUCTURE ==========

enum ascoTilingTypes {
    ASCO_TILING_NONE,       // Does not tile                        (Center)
    ASCO_TILING_MS,         // MS tiling with center and terminii   (Center, Straight, Convex, Concave, Left-hand terminus, Right-hand terminus)
    ASCO_TILING_FENCE,      // Direct connectivity                  (5 degrees)
    ASCO_TILING_LARGE,      // Super-sized tiles                    (Interstitial, Corner [convex])
};


// A tile (type)
struct ascoTile {
    const char * name;                  // Printable name for reference
    const char * debugPrint;            // Printable representation of tile for debugging (different tiling types override this sometimes, e.g. on water edges)
    const char * debugColour;           // Terminal colour to set when printing this tile for debugging (whether or no the above string is overridden)

    enum ascoTilingTypes tilingType;    // The tiling type of this tile. Tells us how many variants to expect and how to rotate/mirror the tile.
};


// One cell of the below tilemap; "contains" (references) a tile, plus other data
struct ascoCell {
    struct ascoTile *tile;              // The tile, drawn from some materials palette

    uint8_t variant;                    // The variant of the material (interpreted according to the material's tiling type)

    uint8_t rotation;                   // Rotation of the tile, clockwise in 90-degree increments from top(-right)
    int z;                              // Z-position of the tile (X and Y inferred from grid traversal)
    
    // TODO floor data
    // TODO other data (game data, collision layer data, etc.)
};



// Accessor helper macro for the below scheme
#define mapCell(m, x, y) (m)->cells[((y) * (m)->width) + (x)]

// Could be a chunk; could be the whole thing. Not our problem.
struct ascoTileMap {
    struct ascoCell *cells;         // Row-major 2D grid of cells
    unsigned int width, height;     // The dimensions of said grid
    // Tilemaps are at present one layer only.
};




// ========== ASCOTHERIAN WORLD TILE PALETTE ==========
// See associated .c file for the data
extern struct ascoTile ascoTiles[];

// These definitions take the place of the cumbersome enums of yore
#define TILE_VOID &ascoTiles[0]
#define TILE_UNKNOWN &ascoTiles[1]
#define TILE_UNRESOLVED &ascoTiles[2]
#define TILE_BLOCKAGE &ascoTiles[3]

#define TILE_BLANK &ascoTiles[4]

#define TILE_CLIFF &ascoTiles[5]
#define TILE_STAIR &ascoTiles[6]
#define TILE_WATER &ascoTiles[7]




// ========== CREATION AND DESTRUCTION ==========

// Allocates and returns a new map of the given dimensions, initialized to void tiles
struct ascoTileMap *newAscoTileMap(unsigned int width, unsigned int height);

void freeAscoTileMap(struct ascoTileMap *map);




// ========== TILE ROTATION HANDLING ==========

// TODO VERY IMPORTANT: Anchored supertiles "clobber" other parts of the map upon rotation.
// THAT'S A BAD DEPENDENCY, WE NEED TO THINK ABOUT THIS FROM A DIFFERENT ANGLE.
// We can't easily abandon multicell tiles in the overworld (trees everywhere..!)


// Applies the indicated rotation and flips to the indicated cell
void rotateCell(struct ascoCell *cell, unsigned int rotation, unsigned int flipH, unsigned int flipV);




// ========== DEBUG PRINTING ==========

void printAscoTileMap(struct ascoTileMap *map);


#endif
