
#include "DFSMaze.h"
// DFSMaze.c
// See header for details

#include <string.h> // for memset in the realizer


void DFSMazeIdeator(struct parcel *parcel){

    // Ignore shape and parameters completely
    
    parcel->realizer = &DFSMazeRealizer;
    parcel->data = NULL;
    parcel->transform = newGridTransform();

    parcel->flexX = 1.0;    // Maze is very growable (though perhaps not as much as, say, a lake...?)
    parcel->flexY = 1.0;

    parcel->minWidth = parcel->parameters.gateWidth + 4;    // Slightly bulgeous, but needed to maintain gatewidth assumption...
    parcel->minHeight = parcel->parameters.gateWidth + 4;

    parcel->children = NULL;
    parcel->childCount = 0;

    // Transform field set by invoker

    // Residuals not set until realization
    
}




// Modification of our compact DFS maze generator (apologies to future self for the obfuscation)
// Lifted from v1
// Populates the indicated grid; 0 for floor, 1 for wall
static void DFSMaze(unsigned char *maze, int x, int y, int width, int height){
    maze[y * width + x] = 0;
    
    int r = rand() % 4;
    for(int t = 0; t < 4; t++){
        int d = (r + t) % 4;
        int dx = (d & 1 ? d-2 : 0) * 2;
        int dy = (d & 1 ? 0 : d-1) * 2;
        
        if(x + dx >= 0 && x + dx < width && y + dy >= 0 && y + dy < height && maze[(y+dy)*width+(x+dx)]){
            maze[(y+dy/2)*width+(x+dx/2)] = 0;
            DFSMaze(maze, x+dx, y+dy, width, height);
        }
    }
}


// Callback context and function for sampling from generated maze and writing to map
// For fillRectCallback below
struct mazeStampCallbackContext {
    struct ascoTileMap *map;
    unsigned char *maze;
    int width;
};


static void mazeStampCallback(void *context, int x, int y, int m, int n){
    // Cast context pointer
    struct mazeStampCallbackContext *contextPtr = (struct mazeStampCallbackContext *)context;

    // Copy cell
    struct ascoCell wallCell = {TILE_BLOCKAGE, 0, 0, 0};
    struct ascoCell floorCell = {TILE_BLANK, 0, 0, 0};

    mapCell(contextPtr->map, m, n) = contextPtr->maze[y * contextPtr->width + x] ? wallCell : floorCell;
}


void DFSMazeRealizer(void *context, struct parcel *parcel){
    // Cast context
    struct ascoTileMap *map = (struct ascoTileMap *)context;

    // Determine how large the maze core will be (the maze core MUST be of odd size)
    int coreWidth = parcel->transform.width - (parcel->parameters.pathWidth);
    if(!(coreWidth % 2)) coreWidth--;
    int coreHeight = parcel->transform.height;
    if(!(coreHeight % 2)) coreHeight--;

    parcel->walkwayWidth = parcel->transform.width - coreWidth;
    parcel->shieldHeight = parcel->transform.height - coreHeight;

    // Pregenerate maze separately (for modularity's sake, minimizes grid transform callbacks too, though wastes memory)
    unsigned char maze[coreWidth * coreHeight];
    memset(maze, 1, coreWidth * coreHeight * sizeof(unsigned char));
    DFSMaze(maze, 1, 1, coreWidth, coreHeight);


    // Iterate over the core with custom maze-sampling callback
    // Prepare transform representing core
    struct gridTransform t = newGridTransform();
    t.x = parcel->walkwayWidth;
    t.y = parcel->shieldHeight;
    t.width = coreWidth;
    t.height = coreHeight;
    gTInherit(&(parcel->transform), &t);

    // Prepare callback struct
    struct mazeStampCallbackContext callbackContext = {map, maze, coreWidth};

    // Run stamping iterator
    gTRegionIterate(&t, &callbackContext, &mazeStampCallback, 0, 0, coreWidth, coreHeight);
    

    // Set gates and holepunch into maze
    // TODO if we add the punched coords to a queue piggybacking on the maze pattern we could floodfill to find the furthest tile and place reward there
    struct ascoCell blankFloorCell = {TILE_BLANK, 0, 0, 0};

    parcel->gates[0].position = ((rand() % (coreHeight - parcel->parameters.gateWidth - 1)) | 1) + parcel->shieldHeight;
    if(selfHasGate(parcel->shape, 0)){
        parcel->gates[0].size = parcel->parameters.gateWidth;
        // Holepunch maze through to gate 2
        fillRect(map, &blankFloorCell, &(parcel->transform), parcel->walkwayWidth, parcel->gates[0].position, 1, 1);
    } else {
        parcel->gates[2].size = 0;
    }

    parcel->gates[1].position = ((rand() % (coreWidth - parcel->parameters.gateWidth - 1)) | 1) + parcel->walkwayWidth;
    if(selfHasGate(parcel->shape, 1)){
        parcel->gates[1].size = parcel->parameters.gateWidth;
        // Holepunch maze through to gate 1
        fillRect(map, &blankFloorCell, &(parcel->transform), parcel->gates[1].position, parcel->transform.height - 1, 1, 1);
    } else {
        parcel->gates[1].size = 0;
    }

    parcel->gates[2].position = ((rand() % (coreHeight - parcel->parameters.gateWidth - 1)) | 1) + parcel->shieldHeight;
    if(selfHasGate(parcel->shape, 2)){
        parcel->gates[2].size = parcel->parameters.gateWidth;
        // Holepunch maze through to gate 2
        fillRect(map, &blankFloorCell, &(parcel->transform), parcel->transform.width - 1, parcel->gates[2].position, 1, 1);
    } else {
        parcel->gates[2].size = 0;
    }


    // TODO No holepunching for gate 3 as yet (gate 3 still up in air)
    parcel->gates[3].position = parcel->transform.width - 3;
    parcel->gates[3].size = parcel->shape > TI_SHAPE ? parcel->parameters.gateWidth : 0;
    
}
