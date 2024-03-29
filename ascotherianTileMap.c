
#include "ascotherianTileMap.h"
// ascotherianTileMap.c
// See header for details

#define PRINT_COLOUR_ROCK "\e[38;5;94m"
#define PRINT_COLOUR_WATER "\e[38;5;33m\e[48;5;20m"
#define PRINT_COLOUR_ICE "\e[38;5;255m\e[48;5;51m"

struct ascoTile ascoTiles[] = {
    {"Tile_Void",       "  ", "\e[0m",                  ASCO_TILING_NONE},
    {"Tile_Unknown",    "??", "\e[38;5;13m\e[7m",       ASCO_TILING_NONE},
    {"Tile_Unresolved", "??", "\e[38;5;240m",           ASCO_TILING_NONE},
    {"Tile_Blockage",   "XX", "\e[38;5;160m\e[1m",      ASCO_TILING_NONE},

    {"Tile_Blank",      "  ", "",                       ASCO_TILING_NONE},

    {"Tile_Cliff",      "██", PRINT_COLOUR_ROCK,        ASCO_TILING_MS},
    {"Tile_Stair",      "!!", "\e[1m\e[38;5;7m",        ASCO_TILING_ROTOR},
    {"Tile_Ledge",      "!!", "\e[1m\e[38;5;7m",        ASCO_TILING_MS},

    {"Tile_Water",      "~~", PRINT_COLOUR_WATER,       ASCO_TILING_MS},
    {"Tile_Ice",        "//", PRINT_COLOUR_ICE,         ASCO_TILING_NONE},  // TODO MS-tiling...?

    {"Tile_Rock_Small", "/\\",PRINT_COLOUR_ROCK,        ASCO_TILING_NONE},
    {"Tile_Rock_Tall",  "/\\",PRINT_COLOUR_ROCK,        ASCO_TILING_NONE},
    {"Tile_Rock_Large", "!!", PRINT_COLOUR_ROCK,        ASCO_TILING_LARGE},

    {"Tile_Door",       "Do", "\e[1m\e[38;5;154m",      ASCO_TILING_ROTOR},
    {"Tile_Ladder",     "HH", "\e[1m\e[38;5;154m",      ASCO_TILING_NONE},
    {"Tile_Pit",        "[]", "\e[1m\e[38;5;154m",      ASCO_TILING_NONE},

    {"Tile_Rock_Smash",     "{}", "\e[1m\e[38;5;184m",  ASCO_TILING_NONE},
    {"Tile_Rock_Strength",  "()",",\e[1m\e[38;5;250m",  ASCO_TILING_NONE},

    {"Tile_NPC_Placeholder","NP", "\e[1m\e[48;5;208m\e[38;5;0m",    ASCO_TILING_ROTOR},
    {"Tile_Item_Placeholder","it", "\e[1m\e[38;5;154m",             ASCO_TILING_NONE},
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
    switch(ascoTiles[cell->tile].tilingType){

        case ASCO_TILING_NONE:
            // Nontiling tiles don't react to being rotated or flipped
        break;

        case ASCO_TILING_ROTOR:
            // Rotors react to flipping as a form of rotation but no more, akin to MS-straight
            if(flipH) cell->rotation = flipHelper(cell->rotation, 3);
            if(flipV) cell->rotation = flipHelper(cell->rotation, 1);
            cell->rotation = (cell->rotation + rotation) % 4;
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
                case 5:
                    // LH and RH Terminii
                    if(flipH){
                        cell->rotation ^= (cell->rotation & 1) ? 2 : 0;
                        cell->variant = (cell->variant == 4) ? 5 : 4;
                    }
                    if(flipV){
                        cell->rotation ^= (cell->rotation & 1) ? 0 : 2;
                        cell->variant = (cell->variant == 4) ? 5 : 4;
                    }
                    cell->rotation = (cell->rotation + rotation) % 4;
                break;
            }
        break;

        case ASCO_TILING_FENCE:
            // TODO
        break;

        case ASCO_TILING_LARGE:
            // Reacts the same as MS-concave if an anchor corner
            if(cell->variant == 1){
                if(flipH) cell->rotation = flipHelper(cell->rotation, 0);
                if(flipV) cell->rotation = flipHelper(cell->rotation, 2);
                cell->rotation = (cell->rotation + rotation) % 4;
            } // Otherwise doesn't react
        break;
    }
}





// ========== DEBUG PRINTING ==========

#define PRINT_COLOUR_BACKGROUND_1 "\e[48;5;238m"
#define PRINT_COLOUR_BACKGROUND_2 "\e[48;5;237m"

static const char * const MSGraphics[5][4] = {
    {"▄▄","█ ","▀▀"," █"},
    {"▄ ","▀ "," ▀"," ▄"},
    {"█▄","█▀","▀█","▄█"},
    {"^▄","_>","▀v","<-"},
    {"▄^","->","v▀","<_"}
};

static const char * const stairGraphics[4] = {
    "^=", "]]", "=v", "[["
};

static const char * const doorGraphics[4] = {
    "D^", "D>", "Dv", "<D"
};

static const char * const NPCGraphics[4] = {
    "N^", "N>", "Nv", "<N"
};

static const char * const largeRockGraphics[4] = {
    "`\\",  ".|",  "|.",  "/`"
};


void printAscoTileMap(struct ascoTileMap *map){
    for(int y = 0; y < map->height; y++){
        for(int x = 0; x < map->width; x++){
            struct ascoCell cell = map->cells[(y * map->width) + x];

            printf("%s%s", (x + y) % 2 ? PRINT_COLOUR_BACKGROUND_1 : PRINT_COLOUR_BACKGROUND_2, ascoTiles[cell.tile].debugColour);

            switch(cell.tile){
                case TILE_STAIR:
                    // Stairs get special graphics
                    printf("%s\e[0m", stairGraphics[cell.rotation]);
                break;
                case TILE_DOOR:
                    // Doors get special graphics
                    printf("%s\e[0m", doorGraphics[cell.rotation]);
                break;
                case TILE_NPC_PLACEHOLDER:
                    // NPCs get special graphics
                    printf("%s\e[0m", NPCGraphics[cell.rotation]);
                break;
                case TILE_ROCK_LARGE:
                    // Large Rocks get special graphics
                    printf("\e[1m%s\e[0m", largeRockGraphics[cell.rotation]);
                break;
                default:
                    if(ascoTiles[cell.tile].tilingType == ASCO_TILING_MS && cell.variant > 0){
                        // MS-tile border
                        printf("%s\e[0m", MSGraphics[cell.variant - 1][cell.rotation]);
                    } else {
                        printf("%s\e[0m", ascoTiles[cell.tile].debugPrint);
                    }
                break;
            }
        }
        printf("\e[0m\n");
    }
}
