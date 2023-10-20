
#include "ladderAndPit.h"
// ladderAndPit.c
// See header for details


static struct ascoCell ladderUpCell = {TILE_LADDER, 1, 0, 0};
static struct ascoCell ladderDownCell = {TILE_LADDER, 0, 0, 0};
static struct ascoCell pitCell = {TILE_PIT, 0, 0, 0};
static struct ascoCell pitLandingCell = {TILE_PIT, 1, 0, 0};

// Core for the below four exposed functions
static void ladderAndPitCoreIdeator(struct parcel *parcel, struct ascoCell *cell){

    // Ignore shape and parameters completely
    
    parcel->realizer = &ladderAndPitRealizer;
    parcel->data = (void *)cell;
    parcel->transform = newGridTransform();

    parcel->flexX = 0.0;    // Ladders don't grow.
    parcel->flexY = 0.0;

    parcel->minWidth = 1 + parcel->parameters.gateWidth + parcel->parameters.pathWidth; // Akin to the base case (width is walkway-inclusive)
    parcel->minHeight = 2 + parcel->parameters.gateWidth;

    parcel->children = NULL;
    parcel->childCount = 0;

    // Transform field set by invoker

    // Residuals not set until realization
    
}



void ladderUpIdeator(struct parcel *parcel){
    ladderAndPitCoreIdeator(parcel, &ladderUpCell);
}


void ladderDownIdeator(struct parcel *parcel){
    ladderAndPitCoreIdeator(parcel, &ladderDownCell);
}


void pitIdeator(struct parcel *parcel){
    ladderAndPitCoreIdeator(parcel, &pitCell);
}


void pitLandingIdeator(struct parcel *parcel){
    ladderAndPitCoreIdeator(parcel, &pitLandingCell);
}





void ladderAndPitRealizer(void *context, struct parcel *parcel){
    // Cast context
    // TODO warp hook handling
    struct ascoTileMap *map = ((struct ascoGenContext *)context)->map;

    int coreWidth = 1 + parcel->parameters.gateWidth;
    int coreHeight = 2 + parcel->parameters.gateWidth;

    parcel->walkwayWidth = parcel->transform.width - coreWidth;
    parcel->shieldHeight = parcel->transform.height - coreHeight;

    // Blank out floor (akin to base case)
    struct ascoCell blankFloorCell = {TILE_BLANK, 0, 0, 0};
    fillRect(map, &blankFloorCell, &(parcel->transform), parcel->walkwayWidth, parcel->shieldHeight, coreWidth, coreHeight); 


    // Place Core phenomenon based on data value (TODO hooks)
    struct ascoCell coreCell = *((struct ascoCell *)parcel->data);
    placeCell(map, &coreCell, &(parcel->transform), parcel->walkwayWidth + 1, parcel->shieldHeight + 1);


    // Set gates
    // (Ladders with more than one gate...?)
    parcel->gates[0].position = parcel->transform.height - (parcel->parameters.gateWidth + 1);
    parcel->gates[0].size = selfHasGate(parcel->shape, 0) ? parcel->parameters.gateWidth : 0;
    parcel->gates[1].position = parcel->transform.width - (parcel->parameters.gateWidth + 1);
    parcel->gates[1].size = selfHasGate(parcel->shape, 1) ? parcel->parameters.gateWidth : 0;
    parcel->gates[2].position = parcel->transform.height - (parcel->parameters.gateWidth + 1);
    parcel->gates[2].size = selfHasGate(parcel->shape, 2) ? parcel->parameters.gateWidth : 0;
    parcel->gates[3].position = parcel->transform.width - (parcel->parameters.gateWidth + 1);
    parcel->gates[3].size = selfHasGate(parcel->shape, 3) ? parcel->parameters.gateWidth : 0;
    // TODO if we're allowing gate-three don't forget trans-shield holepunching

}
