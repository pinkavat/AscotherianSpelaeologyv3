
#include "baseCase.h"
// baseCase.c
// See header for details

#define CORE_WIDTH 3    // TODO gate-size-relative, assumption for correct function
#define CORE_HEIGHT 4


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
        case XL_SHAPE: case XI_SHAPE:

            parcel->walkwayWidth = parcel->transform.width - CORE_WIDTH;
            parcel->shieldHeight = parcel->transform.height - CORE_HEIGHT;

            struct ascoCell blankFloorCell = {TILE_UNKNOWN, 0, 0, 0}; // TODO UNKNOWN FOR DEBUG
            fillRect(map, &blankFloorCell, &(parcel->transform), parcel->walkwayWidth, parcel->shieldHeight, CORE_WIDTH, CORE_HEIGHT); 


        break;
    }

    // Set gates (TODO global gate size...)
    parcel->gates[0].position = parcel->transform.height - 3;
    parcel->gates[0].size = parcel->shape > V_SHAPE ? 2 : 0;
    parcel->gates[1].position = parcel->transform.width - 3;
    parcel->gates[1].size = parcel->shape > E_SHAPE ? 2 : 0;
    parcel->gates[2].position = parcel->transform.height - 3;
    parcel->gates[2].size = parcel->shape > I_SHAPE ? 2 : 0;
    parcel->gates[3].position = parcel->transform.width - 3;
    parcel->gates[3].size = parcel->shape > TI_SHAPE ? 2 : 0;

}
