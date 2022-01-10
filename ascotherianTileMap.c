
#include "ascotherianTileMap.h"
// ascotherianTileMap.c
// See header for details

#define PRINT_COLOUR_ROCK "\e[38;5;94m"
#define PRINT_COLOUR_WATER "\e[38;5;33m\e[48;5;20m"

struct ascoTile ascoTiles[] = {
    {"Tile_Void",       "  ", "\e[0m",                  ASCO_TILING_MS},        // The edge of the map; blends smoothly with the absence of a tilemap
    {"Tile_Unknown",    "??", "\e[38;5;13m\e[7m",       ASCO_TILING_NONE},      // A tile of unrecognized nature; used as a fallback
    {"Tile_Unresolved", "??", "\e[38;5;240m",           ASCO_TILING_NONE},      // Procedural generation can place anything here as long as it does not obstruct
    {"Tile_Blockage",   "XX", "\e[38;5;160m\e[1m",      ASCO_TILING_NONE},      // Procedural generation can place anything here but it must obstruct

    {"Tile_Blank",      "  ", "",                       ASCO_TILING_NONE},      // A flat floor; from the generator's point of view, obligatorily blank
                                                                                //              (i.e. on a critical path, otherwise use Tile_Unresolved)

    {"Tile_Cliff",      "!!", PRINT_COLOUR_ROCK,        ASCO_TILING_MS},        // Trusty old MS-cliffs; the backbone of any AscoSpel map
    {"Tile_Stair",      "!!", "\e[1m\e[38;5;7m",        ASCO_TILING_MS},

    {"Tile_Water",      "~~", PRINT_COLOUR_WATER,       ASCO_TILING_MS},
    {"Tile_LargeRock",  "!!", PRINT_COLOUR_ROCK,        ASCO_TILING_LARGE},
    // TODO everything else
};






struct ascoTileMap *newAscoTileMap(unsigned int width, unsigned int height){
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


void freeAscoTileMap(struct ascoTileMap *map){
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


void rotateCell(struct ascoCell *cell, unsigned int rotation, unsigned int flipH, unsigned int flipV){
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

        case ASCO_TILING_LARGE:
            // Reacts the same as MS-concave if an anchor corner (TODO NOT QUITE TRUE)
            if(cell->variant == 1){
                if(flipH) cell->rotation = flipHelper(cell->rotation, 0);
                if(flipV) cell->rotation = flipHelper(cell->rotation, 2);
                cell->rotation = (cell->rotation + rotation) % 4;
            }
        break;
    }
}





// ========== DEBUG PRINTING ==========

#define PRINT_COLOUR_BACKGROUND_1 "\e[48;5;238m"
#define PRINT_COLOUR_BACKGROUND_2 "\e[48;5;237m"

static const char * const MSGraphics[3][4] = {
    {"▄▄","█ ","▀▀"," █"},
    {"▄ ","▀ "," ▀"," ▄"},
    {"█▄","█▀","▀█","▄█"}

};

static const char * const stairGraphics[4] = {
    "^=", "]]", "=v", "[["
};

void printAscoTileMap(struct ascoTileMap *map){
    for(int y = 0; y < map->height; y++){
        for(int x = 0; x < map->width; x++){
            struct ascoCell cell = map->cells[(y * map->width) + x];

            printf("%s%s", (x + y) % 2 ? PRINT_COLOUR_BACKGROUND_1 : PRINT_COLOUR_BACKGROUND_2, cell.tile->debugColour);

            if(cell.tile == TILE_STAIR){
                // Stairs get special graphics
                printf("%s\e[0m", stairGraphics[cell.rotation]);
            } else if(cell.tile->tilingType == ASCO_TILING_MS && cell.variant > 0){
                // MS-tile border
                printf("%s\e[0m", MSGraphics[cell.variant - 1][cell.rotation]);
            } else {
                printf("%s\e[0m", cell.tile->debugPrint);
            }
        }
        printf("\e[0m\n");
    }
}
