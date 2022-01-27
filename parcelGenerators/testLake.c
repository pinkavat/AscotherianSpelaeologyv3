
#include "testLake.h"
// testLake.c
// See header for details


void testLakeIdeator(struct parcel *parcel){

    // Ignore shape and parameters completely
    
    parcel->realizer = &testLakeRealizer;
    parcel->data = NULL;
    parcel->transform = newGridTransform();

    parcel->flexX = 1.0;    // Test lake absolutely wants to resize as much as possible
    parcel->flexY = 1.0;

    parcel->minWidth = parcel->parameters.gateWidth + 3;   // Initial dimensions are minimal (width is walkway-inclusive) TODO assumptionbased
    parcel->minHeight = parcel->parameters.gateWidth + 2;  // Height is NOT shield-inclusive, shields are optional

    parcel->children = NULL;
    parcel->childCount = 0;

    // Transform field set by invoker

    // Residuals not set until realization
    
}


void testLakeRealizer(void *context, struct parcel *parcel){
    // Cast context
    struct ascoTileMap *map = ((struct ascoGenContext *)context)->map;



    parcel->walkwayWidth = 1;
    parcel->shieldHeight = 0;

    // Draw lake core
    struct ascoCell lakeCell = {TILE_WATER, 0, 0, 0};
    fillRectAuto(map, &lakeCell, &(parcel->transform), 
        parcel->walkwayWidth, 0, parcel->transform.width - parcel->walkwayWidth, parcel->transform.height, 1); 


    parcel->gates[0].position = parcel->transform.height - parcel->parameters.gateWidth - 1;
    parcel->gates[0].size = selfHasGate(parcel->shape, 0) ? parcel->parameters.gateWidth : 0;
    parcel->gates[1].position = parcel->transform.width - parcel->parameters.gateWidth - 1;
    parcel->gates[1].size = selfHasGate(parcel->shape, 1) ? parcel->parameters.gateWidth : 0;
    parcel->gates[2].position = parcel->transform.height - parcel->parameters.gateWidth - 1;
    parcel->gates[2].size = selfHasGate(parcel->shape, 2) ? parcel->parameters.gateWidth : 0;
    parcel->gates[3].position = parcel->transform.width - parcel->parameters.gateWidth - 1;
    parcel->gates[3].size = selfHasGate(parcel->shape, 3) ? parcel->parameters.gateWidth : 0;
}
