
#include "walkwayAndShield.h"
// walkwayAndShield.c
// See header for details



void realizeWalkwayAndShield(struct ascoTileMap *map, struct parcel *parcel, struct gate *innerGate, struct gate *outerGate){
    // TODO something more interesting, no doubt.
    // TODO gate track

    // Right now relies on zero-dimension fail-safe behaviour of gTRegionIterate. Make explicitly safe with checks?
    
    // 1) Draw the walkway
    struct ascoCell walkwayCell = {TILE_UNRESOLVED, 0, 0, 0};
    fillRect(map, &walkwayCell, &(parcel->transform), 0, 0, parcel->walkwayWidth, parcel->transform.height); 

    // 2) Draw the shield 
    struct ascoCell shieldCell = {TILE_BLOCKAGE, 0, 0, 0};
    fillRect(map, &shieldCell, &(parcel->transform), parcel->walkwayWidth, 0, parcel->transform.width - parcel->walkwayWidth, parcel->shieldHeight); 

    // TODO temporary gate markers
    if(parcel->walkwayWidth > 0){
        struct ascoCell gateCell = {TILE_BLANK, 0, 0, 0};
        fillRect(map, &gateCell, &(parcel->transform), 0, outerGate->position, 1, outerGate->size); 
        fillRect(map, &gateCell, &(parcel->transform), parcel->walkwayWidth - 1, innerGate->position, 1, innerGate->size); 
    }
}
