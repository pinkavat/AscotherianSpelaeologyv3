#include <limits.h> // for INT_MAX in isolation computation

#include "postProcessing.h"
// postProcessing.c
// See header for details


///TODO: under consideration for replacement with general isolation fill
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





// TODO document
void computeIsolation(struct ascoTileMap *map, int *isolationMap){
    // Set up a coord queue
    struct coordQueue *queue = newCoordQueue(128);    // TODO initial size...?

    // Pass 1: set initial values and enqueue all critpath tiles
    for(int y = 0; y < map->height; y++){
        for(int x = 0; x < map->width; x++){
            if(mapCell(map, x, y).tile == TILE_BLANK){
                isolationMap[y * map->width + x] = 0;
                enCoordQueue(queue, x, y);
            } else if(mapCell(map, x, y).tile == TILE_UNRESOLVED) {
                isolationMap[y * map->width + x] = INT_MAX;
            } else {
                isolationMap[y * map->width + x] = -1;
            }
        }
    }

    // Pass 2: floodfill from critical path tiles
    int x, y;
    while(deCoordQueue(queue, &x, &y)){
        int selfIsolation = isolationMap[(y * map->width) + x];
        if(y > 0 && isolationMap[(y - 1) * map->width + x] > selfIsolation + 1){
            isolationMap[(y - 1) * map->width + x] = selfIsolation + 1;
            enCoordQueue(queue, x, y - 1);
        }
        if(x < map->width - 1 && isolationMap[y * map->width + x + 1] > selfIsolation + 1){
            isolationMap[y * map->width + x + 1] = selfIsolation + 1;
            enCoordQueue(queue, x + 1, y);
        }
        if(y < map->height - 1 && isolationMap[(y + 1) * map->width + x] > selfIsolation + 1){
            isolationMap[(y + 1) * map->width + x] = selfIsolation + 1;
            enCoordQueue(queue, x, y + 1);
        }
        if(x > 0 && isolationMap[y * map->width + x - 1] > selfIsolation + 1){
            isolationMap[y * map->width + x - 1] = selfIsolation + 1;
            enCoordQueue(queue, x - 1, y);
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



// Helper macro for inaccessibleCliffStep
#define TILE_MAKES_INACCESSIBLE(t) ((t) == TILE_CLIFF || (t) == TILE_BLOCKAGE)


// TODO own file
// TODO better doc
// TODO fold into other map passes (cheaper)
// Passes over the map, turning inaccessible blockages into inaccessible floors
// NOTE: clobbers the blockage's variant field (safe?) as opposed to an auxiliary datastruct
void inaccessibleCliffStep(struct ascoTileMap *map){
    
    // First pass: flag all "connected" blockages

    // Set up a coord queue
    struct coordQueue *queue = newCoordQueue(32);    // TODO initial size...?

    for(int y = 0; y < map->height - 1; y++){
        for(int x = 0; x < map->width - 1; x++){
            struct ascoCell *cell = &(mapCell(map, x, y));
            if(cell->tile == TILE_BLOCKAGE && cell->variant == 0){
                // Check cell's neighbors to see if any aren't blockage or cliff
                if(
                    (y > 0 && !TILE_MAKES_INACCESSIBLE(mapCell(map, x, y-1).tile)) ||
                    (x < map->width - 1 && !TILE_MAKES_INACCESSIBLE(mapCell(map, x+1, y).tile)) ||
                    (y < map->height - 1 && !TILE_MAKES_INACCESSIBLE(mapCell(map, x, y+1).tile)) ||
                    (x > 0 && !TILE_MAKES_INACCESSIBLE(mapCell(map, x-1, y).tile))
                ){
                    // If so, floodfill out from this tile, marking all adjacent blockages as connected (by setting their variant fields to 1)
                    enCoordQueue(queue, x, y); 

                    // TODO: this traversal logic is shared with other floodfills; is there some easy helper refactoring?
                    int m, n;
                    while(deCoordQueue(queue, &m, &n)){
                        // 1) Set self to variant 1
                        mapCell(map, m, n).variant = 1;
                        
                        // 2) Enqueue all variant-0 blockage neighbors
                        if(n > 0 && mapCell(map, m, n-1).tile == TILE_BLOCKAGE && mapCell(map, m, n-1).variant == 0) enCoordQueue(queue, m, n-1);
                        if(m < map->width-1 && mapCell(map, m+1, n).tile == TILE_BLOCKAGE && mapCell(map, m+1, n).variant == 0) enCoordQueue(queue, m+1, n);
                        if(n < map->height-1 && mapCell(map, m, n+1).tile == TILE_BLOCKAGE && mapCell(map, m, n+1).variant == 0) enCoordQueue(queue, m, n+1);
                        if(m > 0 && mapCell(map, m-1, n).tile == TILE_BLOCKAGE && mapCell(map, m-1, n).variant == 0) enCoordQueue(queue, m-1, n);
                    }

                }

            }
        }
    }

    // Second pass: replace all unconnected blockages with blank cliff
    // TODO is it cheaper to enqueue rather than flag and secondpass? Is it safer?
    for(int y = 0; y < map->height - 1; y++){
        for(int x = 0; x < map->width - 1; x++){
            struct ascoCell *cell = &(mapCell(map, x, y));
            if(cell->tile == TILE_BLOCKAGE){
                if(cell->variant == 1){
                    cell->variant = 0;   // Reset variant
                } else {
                    cell->tile = TILE_CLIFF;
                }                
            }
        }
    }
}




/* TODO doesn't really work
// TODO document 
// turns unresolveds to blockage at random based on their neighbors
void rockSprinklingStep(struct ascoTileMap *map){
    // Set up a coord queue
    struct coordQueue *queue = newCoordQueue(16);    // TODO initial size...?

    for(int y = 0; y < map->height; y++){
        for(int x = 0; x < map->width; x++){
            if(mapCell(map, x, y).tile == TILE_UNRESOLVED){
                // If we flatten to blockage, we enqueue our neighbors; therefore start by enqueueing
                enCoordQueue(queue, x, y);

                int m, n;
                while(deCoordQueue(queue, &m, &n)){

                    // Count neighbor blockages: blockages are worth 2, unresolveds worth 1, blanks worth zero.
                    int neighborValue = 0;
                    if(n > 0) neighborValue += mapCell(map, m, n-1).tile == TILE_BLANK ? 0 : mapCell(map, m, n-1).tile == TILE_UNRESOLVED ? 1 : 2;
                    if(m < map->width - 1) neighborValue += mapCell(map, m+1, n).tile == TILE_BLANK ? 0 : mapCell(map, m+1, n).tile == TILE_UNRESOLVED ? 1 : 2;
                    if(n < map->height - 1) neighborValue += mapCell(map, m, n+1).tile == TILE_BLANK ? 0 : mapCell(map, m, n+1).tile == TILE_UNRESOLVED ? 1 : 2;
                    if(m > 0) neighborValue += mapCell(map, m-1, n).tile == TILE_BLANK ? 0 : mapCell(map, m-1, n).tile == TILE_UNRESOLVED ? 1 : 2;

                    // neighborValue should now lie between zero and eight; set the decision threshold accordingly
                    // TODO for now it's linear; probably not the best
                    int threshold = 5 * neighborValue;
                    
                    if(rand() % 80 < threshold){
                        mapCell(map, m, n).tile = TILE_BLOCKAGE;

                        // ...and enqueue neighbor UNRESOLVEDs
                        if(n > 0 && mapCell(map, m, n-1).tile == TILE_UNRESOLVED) enCoordQueue(queue, m, n-1);
                        if(m < map->width-1 && mapCell(map, m+1, n).tile == TILE_UNRESOLVED) enCoordQueue(queue, m+1, n);
                        if(n < map->height-1 && mapCell(map, m, n+1).tile == TILE_UNRESOLVED) enCoordQueue(queue, m, n+1);
                        if(m > 0 && mapCell(map, m-1, n).tile == TILE_UNRESOLVED) enCoordQueue(queue, m-1, n);
                        
                    }

                }
            }
        }
    }

    freeCoordQueue(queue);
}
*/


// Helper macro for isValidRockSite
#define TILE_IS_FLOOR(t) ((t) == TILE_BLANK || (t) == TILE_UNRESOLVED)

// Code ported from AscoSpel v1 (I've forgotten how it works, which is a pity, because it's really cool)
// Returns truth if
//  a) a blockage tile can be legitimately placed at (x, y)
//  b) placing a rock tile at (x, y) would leave the map topology unaltered (i.e. wouldn't block a path)
//  returns falsehood otherwise
static unsigned int isValidRockSite(struct ascoTileMap *map, int x, int y){

    // Initial assumption: blockage can't be placed on the map border (accelerates neighbor checks by removing bound checks)
    // Also check if the site is UNRESOLVED
    // TODO make function precondition for traversal (save checks all)
    if(x <= 0 || x >= map->width || y <= 0 || y >= map->height || mapCell(map, x, y).tile != TILE_UNRESOLVED) return 0;


    // Gather together the Von Neumann neigborhood of the cell
    unsigned int neighborSet = 0;

    if(TILE_IS_FLOOR(mapCell(map, x, y-1).tile)) neighborSet |= 1;
    if(TILE_IS_FLOOR(mapCell(map, x+1, y).tile)) neighborSet |= 2;
    if(TILE_IS_FLOOR(mapCell(map, x, y+1).tile)) neighborSet |= 4;
    if(TILE_IS_FLOOR(mapCell(map, x-1, y).tile)) neighborSet |= 8;

    // Edge case checking (path going straight through)
    if(neighborSet == 5 || neighborSet == 10) return 0;

    // AND the neighbor set with its right-rotation to determine if the putative rock will block access to anything
    // (Copied from prototype -- I've lost the notes on how this works)
    neighborSet = (neighborSet & (neighborSet >> 1 | ((neighborSet & 1) ? 8 : 0)));

    unsigned int cornerSet = 0;
    cornerSet |= (TILE_IS_FLOOR(mapCell(map, x+1, y-1).tile)) ? 1 : 0; 
    cornerSet |= (TILE_IS_FLOOR(mapCell(map, x+1, y+1).tile)) ? 2 : 0; 
    cornerSet |= (TILE_IS_FLOOR(mapCell(map, x-1, y+1).tile)) ? 4 : 0; 
    cornerSet |= (TILE_IS_FLOOR(mapCell(map, x-1, y-1).tile)) ? 8 : 0;

    if((neighborSet & cornerSet) == neighborSet) return 1;

    return 0;
}




// TODO document
void fenceBreakingStep(struct ascoTileMap *map){
    // Pass over twice to ensure fences are fully broken
    for(int times = 0; times < 2; times++){

        for(int y = 1; y < map->height - 1; y++){
            for(int x = 1; x < map->width - 1; x++){
                if(mapCell(map, x, y).tile == TILE_BLOCKAGE){
                    // If the cell is blockage, check to see if we can remove it
                    
                    // 1) Gather together Von Neumann neighborhood, clockwise from top.
                    unsigned int neighborSet = 0;

                    if(TILE_IS_FLOOR(mapCell(map, x, y-1).tile)) neighborSet |= 1;
                    if(TILE_IS_FLOOR(mapCell(map, x+1, y).tile)) neighborSet |= 2;
                    if(TILE_IS_FLOOR(mapCell(map, x, y+1).tile)) neighborSet |= 4;
                    if(TILE_IS_FLOOR(mapCell(map, x-1, y).tile)) neighborSet |= 8;

                    // 2) If VN neighborhood has only one floor, it's safe to flatten this blockage
                    if(neighborSet == 1 || neighborSet == 2 || neighborSet == 4 || neighborSet == 8){   // TODO cheaper check?
                        mapCell(map, x, y).tile = TILE_UNRESOLVED;
                        continue;
                    }

                    /* 
                    // 3) If VN neighborhood has three floors, we have to check corners.
                    if(neighborSet == 7 || neighborSet == 11 || neighborSet == 13 || neighborSet == 14){    // TODO cheaper check?

                        // If two floor sides are connected by a nonfloor corner, we can't flatten.
                        // bitwise-AND the neighbor set with its right-rotation; the bits that are lit are the corners (clockwise from TR) that join two floor edges
                        neighborSet = (neighborSet & (neighborSet >> 1 | ((neighborSet & 1) ? 8 : 0)));

                        // Amalgamate the corners
                        unsigned int cornerSet = 0;
                        cornerSet |= (!TILE_IS_FLOOR(mapCell(map, x+1, y-1).tile)) ? 1 : 0; 
                        cornerSet |= (!TILE_IS_FLOOR(mapCell(map, x+1, y+1).tile)) ? 2 : 0; 
                        cornerSet |= (!TILE_IS_FLOOR(mapCell(map, x-1, y+1).tile)) ? 4 : 0; 
                        cornerSet |= (!TILE_IS_FLOOR(mapCell(map, x-1, y-1).tile)) ? 8 : 0;

                        // bitwise-AND corner set and neighbor set; the bits lit are corners joining two floors that are themselves not floors.
                        // If any such exist, we cannot flatten.
                        if(!(neighborSet & cornerSet)){
                            mapCell(map, x, y).tile = TILE_UNRESOLVED;
                        }
                    } 
                    */
                    
                }
            }
        }


    }
}





void alternativeFenceBreakingStep(struct ascoTileMap *map){

    // Iterate within a border to save on bounds-checking
    for(int y = 1; y < map->height - 1; y++){
        for(int x = 1; x < map->width - 1; x++){
            if(mapCell(map, x, y).tile == TILE_BLOCKAGE){
                
                // TODO improve this logic
                if(
                    (mapCell(map, x, y-1).tile == TILE_CLIFF && mapCell(map, x+1, y).tile == TILE_BLOCKAGE &&
                     TILE_IS_FLOOR(mapCell(map, x, y+1).tile) && mapCell(map, x-1, y).tile == TILE_BLOCKAGE)
                    ||
                    (mapCell(map, x, y-1).tile == TILE_BLOCKAGE && mapCell(map, x+1, y).tile == TILE_CLIFF &&
                     mapCell(map, x, y+1).tile == TILE_BLOCKAGE && TILE_IS_FLOOR(mapCell(map, x-1, y).tile))
                    ||
                    (TILE_IS_FLOOR(mapCell(map, x, y-1).tile) && mapCell(map, x+1, y).tile == TILE_BLOCKAGE &&
                     mapCell(map, x, y+1).tile == TILE_CLIFF && mapCell(map, x-1, y).tile == TILE_BLOCKAGE)
                    ||
                    (mapCell(map, x, y-1).tile == TILE_BLOCKAGE && TILE_IS_FLOOR(mapCell(map, x+1, y).tile) &&
                     mapCell(map, x, y+1).tile == TILE_BLOCKAGE && mapCell(map, x-1, y).tile == TILE_CLIFF)
                ){
                    mapCell(map, x, y).variant = 1;
                }

            }
        }
    }

    for(int i = 0; i < map->width * map->height; i++){
        if(map->cells[i].tile == TILE_BLOCKAGE && map->cells[i].variant == 1){
            map->cells[i].variant = 0;
            map->cells[i].tile = TILE_UNRESOLVED;
        }
    }
}






void tempPostProcess(struct ascoTileMap *map){
    // 1) Thinfilling step
    //thinFillingStep(map);

    // Compute isolation
    int isolation[map->height][map->width];
    computeIsolation(map, (int *)isolation);

    // Floors too isolated become blockages
    for(int y = 0; y < map->height; y++){
        for(int x = 0; x < map->width; x++){
            if(isolation[y][x] > 4) mapCell(map, x, y).tile = TILE_BLOCKAGE;
        }
    }

    // 2) Cliff oozing
    for(int i = 0; i < 5; i++){
        cliffOozeStep(map);  // TODO run as many times as the map is deep (or some fraction thereof?)
    }




    // 4) Rock sprinkling
    //rockSprinklingStep(map);
    /*
    for(int y = 0; y < map->height; y++){
        for(int x = 0; x < map->width; x++){
            if(isValidRockSite(map, x, y) && !(rand() % 4)) mapCell(map, x, y).tile = TILE_ROCK_SMALL;
        }
    }
    */
    //printAscoTileMap(map);
    //fenceBreakingStep(map);


    alternativeFenceBreakingStep(map);

    // TODO test this
    thinFillingStep(map);
    cliffOozeStep(map);
    cliffOozeStep(map);
    alternativeFenceBreakingStep(map);

    // 3) Flatten blockages atop cliffs to inaccessible cliff
    inaccessibleCliffStep(map);



    // Temp large rock accretion step (only operating on oozed cliffs for now)
    for(int y = 0; y < map->height - 1; y++){
        for(int x = 0; x < map->width - 1; x++){
            if( 
                (mapCell(map, x, y).tile == TILE_CLIFF && mapCell(map, x, y).variant == 2 && mapCell(map, x, y).rotation == 3) && 
                (mapCell(map, x+1, y).tile == TILE_CLIFF && mapCell(map, x+1, y).variant == 2 && mapCell(map, x+1, y).rotation == 0) && 
                (mapCell(map, x, y+1).tile == TILE_CLIFF && mapCell(map, x, y+1).variant == 2 && mapCell(map, x, y+1).rotation == 2) && 
                (mapCell(map, x+1, y+1).tile == TILE_CLIFF && mapCell(map, x+1, y+1).variant == 2 && mapCell(map, x+1, y+1).rotation == 1)
            ){
                mapCell(map, x, y).tile = TILE_ROCK_LARGE;
                mapCell(map, x+1, y).tile = TILE_ROCK_LARGE;
                mapCell(map, x, y+1).tile = TILE_ROCK_LARGE;
                mapCell(map, x+1, y+1).tile = TILE_ROCK_LARGE;
            }
        }
    }

    // One final solve pass (join with accretor)
    /* 
    for(int i = 0; i < map->width * map->height; i++){
        if(map->cells[i].tile == TILE_UNRESOLVED) map->cells[i].tile = TILE_BLANK;
        if(map->cells[i].tile == TILE_BLOCKAGE) map->cells[i].tile = (rand() % 2) ? TILE_ROCK_SMALL : TILE_ROCK_TALL;
    }
    */
}
