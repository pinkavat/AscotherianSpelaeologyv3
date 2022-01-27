
#include "lockSmash.h"
// lockSmash.c
// See header for details

#define MAX_CORE_SIZE 10    // If the parcel realizes to larger than this, extra space will be filled with walkway.


void lockSmashIdeator(struct parcel *parcel){

    // Ignore shape and parameters completely
    
    parcel->realizer = &lockSmashRealizer;
    parcel->data = NULL;
    parcel->transform = newGridTransform();

    parcel->flexX = 1.0;    // Rockfield wants to be as small as possible
    parcel->flexY = 1.0;

    parcel->minWidth = parcel->parameters.gateWidth + 3;    // + walkway
    parcel->minHeight = parcel->parameters.gateWidth + 2;

    parcel->children = NULL;
    parcel->childCount = 0;

    // Transform field set by invoker
    // Residuals not set until realization
    
}


// Helper macro for minimization
#define min(x, y) (((x) < (y)) ? (x) : (y))


// Callback for the gTIterate draw below
static void drawCallback(void *context, int x, int y, int m, int n){
    // Cast context pointer
    struct ascoTileMap *map = (struct ascoTileMap *)context;

    // Copy cell
    struct ascoCell rockCell = {TILE_ROCK_SMASH, 0, 0, 0};
    struct ascoCell floorCell = {TILE_BLANK, 0, 0, 0};

    mapCell(map, m, n) = (x == y || !(rand() % 8)) ? rockCell : floorCell;
}




void lockSmashRealizer(void *context, struct parcel *parcel){
    // Cast context
    struct ascoTileMap *map = ((struct ascoGenContext *)context)->map;

    // Determine square core size
    int coreSize = min(min(parcel->transform.width - parcel->parameters.pathWidth, parcel->transform.height), MAX_CORE_SIZE);

    parcel->walkwayWidth = parcel->transform.width - coreSize;
    parcel->shieldHeight = parcel->transform.height - coreSize;


    // Draw core; prep custom transform for gTIterate
    struct gridTransform t = newGridTransform();
    t.x = parcel->walkwayWidth;
    t.y = parcel->shieldHeight;
    t.width = coreSize;
    t.height = coreSize;
    t.flipV = 1;
    gTInherit(&(parcel->transform), &t);

    gTRegionIterate(&t, map, &drawCallback, 0, 0, coreSize, coreSize);
    

    // Set gates
    parcel->gates[0].position = parcel->transform.height - (parcel->parameters.gateWidth + 1);
    parcel->gates[0].size = selfHasGate(parcel->shape, 0) ? parcel->parameters.gateWidth : 0;
    parcel->gates[1].position = parcel->transform.width - (parcel->parameters.gateWidth + 1);
    parcel->gates[1].size = selfHasGate(parcel->shape, 1) ? parcel->parameters.gateWidth : 0;
    parcel->gates[2].position = parcel->transform.height - (parcel->parameters.gateWidth + 1);
    parcel->gates[2].size = selfHasGate(parcel->shape, 2) ? parcel->parameters.gateWidth : 0;
    parcel->gates[3].position = parcel->transform.width - (parcel->parameters.gateWidth + 1);
    parcel->gates[3].size = selfHasGate(parcel->shape, 3) ? parcel->parameters.gateWidth : 0;

}
