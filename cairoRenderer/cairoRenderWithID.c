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


// X, Y, Width, Height, X offset, Y offset, and special layer data for each tile on the tilemap image
static const int tileSetData[][7] = {
    {16, 44, 16, 12,   0, 0, 0},     // 0: Void

    {16, 34, 16,  2,   0, 0, 0},     // 1: Top
    { 0, 22, 16, 12,   0, 0, 0},     // 2: Right
    {16,  0, 16, 22,   0, 0, 0},     // 3: Bottom
    {32, 22, 16, 12,   0, 0, 0},     // 4: Left

    {48, 28, 16, 12,   0, 0, 0},     // 5: TR Convex
    { 0, 34, 16, 22, 0, -10, 0},     // 6: BR Convex
    {32, 34, 16, 22, 0, -10, 0},     // 7: BL Convex
    {48,  0, 16, 12,   0, 0, 0},     // 8: TL Convex

    {48, 42, 16, 14, 0, -12, 0},     // 9: TR Concave
    { 0,  0, 16, 22,   0, 0, 0},     // 10: BR Concave
    {32,  0, 16, 22,   0, 0, 0},     // 11: BL Concave
    {48, 14, 16, 14, 0, -12, 0},     // 12: TL Concave


    {16, 22, 16, 12,   0, 0, 0},     // 13: Accessible Floor
    {64, 44, 16, 12,   0, 0, 0},     // 14: Inaccessible Floor

    {80,  4, 16, 15,  0, -3, 13},    // 15: Small Rock
    {96,  4, 16, 24,  0, -12, 13},   // 16: Tall Rock
    {112, 0, 32, 36,  0, -11, 13},   // 17: Large Rock TL corner (only this part needed for render)

    {80, 19, 16,  3,   0, 0, 1},     // 18: Top Stairs
    {80, 22, 16, 21,   0, -12, 2},   // 19: Right Stairs
    {64, 0,  16, 22,   0, 0, 3},     // 20: Bottom Stairs
    {64, 22, 16, 21,   0, -12, 4},   // 21: Left Stairs

    {64, 98, 48, 7,   -16, -5, 1},   // 22: Interior Door Top
    {64, 56, 38, 42,   -16, -18, 2}, // 23: Interior Door Right
    {140, 56, 48, 48,   -16, -22, 3},// 24: Interior Door Bottom
    {102, 56, 38, 42,   -6, -18, 4}, // 25: Interior Door Left

    {16,104, 16, 12,   0, 0, 0},     // 26: Center Water

    {16,140, 16, 12,   0, 0, 0},     // 27: Top Water
    {0, 104, 16, 12,   0, 0, 0},     // 28: Right Water
    {16, 92, 16, 12,   0, 0, 0},     // 29: Bottom Water
    {32,104, 16, 12,   0, 0, 0},     // 30: Left Water

    {0, 128, 16, 12,   0, 0, 0},     // 31: TR Convex Water
    {0, 116, 16, 12,   0, 0, 0},     // 32: BR Convex Water
    {32,116, 16, 12,   0, 0, 0},     // 33: BL Convex Water
    {32,128, 16, 12,   0, 0, 0},     // 34: TL Convex Water

    {0, 140, 16, 12,   0, 0, 0},     // 35: TR Concave Water
    {0,  92, 16, 12,   0, 0, 0},     // 36: BR Concave Water
    {32, 92, 16, 12,   0, 0, 0},     // 37: BL Concave Water
    {32,140, 16, 12,   0, 0, 0},     // 38: TL Concave Water
    
    {80, 44, 16, 12,   0, 0, 0},     // 39: Placeholder tile

    {96, 31,  8,  3,  11, 0, 0},     // 40: Top Stair Widener
    {119,36, 15, 20,   1,-3, 0},     // 41: Right Stair Widener
    {96, 34,  8, 22,  11, 0, 0},     // 42: Bottom Stair Widener
    {104,36, 15, 20,   0,-3, 0},     // 43: Left Stair Widener

    {194,19, 16, 12,   0, 0, 0},     // 44: "Unresolved" floor
    {194,4, 16, 15,  0, -3, 13},     // 45: "Blockage" rock

    {144, 5, 24, 50,  -2,-36, 13},   // 46: Ladder Ascending
    {168,24, 26, 23,  -5, -5, 13},   // 47: Ladder Descending
    {168, 0, 26, 23,  -5, -5, 13},   // 48: Pit
    
    {48, 56, 16, 12,   0, 0, 13},     // 49: Top Ledge (basic)
    {16, 68, 16, 12,   0, 0, 13},     // 50: Right Ledge (basic)
    {48, 68, 16, 12,   0, 0, 13},     // 51: Bottom Ledge (basic)
    { 0, 68, 16, 12,   0, 0, 13},     // 52: Left Ledge (basic)

    {32, 56, 16, 12,   0, 0, 13},      // 53: Top Left Edge (ledge parts)
    {48, 80, 16, 12,   0, 0, 13},      // 54: Top Right Edge
    {16, 56, 16, 12,   0, 0, 13},      // 55: Right Top Edge
    {16, 80, 16, 12,   0, 0, 13},      // 56: Right Bottom Edge
    {32, 80, 16, 12,   0, 0, 13},      // 57: Bottom Right Edge
    {32, 68, 16, 12,   0, 0, 13},      // 58: Bottom Left Edge
    { 0, 80, 16, 12,   0, 0, 13},      // 59: Left Bottom Edge
    { 0, 56, 16, 12,   0, 0, 13},      // 60: Left Top Edge

    {48,105, 16, 12,   0, 0, 0},      // 61: Bridge Hzont top   
    {48,117, 16, 12,   0, 0, 0},      // 62: Bridge Hzont mid
    {48,129, 16, 12,   0, 0, 0},      // 63: Bridge Hzont bottom

    {64,105, 16, 12,   0, 0, 0},      // 64: Bridge Vert left
    {80,105, 16, 12,   0, 0, 0},      // 65: Bridge Vert mid
    {96,105, 16, 12,   0, 0, 0},      // 66: Bridge Ver right

    {64,117, 16, 15,   0, -4, 13},   // 67: Smashable rock
    {80,117, 16, 16,   0, -4, 13},   // 68: pushable rock

    {48, 93, 16, 12,   0,  0, 13},   // 69: Visible Item pickup
    
    {188, 47, 17, 29,   0,  -17, 13},// 70: NPC Up-facing
    {188, 76, 17, 29,   0,  -17, 13},// 71: NPC Right-facing
    {205, 47, 17, 29,   0,  -17, 13},// 72: NPC Down-facing
    {205, 76, 17, 29,   0,  -17, 13},// 73: NPC Left-facing

};


static int tileMapToImageData(struct ascoCell cell){
    if(cell.tile == TILE_VOID) return 0;
    if(cell.tile == TILE_UNKNOWN) return 39;
    if(cell.tile == TILE_UNRESOLVED) return 44;
    if(cell.tile == TILE_BLOCKAGE) return 45;

    if(cell.tile == TILE_BLANK) return 13;

    if(cell.tile == TILE_CLIFF){
        if(cell.variant == 0) return 14;    // flat cliff doubles as inaccessible floor
        return 1 + ((cell.variant - 1) * 4) + cell.rotation;
    }

    if(cell.tile == TILE_LEDGE){
        if(cell.variant == 1) return 49 + cell.rotation;
        return 53 + (cell.rotation * 2 + (cell.variant - 4));   // TODO other forms of ledge
    }

    if(cell.tile == TILE_WATER){
        if(cell.variant == 0) return 26;
        return 27 + ((cell.variant - 1) * 4) + cell.rotation;
    }

    if(cell.tile == TILE_STAIR){
        return 18 + cell.rotation;
    }

    if(cell.tile == TILE_ROCK_SMALL) return 15;
    if(cell.tile == TILE_ROCK_TALL) return 16;
    if(cell.tile == TILE_ROCK_LARGE) return (cell.rotation == 3) ? 17 : 13;

    if(cell.tile == TILE_DOOR) return 22 + cell.rotation;   // TODO interior vs exterior
    if(cell.tile == TILE_LADDER) return 46;                 // TODO ascending vs descending

    if(cell.tile == TILE_ROCK_SMASH) return 67;
    if(cell.tile == TILE_ROCK_STRENGTH) return 68;

    if(cell.tile == TILE_ITEM_PLACEHOLDER) return 69;
    if(cell.tile == TILE_NPC_PLACEHOLDER) return 70 + cell.rotation;

    return 39;
}




// Draw a tile
static void drawTile(cairo_t *cr, cairo_surface_t *tileSet, int x, int y, unsigned int tile, int palette){
    cairo_set_source_surface(cr, tileSet, x - tileSetData[tile][0], y - (tileSetData[tile][1] + 160 * palette));
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
            drawTile(cr, tileSet, startX + x*16, startY + y*12, tile, 0);
        }
    }
}



#define BUFFER_X 16
#define BUFFER_Y 12


void cairoRenderMap(struct ascoTileMap *map, int palette){

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
            int tile = tileMapToImageData(mapCell(map, x, y));
            // Eliminate convex-concave smashing
            if(mapCell(map, x, y).tile == TILE_CLIFF && y > 0 && mapCell(map, x, y).variant == 3 && mapCell(map, x, y-1).tile == TILE_CLIFF
                && mapCell(map, x, y-1).variant == 2 && mapCell(map, x, y).z == mapCell(map, x, y-1).z){
                tile = 1;
            }


            if(tileSetData[tile][6] > 0){
                // Tile needs another tile printed beneath
                // First draw underlying tile (TODO floor-data instead...)
                int subTile = tileSetData[tile][6];
                yOffset[x] += tileSetData[subTile][5];
                drawTile(cr, tileSet, x * 16 + BUFFER_X + tileSetData[subTile][4], yOffset[x], subTile, palette);
                // Then draw the tile
                /* TODO handling of ladder here needs conversion
                drawTile((map->cells[(y*map->width)+x].tile == TILE_LADDER_ASCENDING) ? tcr : scr, 
                    tileSet, x * 16 + BUFFER_X + tileSetData[tile][4], yOffset[x] + tileSetData[tile][5], tile);
                */
                cairo_t *context = scr;
                drawTile(context, tileSet, x * 16 + BUFFER_X + tileSetData[tile][4], yOffset[x] + tileSetData[tile][5], tile, palette);

                
                // TODO temporary stair-widening process (since stair termination not yet handled by tileSET
                if(mapCell(map, x, y).tile == TILE_STAIR && (
                    (mapCell(map, x+1, y).tile == TILE_STAIR && 
                        mapCell(map, x, y).rotation == mapCell(map, x+1, y).rotation && mapCell(map, x, y).z == mapCell(map, x+1, y).z)|| 
                    (mapCell(map, x, y+1).tile == TILE_STAIR && 
                        mapCell(map, x, y).rotation == mapCell(map, x, y+1).rotation && mapCell(map, x, y).z == mapCell(map, x, y+1).z)
                )){
                    // No bounds check, because stairs will never appear on the map boundary (if they do, I'll eat the segfault as this isn't production-destined code)
                    int widenerTile = 40 + mapCell(map, x, y).rotation;
                    drawTile(tcr, tileSet, x * 16 + BUFFER_X + tileSetData[widenerTile][4], yOffset[x] + tileSetData[widenerTile][5], widenerTile, palette);
                }

                // Then move on
                yOffset[x] += tileSetData[subTile][3];
            } else {
                // Regular-handling rock tile
                yOffset[x] += tileSetData[tile][5];
                drawTile(cr, tileSet, x * 16 + BUFFER_X + tileSetData[tile][4], yOffset[x], tile, palette);
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
