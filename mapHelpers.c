
#include "mapHelpers.h"
// mapHelpers.c
// See header for details


// For fillRectCallback below
struct fillRectCallbackContext {
    struct ascoTileMap *map;
    struct ascoCell *cell;
};


// Helper for fillRect below
static void fillRectCallback(void *context, int x, int y, int m, int n){
    // Cast context pointer
    struct fillRectCallbackContext *contextPtr = (struct fillRectCallbackContext *)context;

    // Copy cell
    mapCell(contextPtr->map, m, n) = *(contextPtr->cell);
}


void fillRect(struct ascoTileMap *map, struct ascoCell *cell, struct gridTransform *t, int a, int b, int w, int h){
    // 1) Copy input cell
    struct ascoCell stampCell = *cell;

    // 2) Transform input cell to match grid transform
    rotateCell(&stampCell, t->rotation, t->flipH, t->flipV);
    stampCell.z += t->z;
    
    // 3) Prepare a context callback struct for gTRegionIterate
    struct fillRectCallbackContext context = {map, &stampCell};

    gTRegionIterate(t, &context, &fillRectCallback, a, b, w, h);
}





void fillRectAuto(struct ascoTileMap *map, struct ascoCell *cell, struct gridTransform *t, int a, int b, int w, int h, unsigned int fillCore){
    switch(cell->tile->tilingType){
        case ASCO_TILING_NONE:
            // Nontiling tiles don't react to smart autofilling: fill as normal
            fillRect(map, cell, t, a, b, w, h);
        break;

        case ASCO_TILING_MS:
        {   // Oh, C. You prickly beast.

            // Generate subordinate nine-patch-rect tiles
            struct ascoCell tl = *cell;     tl.variant = 3;     tl.rotation = 1;
            struct ascoCell to = *cell;     to.variant = 1;     to.rotation = 2;
            struct ascoCell tr = *cell;     tr.variant = 3;     tr.rotation = 2;
            struct ascoCell l  = *cell;      l.variant = 1;      l.rotation = 1;
            struct ascoCell c  = *cell;      c.variant = 0;      c.rotation = 0;
            struct ascoCell r  = *cell;      r.variant = 1;      r.rotation = 3;
            struct ascoCell bl = *cell;     bl.variant = 3;     bl.rotation = 0;
            struct ascoCell bo  = *cell;    bo.variant = 1;     bo.rotation = 0;
            struct ascoCell br = *cell;     br.variant = 3;     br.rotation = 3;

            // Fill ops for each tile (probably a bit baroque using fillRect above, but it's simple to write and the computer can eat it easily)
            fillRect(map, &tl, t, a, b, 1, 1);
            fillRect(map, &to, t, a + 1, b, w - 2, 1);
            fillRect(map, &tr, t, a + w - 1, b, 1, 1);

            fillRect(map, &l, t, a, b + 1, 1, h - 2);
            if(fillCore) fillRect(map, &c, t, a + 1, b + 1, w - 2, h - 2);
            fillRect(map, &r, t, a + w - 1, b + 1, 1, h - 2);

            fillRect(map, &bl, t, a, b + h - 1, 1, 1);
            fillRect(map, &bo, t, a + 1, b + h - 1, w - 2, 1);
            fillRect(map, &br, t, a + w - 1, b + h - 1, 1, 1);
        break;
        }

        case ASCO_TILING_FENCE:
            // TODO
        break;

        case ASCO_TILING_ANCHOR:
            // TODO
        break; 
    }
}





void realizeWalkwayAndShield(struct ascoTileMap *map, struct gridTransform *walkway, struct gridTransform *shield,
    struct gridTransform *entryGate, struct gridTransform *exitGate){

    // TODO something more interesting, no doubt.

    // Right now relies on zero-dimension fail-safe behaviour of gTRegionIterate. Make explicitly safe with checks.
    
    // 1) Draw the walkway
    struct ascoCell blankFloorCell = {TILE_BLANK, 0, 0, 0};
    fillRect(map, &blankFloorCell, walkway, 0, 0, walkway->width, walkway->height); 

    // 2) Draw the shield
    struct ascoCell blockageCell = {TILE_BLOCKAGE, 0, 0, 0};
    fillRect(map, &blockageCell, shield, 0, 0, shield->width, shield->height); 

}
