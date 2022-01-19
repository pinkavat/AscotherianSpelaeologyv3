
#include "postProcessing.h"
// postProcessing.c
// See header for details



// THINFILLING STEP
// TODO document better
// TODO migrate to own file?
// TODO FOLD TRAVERSAL INTO OTHER LOGICS AS WELL
// Traverses entire map: unresolveds surrounded on three sides become blockage and floodfill outward
void thinFillingStep(struct ascoTileMap *map){
    // Set up a coord queue
    struct coordQueue *queue = newCoordQueue(32);    // TODO initial size...?

    for(int y = 0; y < map->height - 1; y++){
        for(int x = 0; x < map->width - 1; x++){
            if(mapCell(map, x, y).tile == TILE_UNRESOLVED){

                // Begin tentative floodfill
                enCoordQueue(queue, x, y);

                int m, n;
                while(deCoordQueue(queue, &m, &n)){
                    // Check if cell m, n is bordered on more than three sides by Cliffs or Blockages
                    uint8_t topTile   = (n > 0)              ? mapCell(map, m, n-1).tile : TILE_VOID;
                    uint8_t rightTile = (m < map->width - 1) ? mapCell(map, m+1, n).tile : TILE_VOID;
                    uint8_t bottomTile= (n < map->height -1) ? mapCell(map, m, n+1).tile : TILE_VOID;
                    uint8_t leftTile  = (m > 0)              ? mapCell(map, m-1, n).tile : TILE_VOID;
                    
                    int sides = 0;
                    if(topTile == TILE_CLIFF || topTile == TILE_BLOCKAGE) sides++;
                    if(rightTile == TILE_CLIFF || rightTile == TILE_BLOCKAGE) sides++;
                    if(bottomTile == TILE_CLIFF || bottomTile == TILE_BLOCKAGE) sides++;
                    if(leftTile == TILE_CLIFF || leftTile == TILE_BLOCKAGE) sides++;

                    if(sides > 2){
                        // So: collapse to blockage and enqueue unresolved neighbors
                        mapCell(map, m, n).tile = TILE_BLOCKAGE;
                        if(topTile == TILE_UNRESOLVED) enCoordQueue(queue, m, n-1);
                        if(rightTile == TILE_UNRESOLVED) enCoordQueue(queue, m+1, n);
                        if(bottomTile == TILE_UNRESOLVED) enCoordQueue(queue, m, n+1);
                        if(leftTile == TILE_UNRESOLVED) enCoordQueue(queue, m-1, n);
                    }
                }
            }
        }
    }

    freeCoordQueue(queue);
}




// CLIFF OOZE STEPS
// TODO migrate to own file

// Helper: Marching squares translation system (TODO move to ascotherianTileMap?)
// TODO note: MS notation is [TL, TR, BR, BL] corners as bits of a number
// Translates from variant-rotation notation (useful for tile transforms) to Marching-Squares notation (useful for tile rewrite systems)
// Pattern: [Variant] -> [Rotation]: MS notation
static const uint8_t MSFromVariantRotation[4][4] = {
    {0, 0, 0, 0},   // Variant 0: flat
    {3, 9, 12, 6},  // Variant 1: straight
    {1, 8, 4, 2},   // Variant 2: convex
    {11, 13, 14, 7} // Variant 3: concave
};

// The opposite translator: MS notation to variant and rotation
// NOTE: uses variant/rotation number "4" to denote error (5, 10, 15 are not covered in varRot scheme!)
static const uint8_t variantFromMS[16] = {
    0, 2, 2, 1, 2, 4, 1, 3, 2, 1, 4, 3, 1, 3, 3, 4
};

static const uint8_t rotationFromMS[16] = {
    0, 0, 3, 0, 2, 4, 3, 3, 1, 1, 4, 0, 2, 1, 2, 4
};


// Helper macro for tile id wrangling in cliffOozeStep below
#define TILE_OOZEABLE(t) ((t) == TILE_CLIFF || (t) == TILE_BLOCKAGE)


// Cliff ooze step: TODO better doc
// Traverses entire map: cliffs that are same height and facing each other rise to same level.
void cliffOozeStep(struct ascoTileMap *map){

    for(int y = 0; y < map->height - 1; y++){
        for(int x = 0; x < map->width - 1; x++){
            // Algorithm checks 2x2 grids of tiles; therefore, traversal never touches last row or column
 
            struct ascoCell *a = &(mapCell(map, x, y));
            struct ascoCell *b = &(mapCell(map, x+1, y));
            struct ascoCell *c = &(mapCell(map, x, y+1));
            struct ascoCell *d = &(mapCell(map, x+1, y+1));

            if(TILE_OOZEABLE(a->tile) && TILE_OOZEABLE(b->tile) && TILE_OOZEABLE(c->tile) && TILE_OOZEABLE(d->tile) &&
                a->z == b->z && a->z == c->z && a->z == d->z){
                // If all four cells in the 2x2 region are same-height nonflat cliffs

                // Fetch the Marching squares representation of all four cells
                uint8_t aMS = MSFromVariantRotation[a->variant][a->rotation];
                uint8_t bMS = MSFromVariantRotation[b->variant][b->rotation];
                uint8_t cMS = MSFromVariantRotation[c->variant][c->rotation];
                uint8_t dMS = MSFromVariantRotation[d->variant][d->rotation];

                //printf("(%d, %d): %d %d %d %d\n", x, y, aMS, bMS, cMS, dMS); // TODO debug
                
                // Set the Marching squares height corner at the center of the region to 1
                aMS = aMS | 2;
                bMS = bMS | 1;
                cMS = cMS | 4;
                dMS = dMS | 8;

                // If this results in an illegal "saddle" configuration, lift the next corner over as well
                // (TODO: we haven't really thought this out, this is just an instinctual fix to a found bug)
                int clobbering = 0;
                if(bMS == 5){
                    bMS = 7;
                    clobbering = 1;
                }
                if(dMS == 10){
                    dMS = 14;
                    clobbering = 1;
                }

                // ...and, like all "instinctual fixes", it's more complex than it first appears.
                if(aMS == 10 || cMS == 5) continue;
                if(clobbering && x < map->width - 2){
                    struct ascoCell *e = &(mapCell(map, x+2, y));
                    struct ascoCell *f = &(mapCell(map, x+2, y+1));
                    if( !( TILE_OOZEABLE(e->tile) && TILE_OOZEABLE(f->tile) && b->z == e->z && b->z == f->z) ) continue;
                    if((MSFromVariantRotation[e->variant][e->rotation] | 1) == 5) continue;
                    if((MSFromVariantRotation[f->variant][f->rotation] | 8) == 10) continue;  // Check to see if raising would cause yet another saddle
                }

                // Adjust the cells accordingly
                struct ascoCell *cells[4] = {a, b, c, d};
                const uint8_t MSVals[4] = {aMS, bMS, cMS, dMS};
                
                for(int i = 0; i < 4; i++){
                    if(MSVals[i] == 15){
                        cells[i]->tile = TILE_BLOCKAGE;
                        cells[i]->rotation = 0;
                        cells[i]->variant = 0;
                        cells[i]->z++;
                        if(cells[i]->z > 0) cells[i]->tile = TILE_VOID;
                    } else {
                        cells[i]->tile = TILE_CLIFF;
                        cells[i]->variant = variantFromMS[MSVals[i]];
                        cells[i]->rotation = rotationFromMS[MSVals[i]];
                    }
                }

            }


        }
    }
}





void tempPostProcess(struct ascoTileMap *map){
    // 1) Thinfilling step
    thinFillingStep(map);

    // 2) Cliff oozing
    for(int i = 0; i < 5; i++){
        cliffOozeStep(map);  // TODO run as many times as the map is deep (or some fraction thereof?)
    }
}
