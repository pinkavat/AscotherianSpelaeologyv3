#ifndef ASCOTHERIAN_TILE_MAP
#define ASCOTHERIAN_TILE_MAP

#include <stdio.h>
#include <stdlib.h>

/* ascotherianTileMap.h
*
*   Header-only library for unifying all my procedural-generation and rendering projects for the Ascotherian World under one 
*   global tile system because I'm sick and tired of rewriting "Marching Squares" every ten minutes.
*
*   TODO: mapfile IO
*   TODO: find some way of excising the debug data from a deployment header?
*   TODO: add a "debug flag" to the tiles, such that they are highlighted in debug print???
*   TODO: map validation?: some tiles declare membership in a tiling type but don't have meaningful tiles for certain variants (e.g. no convex stairs (yet))
*
* written December 2021 as part of AscoSpel v3
*/


// ========== STRUCTURE ==========

enum ascoTilingTypes {
    ASCO_TILING_NONE,       // Does not tile                        (Center)
    ASCO_TILING_MS,         // MS tiling with center and terminii   (Center, Straight, Convex, Concave, Left-hand terminus, Right-hand terminus)
    ASCO_TILING_FENCE,      // Direct connectivity                  (5 degrees)
    ASCO_TILING_ANCHOR      // TODO document better
};


// A tile (type)
struct ascoTile {
    const char * name;                  // Printable name for reference
    const char * debugPrint;            // Printable representation of tile for debugging (different tiling types override this sometimes, e.g. on water edges)
    const char * debugColour;           // Terminal colour to set when printing this tile for debugging (whether or no the above string is overridden)

    enum ascoTilingTypes tilingType;    // The tiling type of this tile. Tells us how many variants to expect and how to rotate/mirror the tile.
    
    // TODO uint8_t width, height;      // Only meaningful if the tiling type is ASCO_TILING_ANCHOR, which means that this tile spans multiple cells.
                                        // Used when rotating or mirroring multicell tiles to replace 

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
// Placed in this file for convenience; could be placed elsewhere, but I've learned the hard way the curse of premodularization.

#define PRINT_COLOUR_ROCK "\e[38;5;94m"
#define PRINT_COLOUR_WATER "\e[38;5;33m\e[48;5;20m"

static struct ascoTile ascoTiles[] = {
    {"Tile_Void",       "  ", "\e[0m",                  ASCO_TILING_MS},        // The edge of the map; blends smoothly with the absence of a tilemap
    {"Tile_Unknown",    "??", "\e[38;5;13m\e[7m",       ASCO_TILING_NONE},      // A tile of unrecognized nature; used as a fallback
    {"Tile_Unresolved", "??", "\e[38;5;240m",           ASCO_TILING_NONE},      // Procedural generation can place anything here as long as it does not obstruct
    {"Tile_Blockage",   "XX", "\e[38;5;160m\e[1m",      ASCO_TILING_NONE},      // Procedural generation can place anything here but it must obstruct

    {"Tile_Blank",      "  ", "",                       ASCO_TILING_NONE},      // A flat floor; from the generator's point of view, obligatorily blank
                                                                                //              (i.e. on a critical path, otherwise use Tile_Unresolved)

    {"Tile_Cliff",      "!!", PRINT_COLOUR_ROCK,        ASCO_TILING_MS},
    {"Tile_Stair",      "!!", "\e[1m\e[38;5;7m",        ASCO_TILING_MS},

    {"Tile_Water",      "~~", PRINT_COLOUR_WATER,       ASCO_TILING_MS}
    // TODO everything else
};

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
static struct ascoTileMap *newAscoTileMap(unsigned int width, unsigned int height){
    struct ascoTileMap *newMap = (void *)malloc(sizeof(struct ascoTileMap));
    newMap->width = width;
    newMap->height = height;
    newMap->cells = (void *)malloc(sizeof(struct ascoCell) * width * height);
    struct ascoCell blankCell = {TILE_VOID, 0, 0, 0};
    for(int i = 0; i < width * height; i++){
        newMap->cells[i] = blankCell;
    }
    return newMap;
}

static void freeAscoTileMap(struct ascoTileMap *map){
    free(map->cells);
    free(map);
}





// ========== TILE ROTATION HANDLING ==========

// TODO VERY IMPORTANT: Anchored supertiles "clobber" other parts of the map upon rotation.
// THAT'S A BAD DEPENDENCY, WE NEED TO THINK ABOUT THIS FROM A DIFFERENT ANGLE.
// We can't easily abandon multicell tiles in the overworld (trees everywhere..!)


// Helper macro for rotateCell (lifted from v2)
// TODO There's some sort of pattern implied here, but I'm too lazy to work it out. Let the computer foot the bill.
#define flipHelper(r, n) (3 - ((r) + (n)) % 4)


// Applies the indicated rotation and flips to the indicated cell
static void rotateCell(struct ascoCell *cell, unsigned int rotation, unsigned int flipH, unsigned int flipV){

    // Effects of rotation/flipping depend on tiling type
    switch(cell->tile->tilingType){

        case ASCO_TILING_NONE:
            // Nontiling tiles don't react to being rotated or flipped
        break;

        case ASCO_TILING_MS:
            switch(cell->variant){
                case 0:
                    // Centers of MS-tiles don't rotate or flip
                break;
                case 1:
                    // Straight
                    if(flipH) cell->rotation = flipHelper(cell->rotation, 3);
                    if(flipV) cell->rotation = flipHelper(cell->rotation, 1);
                    cell->rotation = (cell->rotation + rotation) % 4;
                break;
                case 2:
                case 3:
                    // Convex or Concave
                    if(flipH) cell->rotation = flipHelper(cell->rotation, 0);
                    if(flipV) cell->rotation = flipHelper(cell->rotation, 2);
                    cell->rotation = (cell->rotation + rotation) % 4;
                break;
                case 4:
                    // TODO LH Terminus
                break;
                case 5:
                    // TODO RH Terminus
                break;
            }
        break;

        case ASCO_TILING_FENCE:
            // TODO
        break;

        case ASCO_TILING_ANCHOR:
            // TODO
        break;
    }
}




// ========== DEBUG PRINTING ==========

#define PRINT_COLOUR_BACKGROUND "\e[48;5;238m"

static const char * const MSGraphics[3][4] = {
    {"▄▄","█ ","▀▀"," █"},
    {"▄ ","▀ "," ▀"," ▄"},
    {"█▄","█▀","▀█","▄█"}

};

static const char * const stairGraphics[4] = {
    "^=", "]]", "=v", "[["
};

static void printAscoTileMap(struct ascoTileMap *map){
    for(int y = 0; y < map->height; y++){
        for(int x = 0; x < map->width; x++){
            struct ascoCell cell = map->cells[(y * map->width) + x];

            printf("%s%s", PRINT_COLOUR_BACKGROUND, cell.tile->debugColour);

            if(cell.tile->tilingType == ASCO_TILING_MS && cell.variant > 0){
                // MS-tile border
                printf("%s\e[0m", MSGraphics[cell.variant - 1][cell.rotation]);
            } else {
                printf("%s\e[0m", cell.tile->debugPrint);
            }
        }
        printf("\e[0m\n");
    }
}





#endif
