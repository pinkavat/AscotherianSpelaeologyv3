
#include "NPCEncounter.h"
// NPCEncounter.c
// See header for details


void NPCEncounterIdeator(struct parcel *parcel){

    // Ignore shape and parameters completely
    
    parcel->realizer = &NPCEncounterRealizer;
    parcel->data = NULL;
    parcel->transform = newGridTransform();

    parcel->flexX = 0.0;    // NPC Encounters don't really want to grow.
    parcel->flexY = 0.0;

    parcel->minWidth = 1 + parcel->parameters.gateWidth + parcel->parameters.pathWidth; // Akin to the base case (width is walkway-inclusive)
    parcel->minHeight = 2 + parcel->parameters.gateWidth;

    parcel->children = NULL;
    parcel->childCount = 0;

    // Transform field set by invoker

    // Residuals not set until realization
    
}


void NPCEncounterRealizer(void *context, struct parcel *parcel){
    // Cast context
    struct ascoTileMap *map = ((struct ascoGenContext *)context)->map;

    int coreWidth = 1 + parcel->parameters.gateWidth;
    int coreHeight = 2 + parcel->parameters.gateWidth;

    parcel->walkwayWidth = parcel->transform.width - coreWidth;
    parcel->shieldHeight = parcel->transform.height - coreHeight;

    // Blank out floor (akin to base case)
    struct ascoCell blankFloorCell = {TILE_BLANK, 0, 0, 0};
    fillRect(map, &blankFloorCell, &(parcel->transform), parcel->walkwayWidth, parcel->shieldHeight, coreWidth, coreHeight); 


    // Place NPC (TODO hooks)
    struct ascoCell NPCCell = {TILE_NPC_PLACEHOLDER, 0, 2, 0};
    placeCell(map, &NPCCell, &(parcel->transform), parcel->walkwayWidth, parcel->shieldHeight);


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
