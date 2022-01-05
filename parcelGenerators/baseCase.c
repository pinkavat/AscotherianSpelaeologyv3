
#include "baseCase.h"
// baseCase.c
// See header for details


void baseCaseIdeator(struct parcel *parcel){

    // Ignore shape and parameters completely
    
    parcel->realizer = &baseCaseRealizer;
    parcel->data = NULL;

    parcel->flexX = 0.0;    // If we hit this base case we don't want to make it any more obvious
    parcel->flexY = 0.0;    //  hence it doesn't want to grow any bigger

    parcel->minWidth = 1;   // Initial dimensions are minimal (width is walkway-inclusive)
    parcel->minHeight = 1;  // TODO may need to consult global params about minimum gate size...?

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
            // TODO ?????
        break;
        case E_SHAPE: case L_SHAPE: case I_SHAPE:
        case TL_SHAPE: case TI_SHAPE:

            // Set walkway
            parcel->walkway = newGridTransform();
            parcel->walkway.width = parcel->transform.width - parcel->minWidth;
            parcel->walkway.height = parcel->transform.height;
    
            // Set shield
            parcel->shield = newGridTransform();
            parcel->shield.x = parcel->walkway.width;
            parcel->shield.width = parcel->minWidth;
            parcel->shield.height = parcel->transform.height - parcel->minHeight;
            
            // Draw blank floor core
            struct ascoCell blankFloorCell = {TILE_BLANK, 0, 0, 0};
            fillRect(map, &blankFloorCell, &(parcel->transform), parcel->walkway.width, parcel->shield.height, parcel->minWidth, parcel->minHeight); 

        break;
        case XL_SHAPE: case XI_SHAPE:
            // X-shapes don't have the upper blockage shield (TODO true?)
            // TODO more thought is needed, perhaps X-shapes need to go as they may break assumptions
        break;
    }

    // Set gates
    parcel->gates[0] = newGridTransform();
    parcel->gates[0].x = parcel->walkway.width;
    parcel->gates[0].y = parcel->shield.height;
    parcel->gates[0].width = parcel->minWidth;
    parcel->gates[0].height = parcel->minHeight;
    parcel->gates[1] = parcel->gates[0];
    parcel->gates[2] = parcel->gates[0];
    parcel->gates[3] = parcel->gates[0];
}
