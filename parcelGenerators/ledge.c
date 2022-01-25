
#include "ledge.h"
// ledge.c
// See header for details

#define LEDGE_BLOCK_MIN_WIDTH 2
#define LEDGE_BLOCK_MIN_HEIGHT 3

#define max(a, b) ((a) > (b) ? (a) : (b))


void ledgeIdeator(struct parcel *parcel){

    // Ignore shape and parameters completely
    
    parcel->realizer = &ledgeRealizer;
    parcel->data = NULL;
    parcel->transform = newGridTransform();

    parcel->flexX = 0.3;    // TODO choose a good flex value for ledges (perhaps even dependent on block orientation??)
    parcel->flexY = 0.3;

    parcel->minWidth = (parcel->shape == TL_SHAPE) ? (parcel->parameters.gateWidth + parcel->parameters.pathWidth + LEDGE_BLOCK_MIN_HEIGHT) :
                        ((parcel->shape == I_SHAPE) ? (parcel->parameters.pathWidth + LEDGE_BLOCK_MIN_HEIGHT) : 
                        max(parcel->parameters.pathWidth + 1 + LEDGE_BLOCK_MIN_WIDTH, parcel->parameters.gateWidth));  // All other cases malformed
    parcel->minHeight = (parcel->shape == L_SHAPE) ? (1 + max(LEDGE_BLOCK_MIN_HEIGHT, parcel->parameters.gateWidth + 2)) : (
                        (parcel->shape == TI_SHAPE) ? (parcel->parameters.gateWidth + 1 + LEDGE_BLOCK_MIN_HEIGHT) : max(parcel->parameters.gateWidth + 2,
                        LEDGE_BLOCK_MIN_WIDTH));    // All other cases malformed

    parcel->children = NULL;
    parcel->childCount = 0;

    // Transform field set by invoker

    // Residuals not set until realization
    
}




// Draws the "ledge core" of a ledge parcel. The core is a group of one or more ledges enforcing travel from the top of the core to the bottom.
// TODO make more interesting
static void drawLedgeBlock(struct ascoTileMap *map, struct gridTransform *t){
    // Blank out floor
    struct ascoCell blankFloorCell = {TILE_BLANK, 4, 2, 0};
    fillRect(map, &blankFloorCell, t, 0, 0, t->width, t->height);


    // Every other row, draw a ledge
    for(int i = 1; i < t->height - 1; i+=2){
        drawLedge(map, t, 0, i, t->width, 1, 2);
    }
}


void ledgeRealizer(void *context, struct parcel *parcel){
    // Cast context
    struct ascoTileMap *map = ((struct ascoGenContext *)context)->map;

    parcel->walkwayWidth = parcel->parameters.pathWidth;
    parcel->shieldHeight = 0;

    // Compute position of core ledge block
    int ledgeBlockX = (parcel->shape == TI_SHAPE || parcel->shape == L_SHAPE) ? parcel->walkwayWidth + 1 :
         (parcel->shape == TL_SHAPE) ? parcel->parameters.gateWidth + 1: parcel->walkwayWidth;
    int ledgeBlockY = (parcel->shape == TI_SHAPE) ? parcel->parameters.gateWidth + 1 : ((parcel->shape == L_SHAPE) ? parcel->parameters.pathWidth + 1 : 0);

    // Draw core ledge block
    struct gridTransform ledgeBlockTransform = newGridTransform();
    ledgeBlockTransform.x = ledgeBlockX;
    ledgeBlockTransform.y = ledgeBlockY;
    ledgeBlockTransform.rotation = (parcel->shape == TL_SHAPE) ? 1 : ((parcel->shape == I_SHAPE) ? 3 : 0);

    if(ledgeBlockTransform.rotation & 1){
        ledgeBlockTransform.height = parcel->transform.width - ledgeBlockX;
        ledgeBlockTransform.width = parcel->transform.height - ledgeBlockY;
    } else {
        ledgeBlockTransform.width = parcel->transform.width - ledgeBlockX;
        ledgeBlockTransform.height = parcel->transform.height - ledgeBlockY;
    }


    gTInherit(&(parcel->transform), &(ledgeBlockTransform));
    drawLedgeBlock(map, &ledgeBlockTransform);

    // If L or TI, draw a blockage shield to the left of the ledge
    if(parcel->shape == L_SHAPE || parcel->shape == TI_SHAPE){
        struct ascoCell blockageCell = {TILE_BLOCKAGE, 0, 0, 0};
        fillRect(map, &blockageCell, &(parcel->transform), parcel->walkwayWidth, ledgeBlockY, 1, parcel->transform.height - ledgeBlockY);

        //...and a walkway up top
        struct ascoCell walkwayCell = {TILE_BLANK, 0, 0, 0};
        fillRect(map, &walkwayCell, &(parcel->transform),
            parcel->walkwayWidth, 0, parcel->transform.width - parcel->walkwayWidth, ledgeBlockY);
    }

    // If TL, add a walkway to the left of the block
    if(parcel->shape == TL_SHAPE){
        struct ascoCell walkwayCell = {TILE_BLANK, 0, 0, 0};
        fillRect(map, &walkwayCell, &(parcel->transform), parcel->walkwayWidth, 0, parcel->parameters.gateWidth, parcel->transform.height);
    }


    // Set gates
    parcel->gates[0].position = 1;
    parcel->gates[0].size = parcel->parameters.gateWidth;

    parcel->gates[1].position = (parcel->shape == TL_SHAPE) ? 1 : ledgeBlockX;
    parcel->gates[1].size = selfHasGate(parcel->shape, 1) ? parcel->parameters.gateWidth : 0;

    parcel->gates[2].position = 1;
    parcel->gates[2].size = selfHasGate(parcel->shape, 2) ? parcel->parameters.gateWidth : 0;

    parcel->gates[3].position = 1;
    parcel->gates[3].size = 0;  // Never a gate 3

}
