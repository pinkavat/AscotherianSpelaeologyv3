
#include "postProcessing.h"
// postProcessing.c
// See header for details



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

#include <stdio.h>  // TODO debug

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
                if(bMS == 5) bMS = 7;
                if(dMS == 10) dMS = 14;

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

                /*

                if(aMS == 15){
                    a->tile = TILE_BLOCKAGE;
                    a->rotation = 0;
                    a->variant = 0;
                    a->z++;
                } else {
                    a->tile = TILE_CLIFF;
                    a->variant = variantFromMS[aMS] % 4;    // TODO remove safety system
                    a->rotation = rotationFromMS[aMS] % 4;
                }

                if(bMS == 15){
                    b->tile = TILE_BLOCKAGE;
                    b->rotation = 0;
                    b->variant = 0;
                    b->z++;
                } else {
                    b->tile = TILE_CLIFF;
                    b->variant = variantFromMS[bMS] % 4;    // TODO remove safety system
                    b->rotation = rotationFromMS[bMS] % 4;
                }

                if(cMS == 15){
                    c->tile = TILE_BLOCKAGE;
                    c->rotation = 0;
                    c->variant = 0;
                    c->z++;
                } else {
                    c->tile = TILE_CLIFF;
                    c->variant = variantFromMS[cMS] % 4;    // TODO remove safety system
                    c->rotation = rotationFromMS[cMS] % 4;
                }

                if(dMS == 15){
                    d->tile = TILE_BLOCKAGE;
                    d->rotation = 0;
                    d->variant = 0;
                    d->z++;
                } else {
                    d->tile = TILE_CLIFF;
                    d->variant = variantFromMS[dMS] % 4;    // TODO remove safety system
                    d->rotation = rotationFromMS[dMS] % 4;
                }
                */
            }


        }
    }
}





void tempPostProcess(struct ascoTileMap *map){
    for(int i = 0; i < 5; i++) cliffOozeStep(map);

}
