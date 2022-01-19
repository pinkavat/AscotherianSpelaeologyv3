
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

    parcel->minWidth = 5;   // Initial dimensions are minimal (width is walkway-inclusive) TODO assumptionbased
    parcel->minHeight = 4;  // Height is NOT shield-inclusive, shields are optional

    parcel->children = NULL;
    parcel->childCount = 0;

    // Transform field set by invoker

    // Residuals not set until realization
    
}


void testLakeRealizer(void *context, struct parcel *parcel){
    // Cast context
    struct ascoTileMap *map = ((struct ascoGenContext *)context)->map;

    // The base case must be able to handle all possible shapes
    switch(parcel->shape){
        case V_SHAPE:
            // TODO ????? 
        break;
        case E_SHAPE: case L_SHAPE: case I_SHAPE:
        case TL_SHAPE: case TI_SHAPE:
        case XL_SHAPE: case XI_SHAPE:

            /* TODO RELIC CODE REMOVE
            // Set walkway
            parcel->walkway = newGridTransform();
            parcel->walkway.width = 1;
            parcel->walkway.height = parcel->transform.height;
    
            // Set shield
            parcel->shield = newGridTransform();
            //parcel->shield.x = parcel->walkway.width;
            parcel->shield.width = parcel->transform.width - parcel->walkway.width;
            parcel->shield.height = 0;  // NOTE: NO SHIELD
            
            // Draw lake core
            struct ascoCell blankFloorCell = {TILE_WATER, 0, 0, 0};
            fillRectAuto(map, &blankFloorCell, &(parcel->transform), 
                parcel->walkway.width, parcel->shield.height, parcel->shield.width, parcel->transform.height - parcel->shield.height, 1); 
            */

            parcel->walkwayWidth = 1;
            parcel->shieldHeight = 0;
        
            // Draw lake core
            struct ascoCell lakeCell = {TILE_WATER, 0, 0, 0};
            fillRectAuto(map, &lakeCell, &(parcel->transform), 
                parcel->walkwayWidth, 0, parcel->transform.width - parcel->walkwayWidth, parcel->transform.height, 1); 


        break;
    }

    // Set gates (TODO bad gates here; refactor with selfHasGate)
    parcel->gates[0].position = 1;
    parcel->gates[0].size = 2;
    parcel->gates[1].position = parcel->transform.width - 3;
    parcel->gates[1].size = 2;
    parcel->gates[2].position = 1;
    parcel->gates[2].size = 2;
    parcel->gates[3].position = parcel->transform.width - 3;
    parcel->gates[3].size = 2;
}
