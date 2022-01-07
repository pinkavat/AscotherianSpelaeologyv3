
#include "baseCase.h"
// baseCase.c
// See header for details

#define CORE_WIDTH 1
#define CORE_HEIGHT 1


void baseCaseIdeator(struct parcel *parcel){

    // Ignore shape and parameters completely
    
    parcel->realizer = &baseCaseRealizer;
    parcel->data = NULL;

    parcel->flexX = 0.0;    // If we hit this base case we don't want to make it any more obvious
    parcel->flexY = 0.0;    //  hence it doesn't want to grow any bigger

    parcel->minWidth =  CORE_WIDTH + 1; // Initial dimensions are minimal (width is walkway-inclusive)
    parcel->minHeight = CORE_HEIGHT;    // TODO may need to consult global params about minimum gate size...?

    parcel->children = NULL;
    parcel->childCount = 0;

    // Transform field set by invoker

    // Residuals not set until realization
    
}


void baseCaseRealizer(void *context, struct parcel *parcel){
    // Cast context
    struct ascoTileMap *map = (struct ascoTileMap *)context;

    // The base case must be able to handle all possible shapes
    switch(parcel->shape){
        case V_SHAPE:
            // TODO ???
        break;
        case E_SHAPE: case L_SHAPE: case I_SHAPE:
        case TL_SHAPE: case TI_SHAPE:

            /* TODO RELIC CODE REMOVE
            // Set walkway
            parcel->walkway = newGridTransform();
            parcel->walkway.width = parcel->transform.width - CORE_WIDTH;
            parcel->walkway.height = parcel->transform.height;
    
            // Set shield
            parcel->shield = newGridTransform();
            parcel->shield.x = parcel->walkway.width;
            parcel->shield.width = CORE_WIDTH;
            parcel->shield.height = parcel->transform.height - CORE_HEIGHT;
            
            // Draw blank floor core
            struct ascoCell blankFloorCell = {TILE_UNKNOWN, 0, 0, 0};   // TODO changed to unknown for debug purposes
            fillRect(map, &blankFloorCell, &(parcel->transform), parcel->walkway.width, parcel->shield.height, CORE_WIDTH, CORE_HEIGHT); 
            */

            parcel->walkwayWidth = parcel->transform.width - CORE_WIDTH;
            parcel->shieldHeight = parcel->transform.height - CORE_HEIGHT;

            struct ascoCell blankFloorCell = {TILE_UNKNOWN, 0, 0, 0};   // TODO changed to unknown for debug visualizer purposes
            fillRect(map, &blankFloorCell, &(parcel->transform), parcel->walkwayWidth, parcel->shieldHeight, CORE_WIDTH, CORE_HEIGHT); 


        break;
    }

    // Set gates (TODO!!)
    parcel->gates[0] = newGridTransform();
    parcel->gates[0].x = parcel->walkwayWidth;
    parcel->gates[0].y = parcel->shieldHeight;
    parcel->gates[0].width = parcel->minWidth;
    parcel->gates[0].height = parcel->minHeight;
    parcel->gates[1] = parcel->gates[0];
    parcel->gates[2] = parcel->gates[0];
}
