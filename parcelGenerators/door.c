
#include "door.h"
// door.c
// See header for details


void doorIdeator(struct parcel *parcel){

    // Ignore shape and parameters completely
    
    parcel->realizer = &doorRealizer;
    parcel->data = NULL;
    parcel->transform = newGridTransform();

    parcel->flexX = 0.0;    // If we hit this base case we don't want to make it any more obvious
    parcel->flexY = 0.0;    //  hence it doesn't want to grow any bigger

    parcel->minWidth = parcel->parameters.pathWidth + 1;
    parcel->minHeight = parcel->parameters.gateWidth + 2;   // Guaranteed to be at least 3 then

    parcel->children = NULL;
    parcel->childCount = 0;

    // Transform field set by invoker

    // Residuals not set until realization
    
}


void doorRealizer(void *context, struct parcel *parcel){
    // Cast context
    struct ascoTileMap *map = ((struct ascoGenContext *)context)->map;
    // TODO handle warp hook

    // This parcel can only meaningfully be E-shape. (TODO later add L-shape where the door appears off a side?)

    parcel->walkwayWidth = parcel->transform.width - 1;
    parcel->shieldHeight = 0;

    // Fill "core landing"
    struct ascoCell floorCell = {TILE_UNRESOLVED, 0, 0, 0};
    fillRect(map, &floorCell, &(parcel->transform), parcel->walkwayWidth, 0, 1, parcel->transform.height);

    // Compute door position
    int doorPosition = parcel->transform.height / 2;

    // Overwrite "doorstep"
    struct ascoCell doorstepCell = {TILE_BLANK, 0, 0, 0};
    placeCell(map, &doorstepCell, &(parcel->transform), parcel->walkwayWidth, doorPosition - 1); 
    placeCell(map, &doorstepCell, &(parcel->transform), parcel->walkwayWidth, doorPosition); 
    placeCell(map, &doorstepCell, &(parcel->transform), parcel->walkwayWidth, doorPosition + 1); 

    // Place door
    // TODO a cheeky little trick; place the door out-of-bounds, assuming safe wrapper of the map in cliffs
    struct ascoCell doorCell = {TILE_DOOR, 0, 3, 0};    // Facing towards entry gate
    placeCell(map, &doorCell, &(parcel->transform), parcel->transform.width, doorPosition); 


    // Set gates
    parcel->gates[0].position = doorPosition;
    parcel->gates[0].size = parcel->parameters.gateWidth;
    parcel->gates[1].position = 1;
    parcel->gates[1].size = 0;
    parcel->gates[2].position = 1;
    parcel->gates[2].size = 0;
    parcel->gates[3].position = 1;
    parcel->gates[3].size = 0;
}
