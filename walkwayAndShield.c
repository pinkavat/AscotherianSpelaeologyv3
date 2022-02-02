
#include "walkwayAndShield.h"
// walkwayAndShield.c
// See header for details


void realizeWalkwayAndShield(struct ascoTileMap *map, struct parcel *parcel, struct gate *innerGate, struct gate *outerGate, int topBarrier, int bottomBarrier){
    // TODO something more interesting, no doubt.
    // TODO gate track

    // Right now relies on zero-dimension fail-safe behaviour of gTRegionIterate. Make explicitly safe with checks?
    
    // 1) Blank out the walkway
    struct ascoCell walkwayCell = {TILE_UNRESOLVED, 0, 0, 0};
    fillRect(map, &walkwayCell, &(parcel->transform), 0, 0, parcel->walkwayWidth, parcel->transform.height); 

    // 2) Blank out the shield 
    struct ascoCell shieldCell = {TILE_BLOCKAGE, 0, 0, 0};
    fillRect(map, &shieldCell, &(parcel->transform), parcel->walkwayWidth, 0, parcel->transform.width - parcel->walkwayWidth, parcel->shieldHeight);

    // 3) If the parcel has gate 3, bore a path through the shield
    if(selfHasGate(parcel->shape, 3)){
        struct ascoCell pathCell = {TILE_BLANK, 0, 0, 0};
        fillRect(map, &pathCell, &(parcel->transform), parcel->gates[3].position, 0, parcel->gates[3].size, parcel->shieldHeight);
    }

    // 4) Draw barriers (if needful)
    struct ascoCell barrierCell = {TILE_BLOCKAGE, 0, 0, 0};
    if(topBarrier){
        fillRect(map, &barrierCell, &(parcel->transform), 0, 0, parcel->walkwayWidth, 1);
    }
    if(bottomBarrier){
        fillRect(map, &barrierCell, &(parcel->transform), 0, parcel->transform.height - 1, parcel->walkwayWidth, 1);
    }




    // 5) Draw path through the walkway
    if(parcel->walkwayWidth > 0){
        // Determine where the path falls
        int pathX = 0, pathWidth = 0;
        if(parcel->walkwayWidth < parcel->parameters.pathWidth){
            pathX = 0;
            pathWidth = 1;
        } else {    // TODO superthreshold check to see if path should be shunted left to leave room for something more interesting
            pathX = (parcel->walkwayWidth / 2) - (parcel->parameters.pathWidth / 2);
            pathWidth = parcel->parameters.pathWidth;
        }

        // Draw the path
        struct ascoCell pathCell = {TILE_BLANK, 0, 0, 0};

        // Outer gate incursion
        fillRect(map, &pathCell, &(parcel->transform), 0, outerGate->position, pathX, outerGate->size);

        // Inner gate incursion
        fillRect(map, &pathCell, &(parcel->transform), pathX, innerGate->position, parcel->walkwayWidth - pathX, innerGate->size);

        // Linking pathway
        if(outerGate->position > innerGate->position){
            // Draw from inner to outer
            fillRect(map, &pathCell, &(parcel->transform), pathX, innerGate->position, pathWidth, (outerGate->position - innerGate->position) + outerGate->size);
        } else {
            // Draw from outer to inner
            fillRect(map, &pathCell, &(parcel->transform), pathX, outerGate->position, pathWidth, (innerGate->position - outerGate->position) + innerGate->size);
        } 
    }
}
