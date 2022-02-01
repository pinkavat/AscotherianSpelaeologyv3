
#include "recursorGridGenerator.h"
// recursorGridGenerator.c
// See header for details

#include <string.h> // For memcpy for signature copying


#define WALKWAY_WIDTH parcel->parameters.pathWidth


// ==================== HEAP MANAGEMENT FOR SIGNATURE COPYING ====================

// Why isn't there a standard memdup, do you suppose?
static inline void *memdup(void *source, size_t size){
    void *duplicate = malloc(size);
    if(duplicate != NULL) memcpy(duplicate, source, size);
    return duplicate;
}


// Copy a grid signature to the heap and return a pointer to it
static struct recursorGridSignature *copyGridSignature(const struct recursorGridSignature *signature){
    struct recursorGridSignature *signatureCopy = (void *)malloc(sizeof(struct recursorGridSignature));
    
    signatureCopy->width = signature->width;
    signatureCopy->height = signature->height;

    int numCells = signature->width * signature->height;

    signatureCopy->shapes = memdup(signature->shapes, numCells * sizeof(enum parcelShapes)); 
    signatureCopy->populatorFunctions = memdup(signature->populatorFunctions, numCells * sizeof(cellPopulatorFunctionPtr)); 

    signatureCopy->rotations = memdup(signature->rotations, numCells * sizeof(unsigned int)); 
    signatureCopy->flipHs = memdup(signature->flipHs, numCells * sizeof(unsigned int)); 
    signatureCopy->flipVs = memdup(signature->flipVs, numCells * sizeof(unsigned int)); 
    
    memcpy(signatureCopy->gateSourceIndices, signature->gateSourceIndices, 4 * sizeof(int));
    memcpy(signatureCopy->gateIsFork, signature->gateIsFork, 4 * sizeof(int));

    return signatureCopy;
}


// Free a heap-resident grid signature (one that was created with the above function)
static void freeGridSignatureCopy(struct recursorGridSignature *signature){

    free(signature->shapes);
    free(signature->populatorFunctions);
    free(signature->rotations);
    free(signature->flipHs);
    free(signature->flipVs);

    free(signature);
}





// ==================== FLEXIBILITY MANAGEMENT ====================

// TODO strategy one for flex score amalgamation: INDIVIDUAL row/col score is AVERAGE of all subscores.
// Helper for both ideator and realizer; computes row/col min dimensions and flex scores of children
static void computeMinDimsAndFlexScores(struct parcel *children, struct sheathData *sheathes, 
    int *minColDims, int *minRowDims, float *xFlexes, float *yFlexes, int width, int height){

    // Reset column min dims and flexes
    for(int x = 0; x < width; x++){
        minColDims[x] = 0;
        xFlexes[x] = 0.0;
    }

    for(int y = 0; y < height; y++){
        // Reset row min dims and flexes
        minRowDims[y] = 0;
        yFlexes[y] = 0.0;
        for(int x = 0; x < width; x++){
            // Compute minimum dimensions
            struct parcel *child = &(children[(y * width) + x]);

            int sheathWidth, sheathHeight;
            getSheathMinSize(&(sheathes[(y * width) + x]), &sheathWidth, &sheathHeight);
            int minWidth = parcelAbsMinWidth(child) + sheathWidth;
            int minHeight = parcelAbsMinHeight(child) + sheathHeight;
            
            if(minWidth > minColDims[x]) minColDims[x] = minWidth;
            if(minHeight > minRowDims[y]) minRowDims[y] = minHeight;

            // Add flex scores
            xFlexes[x] = xFlexes[x] + parcelAbsFlexX(child);            
            yFlexes[y] = yFlexes[y] + parcelAbsFlexY(child);            
        }
        // Divide row flex score to get the mean
        yFlexes[y] /= (float)width;
    }

    // Divide all column flex scores to get the means
    for(int x = 0; x < width; x++){
        xFlexes[x] /= (float)height;
    }

}


// TODO strategy one: individual row/col score is average of all subscores.
// Helper for the realizer; distributes one dimensional increase based on the flex scores of either a row or a column
// Obvious precondition: length is at least one (and both arrays have said length)
static void distributeDimensionalIncreases(int increase, int *dimensions, float *flexes, int length){
    // 1) Sum flex values
    float sumFlexes = 0.0;
    for(int i = 0; i < length; i++) sumFlexes += flexes[i];

    // 2) For each cell in the line, add a proportional amount of the increase to its dimension
    int remainder = increase;
    for(int i = 0; i < length; i++){
        int addedIncrease = (sumFlexes == 0.0) ? (increase / length) : ((float)increase * (flexes[i] / sumFlexes));
        dimensions[i] += addedIncrease;
        // ...and subtract it from the remainder
        remainder -= addedIncrease;
    }
    
    // 3) In the event that the increase doesn't divide perfectly, the first cell eats the remainder
    dimensions[0] += remainder;
}






// Helper for the Helper for computing sheath data; fetches the z of the parcel at (x, y), or 0 if the requested index is out-of-bounds
static int heightAdjHelperCore(struct parcel *children, int x, int y, int width, int height){
    if(x < 0 || x >= width || y < 0 || y >= height) return 0;
    return children[(y * width) + x].transform.z;
}

// Helper for computing sheath data; populates the child (given by x, y)'s height adjacencies
//  ...lots of function call overhead *sigh*
static void heightAdjHelper(struct parcel *children, int x, int y, int width, int height, int heightAdj[9]){
    heightAdj[0] = heightAdjHelperCore(children, x-1, y-1, width, height);  // TL
    heightAdj[1] = heightAdjHelperCore(children, x  , y-1, width, height);  // T
    heightAdj[2] = heightAdjHelperCore(children, x+1, y-1, width, height);  // TR
    heightAdj[3] = heightAdjHelperCore(children, x+1, y  , width, height);  // R
    heightAdj[4] = heightAdjHelperCore(children, x+1, y+1, width, height);  // BR
    heightAdj[5] = heightAdjHelperCore(children, x  , y+1, width, height);  // B
    heightAdj[6] = heightAdjHelperCore(children, x-1, y+1, width, height);  // BL
    heightAdj[7] = heightAdjHelperCore(children, x-1, y  , width, height);  // L
    // Self is already set
}



// ==================== IDEATOR ====================

void recursorGridIdeator(struct parcel *parcel, const struct recursorGridSignature *signature){

    // Shape has no meaning as yet
    // Parameters mean nothing yet

    // 1) Set the realizer and data fields
    parcel->realizer = &recursorGridRealizer;
    parcel->data = (void *)malloc(sizeof(struct recursorGridDataStruct));
    struct recursorGridDataStruct *dataStruct = (struct recursorGridDataStruct *)(parcel->data);

    // Copy over signature
    dataStruct->signatureCopy = copyGridSignature(signature);

    // Blank out transform
    parcel->transform = newGridTransform();


    // 2) Allocate child memory
    parcel->childCount = signature->width * signature->height;
    parcel->children = (void *)malloc(sizeof(struct parcel) * parcel->childCount);

    // 3) Divide up child parameters
    divideParametersByGrid(&(parcel->parameters), signature, parcel->children);

    // 4) Set child shapes; generate children
    // 5) ...and transform children in same loop
    for(int i = 0; i < parcel->childCount; i++){

        // Set child shape
        parcel->children[i].shape = signature->shapes[i];

        // Ideate child
        (*(signature->populatorFunctions[i]))(&(parcel->children[i]));

        // Transform (rotate and mirror, not translate yet) child from signature
        parcel->children[i].transform.rotation = signature->rotations[i];
        parcel->children[i].transform.flipH = signature->flipHs[i];
        parcel->children[i].transform.flipV = signature->flipVs[i];
    }

    // 6) Set child heights
    if(parcel->parameters.recursionDepth >= 0) gridHeightSelect(parcel, signature);


    // 7) Compute sheath data
    // This loop could probably be single, but better to be sure as the human than efficient as the computer
    dataStruct->sheathes = (void *)malloc(sizeof(struct sheathData) * signature->width * signature->height);
    for(int y = 0; y < signature->height; y++){
        for(int x = 0; x < signature->width; x++){

            int curIndex = (y * signature->width) + x;
            struct parcel *child = &(parcel->children[curIndex]);

            int topoAdj[4] = {1, 1, 1, 1};

            // General check
            if(y > 0 && !otherHasGate(child->shape, &(child->transform), 3)) topoAdj[0] = 0;   // Top gate
            if(x > 0 && !otherHasGate(child->shape, &(child->transform), 0)) topoAdj[3] = 0;   // Left gate

            // Recursor walkway clash preventor
            if(x == 0 && !otherHasGate(child->shape, &(child->transform), 0) && parcel->shape != V_SHAPE) topoAdj[3] = 0;

            // L-case inelegant solution (TODO improve)
            // Only works for gate 1 for now, not gate 3
            int throwaway;
            if((getGateIndex(&(child->transform), 3, &throwaway) == 1) && selfHasGate(child->shape, 1)) topoAdj[0] = 0;
            if((getGateIndex(&(child->transform), 2, &throwaway) == 1) && selfHasGate(child->shape, 1)) topoAdj[1] = 0;
            if((getGateIndex(&(child->transform), 1, &throwaway) == 1) && selfHasGate(child->shape, 1)) topoAdj[2] = 0;
            if((getGateIndex(&(child->transform), 0, &throwaway) == 1) && selfHasGate(child->shape, 1)) topoAdj[3] = 0;
        

            // Compute height-adj with a helper
            int heightAdj[9];
            heightAdj[8] = child->transform.z;
            heightAdjHelper(parcel->children, x, y, signature->width, signature->height, heightAdj);
           
            // Hand data off to the sheath solver
            computeSheathData(&(dataStruct->sheathes[curIndex]), topoAdj, heightAdj);

        }
    }


    // 8) Compute row/col min dimensions and flex scores
    int minColDims[signature->width];
    int minRowDims[signature->height];
    float xFlexes[signature->width];
    float yFlexes[signature->height];
    computeMinDimsAndFlexScores(parcel->children, dataStruct->sheathes, minColDims, minRowDims, xFlexes, yFlexes, signature->width, signature->height);

    // 9) Establish overall minimum dimensions and flex scores
    parcel->minWidth = (parcel->shape == V_SHAPE) ? 0 : WALKWAY_WIDTH;    // Initially add walkway width if non-V
    for(int i = 0; i < signature->width; i++) parcel->minWidth += minColDims[i];
    parcel->minHeight = 0;
    for(int i = 0; i < signature->height; i++) parcel->minHeight += minRowDims[i];

    // TODO strategy one for flex score amalgamation: OVERALL flex score is MAXIMUM of row/col subscores.
    parcel->flexX = 0.0;
    for(int i = 0; i < signature->width; i++){
        if(xFlexes[i] > parcel->flexX) parcel->flexX = xFlexes[i];
    }
    parcel->flexY = 0.0;
    for(int i = 0; i < signature->height; i++){
        if(yFlexes[i] > parcel->flexY) parcel->flexY = yFlexes[i];
    }


    // Transform field set by invoker
    // Residuals not set until realization
}






// ==================== SHEATH REALIZER HELPER ====================

// Lookup table for edges to get edge tile
// Also used by corners in flat edge case
// Ordered by enum in sheath.h; depends on its being correct
const struct ascoCell edgeLookup[4] = {
    {TILE_BLOCKAGE, 0, 0, 0},   // Blockage
    {TILE_UNRESOLVED, 0, 0, 0}, // Flat
    {TILE_CLIFF, 1, 2, 0},      // Up
    {TILE_CLIFF, 1, 0, -1}      // Down
};

const struct ascoCell gateLookup[4] = {
    {TILE_BLANK, 0, 0, 0},      // Blockage
    {TILE_BLANK, 0, 0, 0},      // Flat
    {TILE_STAIR, 0, 2, 0},      // Up
    {TILE_STAIR, 0, 0, -1}      // Down
};


// Helper for realizeSheath
// I tried to use lookups to help; they clashed with rotation and went down to grim defeat.
// This is a step to optimize later, for sure.
static void drawSheathCorner(struct ascoTileMap *map, struct gridTransform *t, int cornerRotation,
enum sheathCornerTypes self, enum sheathEdgeTypes left, enum sheathEdgeTypes right){

    if(left == SHEATH_EDGE_NONE || right == SHEATH_EDGE_NONE) return;   // No corner if either edge is missing

    // Otherwise corner is drawn
    struct ascoCell cornerCell = {TILE_BLOCKAGE, 0, cornerRotation, 0};
    if((left < SHEATH_EDGE_UP) && (right < SHEATH_EDGE_UP)){
        // Edges exist and are not cliffs
        if(self == SHEATH_CORNER_BLOCKAGE){
            // Corner is a blockage
            cornerCell.rotation = 0;
        } else {
            // Corner is concave from below or convex from above, as per the self's height
            cornerCell.tile = TILE_CLIFF;
            cornerCell.variant = (self == SHEATH_CORNER_DOWN) ? 3 : 2;
            cornerCell.rotation = (cornerCell.rotation + ((self == SHEATH_CORNER_DOWN) ? 0 : 2)) % 4;
            cornerCell.z = (self == SHEATH_CORNER_DOWN) ? -1 : 0;
        }
    } else if (left == SHEATH_EDGE_DOWN && right == SHEATH_EDGE_DOWN){
        // Convex down case
        cornerCell.tile = TILE_CLIFF;
        cornerCell.variant = 2;
        cornerCell.z = -1;
    } else if (left == SHEATH_EDGE_UP && right == SHEATH_EDGE_UP){
        // Concave up case
        cornerCell.tile = TILE_CLIFF;
        cornerCell.variant = 3;
        cornerCell.rotation = (cornerCell.rotation + 2) % 4;
    } else {
        // Flat edge case
        if(left < SHEATH_EDGE_UP){
            // Copy right
            cornerCell = edgeLookup[right];
            cornerCell.rotation = (cornerCell.rotation + cornerRotation + 1) % 4;
        } else {
            // Copy left
            cornerCell = edgeLookup[left];
            cornerCell.rotation = (cornerCell.rotation + cornerRotation) % 4;
        }
    }// Other cases are malformed

    // Determine corner coordinate from rotation value and place corner
    int x = (cornerRotation & 2) ? 0 : t->width - 1;
    int y = (cornerRotation == 3 || cornerRotation == 0) ? 0 : t->height - 1;
    placeCell(map, &cornerCell, t, x, y);

}



// Helper for recursorGridRealizer; Realizes the given sheath into the map; the transform provided represents the area of the entire sheath
static void realizeSheath(struct ascoTileMap *map, struct gridTransform *t, struct sheathData *sheath, gateSet gates){
    
    // Draw edges (corners will overwrite a bit if present)
    if(sheath->edges[0] != SHEATH_EDGE_NONE){
        // Top edge
        struct ascoCell cell = edgeLookup[sheath->edges[0]];
        // cell.rotation = (cell.rotation + 0) % 4;
        fillRect(map, &cell, t, 0, 0, t->width, 1);
    }
    if(sheath->edges[1] != SHEATH_EDGE_NONE){
        // Right edge
        struct ascoCell cell = edgeLookup[sheath->edges[1]];
        cell.rotation = (cell.rotation + 1) % 4;
        fillRect(map, &cell, t, t->width - 1, 0, 1, t->height);
    }
    if(sheath->edges[2] != SHEATH_EDGE_NONE){
        // Bottom edge
        struct ascoCell cell = edgeLookup[sheath->edges[2]];
        cell.rotation = (cell.rotation + 2) % 4;
        fillRect(map, &cell, t, 0, t->height - 1, t->width, 1);
    }
    if(sheath->edges[3] != SHEATH_EDGE_NONE){
        // Left edge
        struct ascoCell cell = edgeLookup[sheath->edges[3]];
        cell.rotation = (cell.rotation + 3) % 4;
        fillRect(map, &cell, t, 0, 0, 1, t->height);
    }


    // Draw corners    
    drawSheathCorner(map, t, 3, sheath->corners[0], sheath->edges[3], sheath->edges[0]);    // Top left
    drawSheathCorner(map, t, 0, sheath->corners[1], sheath->edges[0], sheath->edges[1]);    // Top right
    drawSheathCorner(map, t, 1, sheath->corners[2], sheath->edges[1], sheath->edges[2]);    // Bottom right
    drawSheathCorner(map, t, 2, sheath->corners[3], sheath->edges[2], sheath->edges[3]);    // Bottom left


    // Draw gates
    // Transform gates to bring them into sheathspace from corespace
    if(sheath->edges[0] != SHEATH_EDGE_NONE){
        gates[0].position++;
        gates[2].position++;
    }
    if(sheath->edges[3] != SHEATH_EDGE_NONE){
        gates[1].position++;
        gates[3].position++;
    }

    // The sheather draws all gates, so all gates must be well-formed
    if(sheath->edges[3] != SHEATH_EDGE_NONE){
        // Draw gate 0 (Left edge)
        struct ascoCell cell = gateLookup[sheath->edges[3]];
        cell.rotation = (cell.rotation + 3) % 4;
        fillRect(map, &cell, t, 0, gates[0].position, 1, gates[0].size);
    }
    if(sheath->edges[2] != SHEATH_EDGE_NONE){
        // Draw gate 1 (Bottom edge)
        struct ascoCell cell = gateLookup[sheath->edges[2]];
        cell.rotation = (cell.rotation + 2) % 4;
        fillRect(map, &cell, t, gates[1].position, t->height - 1, gates[1].size, 1);
    }
    if(sheath->edges[1] != SHEATH_EDGE_NONE){
        // Draw gate 2 (Right edge)
        struct ascoCell cell = gateLookup[sheath->edges[1]];
        cell.rotation = (cell.rotation + 1) % 4;
        fillRect(map, &cell, t, t->width - 1, gates[2].position, 1, gates[2].size);
    }
    if(sheath->edges[0] != SHEATH_EDGE_NONE){
        // Draw gate 3 (Top edge)
        struct ascoCell cell = gateLookup[sheath->edges[0]];
        //cell.rotation = (cell.rotation + 0) % 4;
        fillRect(map, &cell, t, gates[3].position, 0, gates[3].size, 1);
    }
}







// ==================== REALIZER ====================

void recursorGridRealizer(void *context, struct parcel *parcel){

    // Cast context
    struct ascoGenContext *castContext = (struct ascoGenContext *)context;
    struct ascoTileMap *map = castContext->map;
    struct recursorGridDataStruct *dataStruct = (struct recursorGridDataStruct *)(parcel->data);
    struct recursorGridSignature *signature = dataStruct->signatureCopy;


    // 1) Recompute row/col dimensions and flex scores
    int colDims[signature->width];
    int rowDims[signature->height];
    float xFlexes[signature->width];
    float yFlexes[signature->height];
    computeMinDimsAndFlexScores(parcel->children, dataStruct->sheathes, colDims, rowDims, xFlexes, yFlexes, signature->width, signature->height);


    // 2) Distribute dimensional increases across rows and columns
    distributeDimensionalIncreases(parcel->transform.width - parcel->minWidth, colDims, xFlexes, signature->width);     // x
    distributeDimensionalIncreases(parcel->transform.height - parcel->minHeight, rowDims, yFlexes, signature->height);  // y

    // *Part 6 sneaks in here* Set up gazumption's precompute array into which gazumping gate indices are stored
    int gazumptionGateIndices[signature->width * signature->height];
    int gazumperIndices[signature->width * signature->height];
    for(int i = 0; i < signature->width * signature->height; i++) gazumperIndices[i] = -1;
    
    // NOTE: Because we're idiots, this design is going to get even more inefficient and hacky. The sheather realizer needs to know the original transforms
    // of the source rects, in order to place the gates correctly. We're going to piggyback the gazumper on this data also.
    struct gridTransform oldTransforms[signature->width * signature->height];

    // If the recursor is V-shaped, no walkway is present:
    int walkwayWidth = (parcel->shape == V_SHAPE) ? 0 : WALKWAY_WIDTH;


    // 3) Distribute dimensional increases across individual child parcels
    // 4) ...and translate children while we have a loop open for old time's sake
    int translationCursorX = walkwayWidth; // offset for self's walkway
    int translationCursorY = 0;
    // 5) ...and realize the child parcels as well, come to think of it
    for(int y = 0; y < signature->height; y++){
        for(int x = 0; x < signature->width; x++){

            int curIndex = (y * signature->width) + x;
            struct parcel *child = &(parcel->children[curIndex]);


            // *Part 6 sneaks in here* gazumption needs a precompute step to account for rotation inheritance
            // If the child has no gate zero, no gazumption.
            if(child->shape != V_SHAPE){
                // Otherwise, store its neighbor's index and appropriate gate for gazumption
                unsigned int rotation = child->transform.rotation;
                const unsigned int rotFlipLookup[4] = {2, 3, 0, 1}; // Another bloody kludge
                if(child->transform.flipH) rotation = rotFlipLookup[rotation];
                switch(rotation){
                    case 0: // Gazump from the left
                        if(x > 0){
                            gazumperIndices[curIndex] = (y * signature->width) + (x-1);
                            gazumptionGateIndices[curIndex] = 2;
                        }
                    break;
                    case 1: // Gazump from above
                        if(y > 0){
                            gazumperIndices[curIndex] = ((y-1) * signature->width) + x;
                            gazumptionGateIndices[curIndex] = 1;
                        }
                    break;
                    case 2: // Gazump from the right
                        if(x < signature->width - 1){
                            gazumperIndices[curIndex] = (y * signature->width) + (x+1);
                            gazumptionGateIndices[curIndex] = 0;
                        }
                    break;
                    case 3: // Gazump from below
                        if(y < signature->height - 1){
                            gazumperIndices[curIndex] = ((y+1) * signature->width) + x;
                            gazumptionGateIndices[curIndex] = 3;
                        }
                    break;
                }
            }
 

            // 3a) Fetch new width and height, subtracting the child's sheath dimensions
            int sheathWidth, sheathHeight;
            getSheathMinSize(&(dataStruct->sheathes[curIndex]), &sheathWidth, &sheathHeight);

            int newWidth = colDims[x] - sheathWidth;
            int newHeight = rowDims[y] - sheathHeight;


            // 3b) Transform to match child
            if(child->transform.rotation & 1){
                int temp = newWidth;
                newWidth = newHeight;
                newHeight = temp;
            }

            // 3c) Apply width and height to child
            child->transform.width = newWidth;
            child->transform.height = newHeight;


            // 4a) Apply child translation, modulated by child's sheath
            int sheathCoreX, sheathCoreY;
            getSheathCoreOffset(&(dataStruct->sheathes[curIndex]), &sheathCoreX, &sheathCoreY);
            child->transform.x = translationCursorX + sheathCoreX;
            child->transform.y = translationCursorY + sheathCoreY;


            // 4b) KLUDGE HOOK Copy out old transform (before running inheritance pass)
            oldTransforms[curIndex] = child->transform;


            // Move the "translation cursor" to the top-left corner of the next cell in the row
            translationCursorX += colDims[x];


            // 5a) Finalize child's transform by inheriting from ours, placing it into our space (which is made global by our invoker)
            gTInherit(&(parcel->transform), &(child->transform));
            // 5b) Realize child parcel
            child->realizer(context, child);
        }

        // Reset "translation cursor" to first column and move down a row
        translationCursorX = walkwayWidth; // offset for self's walkway
        translationCursorY += rowDims[y];
    }


    // 6) The Gate-gazumption (and walkway realization) process
    for(int i = 0; i < signature->width * signature->height; i++){
        if(gazumperIndices[i] == -1){

            // Borders edge (or malformed)
            realizeWalkwayAndShield(map, &(parcel->children[i]), &(parcel->children[i].gates[0]), &(parcel->children[i].gates[0]));

        } else {
            struct gate gazumpGate = getGate(parcel->children[gazumperIndices[i]].gates, &(oldTransforms[gazumperIndices[i]]), gazumptionGateIndices[i]);

            if(gazumpGate.size == 0){

                // Neighbor doesn't have that gate, fail safely
                realizeWalkwayAndShield(map, &(parcel->children[i]), &(parcel->children[i].gates[0]), &(parcel->children[i].gates[0]));

            } else {

                // Transform the gate based on the sheathes of gazumper and gazumpee
                int sheathEdgeIndex = (oldTransforms[i].rotation & 1) ? 3 : 0;
                gazumpGate.position += ((dataStruct->sheathes[gazumperIndices[i]].edges[sheathEdgeIndex] == SHEATH_EDGE_NONE) ? 0:1) - 
                        ((dataStruct->sheathes[i].edges[sheathEdgeIndex] == SHEATH_EDGE_NONE) ? 0:1);


                // In order to correct the gate for gazumpee's flips, etc. we need to write the gate to the gazumpee
                // Store the original gate first
                struct gate innerGate = parcel->children[i].gates[0];
                // Then clobber
                // ...argh, another kludge for compatibility. Save on funcs/relogic though:
                int receiverGateIndex = (gazumptionGateIndices[i] + 2) % 4;

                setGate(parcel->children[i].gates, &(oldTransforms[i]), receiverGateIndex, &gazumpGate);

                
                // Finally, realize the walkway and shield of the child
                realizeWalkwayAndShield(map, &(parcel->children[i]), &innerGate, &(parcel->children[i].gates[0]));
            }
        }
    }
    // After this loop is run, every child's gate zero has been gazumped.

    
    // 7) Realize sheathes
    int cursorX = walkwayWidth;    // offset for self's walkway
    int cursorY = 0;
    for(int y = 0; y < signature->height; y++){
        for(int x = 0; x < signature->width; x++){
            
            int curIndex = (y * signature->width) + x;
            struct parcel *child = &(parcel->children[curIndex]); 

            struct sheathData *sheath = &(dataStruct->sheathes[curIndex]);
            struct gridTransform sheathTransform = newGridTransform();
            sheathTransform.x = cursorX;
            sheathTransform.y = cursorY;
            sheathTransform.width = colDims[x];
            sheathTransform.height = rowDims[y];
            gTInherit(&(parcel->transform), &sheathTransform);
            sheathTransform.z = child->transform.z; // Bad assumptions about relative heights carried over from v2

            // Another kludge deriving from our stubbornness: we have to correct the rotation of the child's gates here too.
            // Oh, well. Anything's cheaper than ECP.
            gateSet newGates;
            newGates[0] = getGate(child->gates, &(oldTransforms[curIndex]), 0);
            newGates[1] = getGate(child->gates, &(oldTransforms[curIndex]), 1);
            newGates[2] = getGate(child->gates, &(oldTransforms[curIndex]), 2);
            newGates[3] = getGate(child->gates, &(oldTransforms[curIndex]), 3);

            // Argh. It gets worse. The child's transform means that the gates have to be transformed too.
            // We fix this thus: the sheath now draws all four gates under every circumstance, and it's up to the child to ensure all four gates
            // are well-formed based on its OWN SHAPE.

            realizeSheath(map, &sheathTransform, sheath, newGates);

            // Move the cursor to the top-left corner of the next cell in the row
            cursorX += colDims[x];
        }

        // Reset cursor to first column and move down a row
        cursorX = walkwayWidth;    // offset for self's walkway
        cursorY += rowDims[y];
    }




    // 8) Generate my own residuals

    // Set walkway
    parcel->walkwayWidth = walkwayWidth; 

    // Set shield (no shield should exist in a grid recursor at all)
    parcel->shieldHeight = 0;




    // TODO FORKS (....? necessity for special handling under question)


    // In case the grid signature is malformed initialize gates to useful failsafes
    parcel->gates[0].position = 1;
    parcel->gates[0].size = 0;
    parcel->gates[1].position = 1;
    parcel->gates[1].size = 0;
    parcel->gates[2].position = 1;
    parcel->gates[2].size = 0;
    parcel->gates[3].position = 1;
    parcel->gates[3].size = 0;

    if(selfHasGate(parcel->shape, 0)){  // Probably redundant but you never know
        int sourceChildIndex = signature->gateSourceIndices[0];
        parcel->gates[0] = getGate(parcel->children[sourceChildIndex].gates, &(oldTransforms[sourceChildIndex]), 0);
        parcel->gates[0].position += oldTransforms[sourceChildIndex].y;
    } else {
        parcel->gates[0].size = 0;
    }


    if(selfHasGate(parcel->shape, 1)){
        int sourceChildIndex = signature->gateSourceIndices[1];
        parcel->gates[1] = getGate(parcel->children[sourceChildIndex].gates, &(oldTransforms[sourceChildIndex]), 1);
        parcel->gates[1].position += oldTransforms[sourceChildIndex].x;
    } else {
        parcel->gates[1].size = 0;
    }


    if(selfHasGate(parcel->shape, 2)){
        int sourceChildIndex = signature->gateSourceIndices[2];
        parcel->gates[2] = getGate(parcel->children[sourceChildIndex].gates, &(oldTransforms[sourceChildIndex]), 2);
        parcel->gates[2].position += oldTransforms[sourceChildIndex].y;
    } else {
        parcel->gates[2].size = 0;
    }


    if(selfHasGate(parcel->shape, 3)){
        int sourceChildIndex = signature->gateSourceIndices[3];
        parcel->gates[3] = getGate(parcel->children[sourceChildIndex].gates, &(oldTransforms[sourceChildIndex]), 3);
        parcel->gates[3].position += oldTransforms[sourceChildIndex].x;
    } else {
        parcel->gates[3].size = 0;
    }



    // Lastly, deallocate child memory
    for(int i = 0; i < signature->width * signature->height; i++){
        // Free child obligate lists
        if(parcel->children[i].parameters.obligatesCount > 0) free(parcel->children[i].parameters.obligates);
    }
    free(parcel->children);
    // ... and data struct
    free(dataStruct->sheathes);
    freeGridSignatureCopy(signature);
    free(parcel->data);
}

