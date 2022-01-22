
#include "voidBubble.h"
// voidBubble.c
// See header for details


void voidBubbleIdeator(struct parcel *parcel){

    // Force shape to conform (don't have a good malformed-map failure handling system yet)
    parcel->shape = V_SHAPE;
    
    parcel->realizer = &voidBubbleRealizer;
    parcel->data = NULL;
    parcel->transform = newGridTransform();

    parcel->flexX = 0.5;    // TODO this may need tuning (or may even derive from global params!!!!)
    parcel->flexY = 0.5;

    parcel->minWidth = 1;   // TODO any limitations on this? Any good values to choose?
    parcel->minHeight = 1;

    parcel->children = NULL;
    parcel->childCount = 0;

    // Transform field set by invoker
    // Residuals not set until realization
}


void voidBubbleRealizer(void *context, struct parcel *parcel){
    // Cast context
    struct ascoTileMap *map = ((struct ascoGenContext *)context)->map;

    // Fill with blockage (as we now solve void with the postprocessor)
    struct ascoCell inaccessibleCell = {TILE_BLOCKAGE, 0, 0, 0};
    fillRect(map, &inaccessibleCell, &(parcel->transform), 0, 0, parcel->transform.width, parcel->transform.height);

    // Blank out gates and size buffers, as expected:
    parcel->walkwayWidth = 0;
    parcel->shieldHeight = 0;
    parcel->gates[0].size = 0;
    parcel->gates[1].size = 0;
    parcel->gates[2].size = 0;
    parcel->gates[3].size = 0;
}
