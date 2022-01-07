
#include "testLake.h"
// testLake.c
// See header for details


void testLakeIdeator(struct parcel *parcel){

    // Ignore shape and parameters completely
    
    parcel->realizer = &testLakeRealizer;
    parcel->data = NULL;

    parcel->flexX = 1.0;    // Test lake absolutely wants to resize as much as possible
    parcel->flexY = 1.0;

    parcel->minWidth = 3;   // Initial dimensions are minimal (width is walkway-inclusive)
    parcel->minHeight = 2;  // Height is NOT shield-inclusive, shields are optional

    parcel->children = NULL;
    parcel->childCount = 0;

    // Transform field set by invoker

    // Residuals not set until realization
    
}


void testLakeRealizer(void *context, struct parcel *parcel){
    // Cast context
    struct ascoTileMap *map = (struct ascoTileMap *)context;

    // The base case must be able to handle all possible shapes
    switch(parcel->shape){
        case V_SHAPE:
            // TODO ????? 
        break;
        case E_SHAPE: case L_SHAPE: case I_SHAPE:
        case TL_SHAPE: case TI_SHAPE:

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

    // Set gates (TODO bad gates here)
    parcel->gates[0] = newGridTransform();
    parcel->gates[0].x = parcel->walkwayWidth;
    parcel->gates[0].y = parcel->shieldHeight;
    parcel->gates[0].width = parcel->minWidth;
    parcel->gates[0].height = parcel->minHeight;
    parcel->gates[1] = parcel->gates[0];
    parcel->gates[2] = parcel->gates[0];
}
