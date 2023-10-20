
#include "icePuzzle.h"
// icePuzzle.c
// See header for details

//   TODO FUTURE ME, IF YOU'RE READING THIS TO POACH THE ALGORITHM:
//       REMEMBER ICEFALL CAVE -- where PITS LEAD TO SPECIAL ICE LANES. ADD THIS
/*
    ALSO: further considerations for ice puzzles:
        1) Multipaths (generalized form of icefall)
        2) Out-of-phase validity proof 
        3) Failsides
        4) Obstacle surrounding (horizontal-vertical line matching)
        5) Aligning a strength boulder to stop at correct output edge (seafoams)

*/

void icePuzzleIdeator(struct parcel *parcel){

    // Ignore shape and parameters completely
    
    parcel->realizer = &icePuzzleRealizer;
    parcel->data = NULL;
    parcel->transform = newGridTransform();

    parcel->flexX = 1.0;    // Ice puzzle wants to be nice and large
    parcel->flexY = 1.0;

    parcel->minWidth = 8 + parcel->parameters.pathWidth;   // This is one of the gate-width unsafe ops (TODO)
    parcel->minHeight = 7;

    parcel->children = NULL;
    parcel->childCount = 0;

    // Transform field set by invoker
    // Residuals not set until realization
    
}



// Helper for ice puzzle generation: Fisher-Yates shuffle
static void shuffleInts(int arr[], size_t size){
    for(int i = size - 1; i > 0; i--){
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}


void icePuzzleRealizer(void *context, struct parcel *parcel){
    // Cast context
    struct ascoTileMap *map = ((struct ascoGenContext *)context)->map;

    // Determine how large the core will be (the core MUST be of odd height and even width)
    int coreWidth = parcel->transform.width - (parcel->parameters.pathWidth);
    if((coreWidth % 2)) coreWidth--;
    int coreHeight = parcel->transform.height;
    if(!(coreHeight % 2)) coreHeight--;

    parcel->walkwayWidth = parcel->transform.width - coreWidth;
    parcel->shieldHeight = parcel->transform.height - coreHeight;


    // Preliminary backdrop filling
    // 1) Surround icefield with blockage
    struct ascoCell perimeterCell = {TILE_BLOCKAGE, 0, 0, 0};
    fillRectAuto(map, &perimeterCell, &(parcel->transform), parcel->walkwayWidth, parcel->shieldHeight, coreWidth, coreHeight, 0);

    // 2) Leftside landing (TODO fail drain is this for now)
    struct ascoCell landingCell = {TILE_BLANK, 0, 0, 0};
    fillRect(map, &landingCell, &(parcel->transform), parcel->walkwayWidth, parcel->shieldHeight + 1, 2, coreHeight - 2);

    // 3) Core Icefield
    struct ascoCell iceCell = {TILE_ICE, 0, 0, 0};
    // TODO is ice MS or not?
    fillRect(map, &iceCell, &(parcel->transform), parcel->walkwayWidth + 2, parcel->shieldHeight + 1, coreWidth - 3, coreHeight - 2);




    // Ice puzzle path generation process
    // 1) Establish how many "slide lines" are on each axis, based on the core size
    int numXLines = ((coreWidth - 3) / 2) - 1;
    int numYLines = ((coreHeight - 2) / 2) - 1;

    // 2) Create arrays of these slide lines
    int xLines[numXLines];
    for(int i = 0; i < numXLines; i++) xLines[i] = i;
    int yLines[numYLines];
    for(int i = 0; i < numYLines; i++) yLines[i] = i;

    // 3) Shuffle the arrays to get our traversal path
    shuffleInts(xLines, numXLines);
    shuffleInts(yLines, numYLines);

    // 4) "Walk" path made of slide lines to place blocking tiles
    struct ascoCell iceBlockerCell = {TILE_BLOCKAGE, 0, 0, 0};

    int leastDimension = (numXLines < numYLines) ? numXLines : numYLines;
    int numMoves = (parcel->shape == L_SHAPE) ? (leastDimension * 2 - 1) : (leastDimension * 2 - 2);
    int dirToggle = 0;  // Start on a Y
    // L-shape ends on an X
    // I-shape ends on a Y
    int xIndex = 0, yIndex = 0;
    int oldX = -1, oldY = yLines[0];

    for(int i = 0; i < numMoves; i++){
        // Move to the next intersection and place a boulder there

        int stopX = 2 * xLines[xIndex] + 4 + parcel->walkwayWidth;
        int stopY = 2 * yLines[yIndex] + 3 + parcel->shieldHeight;
        
        //placeCell(map, &landingCell, &(parcel->transform), stopX, stopY);   // Has no effect on gameplay. Happen at random...?
        // TODO in fact, we could alternate between placing rocks and placing stops. Under our assumptions they're equivalent!

        if(dirToggle){
            placeCell(map, &iceBlockerCell, &(parcel->transform), stopX, stopY + ((oldY > stopY) ? -1 : 1));
            xIndex++;
        } else {
            placeCell(map, &iceBlockerCell, &(parcel->transform), stopX + ((oldX > stopX) ? -1 : 1), stopY);

            // If we've placed a stop that the player could "skip" to, lay down a blockage preventing this
            // TODO this op is sensitive to the direction of the fail drain; when we add variable fail drains we need to change this
            if(yIndex > 0) placeCell(map, &perimeterCell, &(parcel->transform), parcel->walkwayWidth + 1, stopY);

            yIndex++;
        }
        dirToggle = !dirToggle;
        oldX = stopX;
        oldY = stopY;
    }


    // Preset exit gates (overwritten by burrowing below)
    parcel->gates[1].position = 0;
    parcel->gates[1].size = 0;
    parcel->gates[2].position = 0;
    parcel->gates[2].size = 0;


    // Finally burrow exit gate
    // TODO this is a gate-size-dangerous op as well
    if(dirToggle){

        // L-case: last X line
        int terminalX = 2 * xLines[xIndex] + 4 + parcel->walkwayWidth;
        placeCell(map, &iceCell, &(parcel->transform), terminalX, parcel->transform.height - 1);
        // Set gate 1
        parcel->gates[1].position = terminalX;
        parcel->gates[1].size = parcel->parameters.gateWidth;

    } else {

        // I-case: last Y line
        int terminalY = 2 * yLines[yIndex] + 3 + parcel->shieldHeight;
        placeCell(map, &iceCell, &(parcel->transform), parcel->transform.width - 1, terminalY);
        // Set gate 2
        parcel->gates[2].position = terminalY;
        parcel->gates[2].size = parcel->parameters.gateWidth;

    }
    

    // TODO generation
    // TODO shape basis
        // E-shape...?
        // L/I shapes normal
        // TLS/TLT/TI shapes have a branch (which is always AFTER main exit in classic, and not part of puzzle in fail mode)
        // NOTE: minimum-size puzzle doesn't permit branches of this kind (not enough lines!)
        // Fail mode possible if exists in deg-3 or greater parcel
        // X-shapes??? Leave until later



    // Set gate zero
    parcel->gates[0].position = (parcel->transform.height / 2) - (parcel->parameters.gateWidth / 2);
    parcel->gates[0].size = parcel->parameters.gateWidth;

    // TODO X-shape support
    parcel->gates[3].position = 1;
    parcel->gates[3].size = 0;
 }
