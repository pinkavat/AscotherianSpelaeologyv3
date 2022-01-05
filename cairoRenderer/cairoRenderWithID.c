#include "cairoRenderWithID.h"

/* cairoRenderWithID.c:
 *
 * a test of Cairo's ability to draw a 2.5D topdown tilemap for prototype rendering purposes (Pokemon Cave Project)
 *
 * prototype written June 2020
 * cart replaced behind horse September 2020
 *
 * Dragged kicking and screaming into AscoSpel v3 December 2021
 *
 * uses cairo (https://www.cairographics.org)
 * 
*/

#define OUTFILE "cave.png" // May make this a parameter eventually
#define TILE_FILE "cairoRenderer/CairoGen4TileSet.png"    // NOTE: placed in subdir called cairoRenderer

// Shorthand defines for indices in the below array
#define TILE_FLOOR_ACCESSIBLE 13
#define TILE_CLIFF_TOP 9
#define TILE_CLIFF_RIGHT 10
#define TILE_CLIFF_BOTTOM 11
#define TILE_CLIFF_LEFT 12

// X, Y, Width, Height, X offset, Y offset, and special layer data for each tile on the tilemap image
static const int tileSetData[][7] = {
    {16, 44, 16, 12,   0, 0, 0},     // Void

    {48,  0, 16, 12,   0, 0, 0},     // TL Convex
    {48, 28, 16, 12,   0, 0, 0},     // TR Convex
    {32, 34, 16, 22, 0, -10, 0},     // BL Convex
    { 0, 34, 16, 22, 0, -10, 0},     // BR Convex

    { 0,  0, 16, 22,   0, 0, 0},     // BR Concave
    {32,  0, 16, 22,   0, 0, 0},     // BL Concave
    {48, 42, 16, 14, 0, -12, 0},     // TR Concave
    {48, 14, 16, 14, 0, -12, 0},     // TL Concave

    {16, 34, 16,  2,   0, 0, 0},     // Top
    { 0, 22, 16, 12,   0, 0, 0},     // Right
    {16,  0, 16, 22,   0, 0, 0},     // Bottom
    {32, 22, 16, 12,   0, 0, 0},     // Left

    {16, 22, 16, 12,   0, 0, 0},     // Accessible Floor
    {64, 44, 16, 12,   0, 0, 0},     // Inaccessible Floor

    {80,  4, 16, 15,  0, -3, TILE_FLOOR_ACCESSIBLE},     // Small Rock
    {96,  4, 16, 24,  0, -12, TILE_FLOOR_ACCESSIBLE},    // Tall Rock
    {112, 0, 32, 36,  0, -11, TILE_FLOOR_ACCESSIBLE},    // Large Rock TL corner (only this part needed for render)

    {80, 19, 16,  3,   0, 0, TILE_CLIFF_TOP},     // Top Stairs
    {80, 22, 16, 21,   0, -12, TILE_CLIFF_RIGHT},     // Right Stairs   // USED TO BE 9
    {64, 0,  16, 22,   0, 0, TILE_CLIFF_BOTTOM},     // Bottom Stairs
    {64, 22, 16, 21,   0, -12, TILE_CLIFF_LEFT},     // Left Stairs     // USED TO BE 9

    {64, 98, 48, 7,   -16, -5, TILE_CLIFF_TOP},    // Interior Door Top
    {64, 56, 38, 42,   -16, -18, TILE_CLIFF_RIGHT},    // Interior Door Right
    {140, 56, 48, 48,   -16, -22, TILE_CLIFF_BOTTOM},    // Interior Door Bottom
    {102, 56, 38, 42,   -6, -18, TILE_CLIFF_LEFT},    // Interior Door Left

    {32,128, 16, 12,   0, 0, 0},     // TL Convex Water
    {0, 128, 16, 12,   0, 0, 0},     // TR Convex Water
    {32,116, 16, 12,   0, 0, 0},     // BL Convex Water
    {0, 116, 16, 12,   0, 0, 0},     // BR Convex Water
    {0,  92, 16, 12,   0, 0, 0},     // BR Concave Water
    {32, 92, 16, 12,   0, 0, 0},     // BL Concave Water
    {0, 140, 16, 12,   0, 0, 0},     // TR Concave Water
    {32,140, 16, 12,   0, 0, 0},     // TL Concave Water
    {16,140, 16, 12,   0, 0, 0},     // Top Water
    {0, 104, 16, 12,   0, 0, 0},     // Right Water
    {16, 92, 16, 12,   0, 0, 0},     // Bottom Water
    {32,104, 16, 12,   0, 0, 0},     // Left Water
    {16,104, 16, 12,   0, 0, 0},     // Center Water
    
    {80, 44, 16, 12,   0, 0, 0},     // Placeholder tile

    {96, 31,  8,  3,  11, 0, 0},     // Top Stair Widener
    {119,36, 15, 20,   1,-3, 0},     // Right Stair Widener
    {96, 34,  8, 22,  11, 0, 0},     // Bottom Stair Widener
    {104,36, 15, 20,   0,-3, 0},     // Left Stair Widener

    {144, 5, 24, 50,  -2,-36, TILE_FLOOR_ACCESSIBLE},   // Ladder Ascending
    {168,24, 26, 23,  -5, -5, TILE_FLOOR_ACCESSIBLE},   // Ladder Descending
    {168, 0, 26, 23,  -5, -5, TILE_FLOOR_ACCESSIBLE},   // Pit
    
    {48, 56, 16, 12,   0, 0, 0},     // Top Ledge (basic)
    {16, 68, 16, 12,   0, 0, 0},     // Right Ledge (basic)
    {48, 68, 16, 12,   0, 0, 0},     // Bottom Ledge (basic)
    { 0, 68, 16, 12,   0, 0, 0},     // Left Ledge (basic)

    {32, 56, 16, 12,   0, 0, 0},      // Top Left Edge
    {48, 80, 16, 12,   0, 0, 0},      // Top Right Edge
    {16, 56, 16, 12,   0, 0, 0},      // Right Top Edge
    {16, 80, 16, 12,   0, 0, 0},      // Right Bottom Edge
    {32, 68, 16, 12,   0, 0, 0},      // Bottom Left Edge
    {32, 80, 16, 12,   0, 0, 0},      // Bottom Right Edge
    { 0, 56, 16, 12,   0, 0, 0},      // Left Top Edge
    { 0, 80, 16, 12,   0, 0, 0},      // Left Bottom Edge

    {48,105, 16, 12,   0, 0, 0},      // Bridge Hzont top   
    {48,117, 16, 12,   0, 0, 0},      // Bridge Hzont mid
    {48,129, 16, 12,   0, 0, 0},      // Bridge Hzont bottom

    {64,105, 16, 12,   0, 0, 0},      // Bridge Vert left
    {80,105, 16, 12,   0, 0, 0},      // Bridge Vert mid
    {96,105, 16, 12,   0, 0, 0},      // Bridge Ver right

    {64,117, 16, 15,   0, -4, TILE_FLOOR_ACCESSIBLE},   // Smashable rock 65
    {80,117, 16, 16,   0, -4, TILE_FLOOR_ACCESSIBLE},   // pushable rock 66

    {48, 93, 16, 12,   0,  0, TILE_FLOOR_ACCESSIBLE},   // Visible Item pickup 67

};


static int tileMapToImageData(struct ascoCell cell){
    return 13;
}


/* TODO old translator; eliminate
// Translate tileIDs.h tile IDs to the above array index
static int tileMapToImageData(int tile){
    switch(tile){
        case TILE_VOID: return 0;
        case TILE_BLOCKAGE: return tileMapToImageData(TILE_PLACEHOLDER);
        case TILE_FLOOR_ACCESSIBLE: return 13;
        case TILE_FLOOR_INACCESSIBLE: return 14;
        case TILE_STAIR_TOP: return 18;
        case TILE_STAIR_RIGHT: return 19;
        case TILE_STAIR_BOTTOM: return 20;
        case TILE_STAIR_LEFT: return 21;
        case TILE_CLIFF_TL_CONVEX: return 1;
        case TILE_CLIFF_TR_CONVEX: return 2;
        case TILE_CLIFF_BL_CONVEX: return 3;
        case TILE_CLIFF_BR_CONVEX: return 4;
        case TILE_CLIFF_BR_CONCAVE: return 5;
        case TILE_CLIFF_BL_CONCAVE: return 6;
        case TILE_CLIFF_TR_CONCAVE: return 7;
        case TILE_CLIFF_TL_CONCAVE: return 8;
        case TILE_CLIFF_TOP: return 9;
        case TILE_CLIFF_RIGHT: return 10;
        case TILE_CLIFF_BOTTOM: return 11;
        case TILE_CLIFF_LEFT: return 12;
        case TILE_ROCK_SMALL: return 15;
        case TILE_ROCK_TALL: return 16;
        case TILE_ROCK_LARGE_TL: return 17;         // TODO large rock non-TL maps to blank floor (non-modular for floor types)
        case TILE_ROCK_LARGE_TR: return 13;
        case TILE_ROCK_LARGE_BL: return 13;
        case TILE_ROCK_LARGE_BR: return 13;
        case TILE_EXTERIOR_DOOR_TOP: return 22;      // TODO currently maps to interior doors
        case TILE_EXTERIOR_DOOR_RIGHT: return 23;
        case TILE_EXTERIOR_DOOR_BOTTOM: return 24;
        case TILE_EXTERIOR_DOOR_LEFT: return 25;
        case TILE_INTERIOR_DOOR_TOP: return 22;
        case TILE_INTERIOR_DOOR_RIGHT: return 23;
        case TILE_INTERIOR_DOOR_BOTTOM: return 24;
        case TILE_INTERIOR_DOOR_LEFT: return 25;
        case TILE_LADDER_ASCENDING: return 44;
        case TILE_LADDER_DESCENDING: return 45;
        case TILE_PIT: return 46;
        case TILE_LEDGE_TOP_LEFT: return 51;
        case TILE_LEDGE_TOP_CENTER: return 47;
        case TILE_LEDGE_TOP_RIGHT: return 52;
        case TILE_LEDGE_RIGHT_TOP: return 53;
        case TILE_LEDGE_RIGHT_CENTER: return 48;
        case TILE_LEDGE_RIGHT_BOTTOM: return 54;
        case TILE_LEDGE_BOTTOM_RIGHT: return 56;
        case TILE_LEDGE_BOTTOM_CENTER: return 49;
        case TILE_LEDGE_BOTTOM_LEFT: return 55;
        case TILE_LEDGE_LEFT_BOTTOM: return 58;
        case TILE_LEDGE_LEFT_CENTER: return 50;
        case TILE_LEDGE_LEFT_TOP: return 57;
        case TILE_WATER_TL_CONVEX: return 26;
        case TILE_WATER_TR_CONVEX: return 27;
        case TILE_WATER_BL_CONVEX: return 28;
        case TILE_WATER_BR_CONVEX: return 29;
        case TILE_WATER_BR_CONCAVE: return 30;
        case TILE_WATER_BL_CONCAVE: return 31;
        case TILE_WATER_TR_CONCAVE: return 32;
        case TILE_WATER_TL_CONCAVE: return 33;
        case TILE_WATER_TOP: return 34;
        case TILE_WATER_RIGHT: return 35;
        case TILE_WATER_BOTTOM: return 36;
        case TILE_WATER_LEFT: return 37;
        case TILE_WATER_CENTER: return 38;
        case TILE_PLACEHOLDER: return 39;
        case TILE_ITEM_PICKUP: return 67;
        case TILE_SMASHABLE_ROCK: return 65;
        case TILE_PUSHABLE_ROCK: return 66;
        default: return tileMapToImageData(TILE_PLACEHOLDER);
    }
}
*/



// Draw a tile
static void drawTile(cairo_t *cr, cairo_surface_t *tileSet, int x, int y, unsigned int tile){
    cairo_set_source_surface(cr, tileSet, x - tileSetData[tile][0], y - tileSetData[tile][1]);
    cairo_rectangle(cr, x, y, tileSetData[tile][2], tileSetData[tile][3]);
    cairo_fill(cr);
}


// Draw a bridge
// Direction param: 1 if horizontal, 0 if vertical
static void drawBridge(cairo_t *cr, cairo_surface_t *tileSet, int startX, int startY, int width, int height, unsigned int direction){

    if(direction)startY -= 12;  // horizontal bridges render too low for some reason

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            int tile;
            if(direction){
                // Horizontal
                if(y == 0){
                    tile = 59;
                } else if(y == height - 1){
                    tile = 61;
                } else {
                    tile = 60;
                }
            } else {
                // Vertical
                if(x == 0){
                    tile = 62;
                } else if(x == width - 1){
                    tile = 64;
                } else {
                    tile = 63;
                }
            }
            drawTile(cr, tileSet, startX + x*16, startY + y*12, tile);
        }
    }
}



#define BUFFER_X 16
#define BUFFER_Y 12


void cairoRenderMap(struct ascoTileMap *map){

    // Prepare arrays of y-offsets and skips
    int *yOffset = (int *)malloc(map->width *  sizeof(int));
    for(int i = 0; i < map->width; i++){
        yOffset[i] = BUFFER_Y;
    }
    
    // Initialize the cairo context
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, map->width * 16 + (2 * BUFFER_X), map->height * 12 + (2 * BUFFER_Y));
    cairo_surface_t *specialLayer = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, map->width * 16 + (2 * BUFFER_X), map->height * 12 + (2 * BUFFER_Y));
    cairo_surface_t *stairCoverLayer = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, map->width * 16 + (2 * BUFFER_X), map->height * 12 + (2 * BUFFER_Y));
    cairo_surface_t *bridgeLayer = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, map->width * 16 + (2 * BUFFER_X), map->height * 12 + (2 * BUFFER_Y));
    cairo_t *scr = cairo_create(specialLayer);
    cairo_t *tcr = cairo_create(stairCoverLayer);
    cairo_t *bcr = cairo_create(bridgeLayer);
    cairo_t *cr = cairo_create(surface);

    // Blackout background
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    // Load tileset
    cairo_surface_t *tileSet = cairo_image_surface_create_from_png(TILE_FILE);

    // TODO TODO TODO ================= VOLATILE AREA ONE

    // Render base tile data
    for(int y = 0; y < map->height; y++){
        for(int x = 0; x < map->width; x++){

            /* TODO bridge handler needs conversion
            if(map->cells[(y*map->width)+x].flags & CELL_FLAG_BRIDGE_ANCHOR
                && ( (map->cells[(y * map->width)+x].tile == TILE_CLIFF_RIGHT && !(map->cells[(y-1)*map->width+x].flags & CELL_FLAG_BRIDGE_ANCHOR)) ||
                     (map->cells[(y * map->width)+x].tile == TILE_CLIFF_BOTTOM && !(map->cells[(y*map->width)+x-1].flags & CELL_FLAG_BRIDGE_ANCHOR)) )){
                // Launch a bridge-drawing unit
                // Determine which way the bridge is facing (assumes TL-BR traversal and no malformed bridges)
                unsigned int direction = (map->cells[(y * map->width) + x].tile == TILE_CLIFF_RIGHT);
                // Determine the bridge's dimensions
                int bridgeWidth = 2, bridgeHeight = 2;
                if(direction){
                    // Horizontal
                    bridgeHeight = 2;   // TODO bridge size limited
                    for(int runnerX = x + 1; (runnerX < map->width) && !(map->cells[(y*map->width)+runnerX].flags & CELL_FLAG_BRIDGE_ANCHOR);runnerX++) bridgeWidth++;
                } else {
                    // Vertical
                    bridgeWidth = 2;    // TODO bridge size limited
                    for(int runnerY = y + 1; (runnerY < map->height) && !(map->cells[(runnerY*map->width)+x].flags & CELL_FLAG_BRIDGE_ANCHOR);runnerY++) bridgeHeight++;
                }
                drawBridge(bcr, tileSet, x * 16 + BUFFER_X, yOffset[x], bridgeWidth, bridgeHeight, direction);
            }*/

            // Translate tile to Image tileset data
            // OLD VERSION int tile = tileMapToImageData(map->cells[(y*map->width)+x].tile);
            int tile = tileMapToImageData(mapCell(map, x, y));

            if(tileSetData[tile][6] > 0){
                // Tile needs another tile printed beneath
                // First draw underlying tile (TODO floor-data instead...)
                int subTile = tileSetData[tile][6];
                yOffset[x] += tileSetData[subTile][5];
                drawTile(cr, tileSet, x * 16 + BUFFER_X + tileSetData[subTile][4], yOffset[x], subTile);
                // Then draw the tile
                /* TODO handling of ladder here needs conversion
                drawTile((map->cells[(y*map->width)+x].tile == TILE_LADDER_ASCENDING) ? tcr : scr, 
                    tileSet, x * 16 + BUFFER_X + tileSetData[tile][4], yOffset[x] + tileSetData[tile][5], tile);
                */
                drawTile(scr, tileSet, x * 16 + BUFFER_X + tileSetData[tile][4], yOffset[x] + tileSetData[tile][5], tile);

                /* TODO widened stair check needs conversion (removal; stairs are now terminated in-map)
                // Check for widened stairs for special stair graphics
                if(map->cells[y*map->width+x].tile == TILE_STAIR_TOP && map->cells[y*map->width+x+1].tile == TILE_STAIR_TOP){
                    // Draw widened top stair
                    drawTile(tcr, tileSet, x*16 + BUFFER_X + tileSetData[40][4], yOffset[x]+ tileSetData[40][5], 40);
                } else if(map->cells[y*map->width+x].tile == TILE_STAIR_BOTTOM && map->cells[y*map->width+x+1].tile == TILE_STAIR_BOTTOM){
                    // Draw widened bottom stair
                    drawTile(tcr, tileSet, x*16 + BUFFER_X + tileSetData[42][4], yOffset[x]+ tileSetData[42][5], 42);
                } else if(map->cells[y*map->width+x].tile == TILE_STAIR_RIGHT && map->cells[(y+1)*map->width+x].tile == TILE_STAIR_RIGHT){
                    // Draw widened right stair
                    drawTile(tcr, tileSet, x*16 + BUFFER_X + tileSetData[41][4], yOffset[x]+ tileSetData[41][5], 41);
                } else if(map->cells[y*map->width+x].tile == TILE_STAIR_LEFT && map->cells[(y+1)*map->width+x].tile == TILE_STAIR_LEFT){
                    // Draw widened right stair
                    drawTile(tcr, tileSet, x*16 + BUFFER_X + tileSetData[43][4], yOffset[x]+ tileSetData[43][5], 43);
                }
                */

                // Then move on
                yOffset[x] += tileSetData[subTile][3];
            } else {
                // Regular-handling rock tile
                yOffset[x] += tileSetData[tile][5];
                drawTile(cr, tileSet, x * 16 + BUFFER_X + tileSetData[tile][4], yOffset[x], tile);
                yOffset[x] += tileSetData[tile][3];

            }



        }
    }

    // TODO TODO TODO END VOLATILE AREA ONE =========================





    // Paste the special layer atop the rock layer
    cairo_set_source_surface(cr, specialLayer, 0, 0);
    cairo_paint(cr);

    // Paste the stair cover layer atop that
    cairo_set_source_surface(cr, stairCoverLayer, 0, 0);
    cairo_paint(cr);

    // Paste the bridge layer atop that
    cairo_set_source_surface(cr, bridgeLayer, 0, 0);
    cairo_paint(cr);


    // Output the cairo context to the given PNG file and clean up
    cairo_destroy(cr);
    cairo_destroy(scr);
    cairo_destroy(tcr);
    cairo_destroy(bcr);
    cairo_surface_write_to_png(surface, OUTFILE);
    cairo_surface_destroy(surface);
    cairo_surface_destroy(specialLayer);
    cairo_surface_destroy(stairCoverLayer);
    cairo_surface_destroy(bridgeLayer);

    fprintf(stderr, "Image saved as \"");
    fprintf(stderr, OUTFILE);
    fprintf(stderr, "\"\n");

    free(yOffset);
}
