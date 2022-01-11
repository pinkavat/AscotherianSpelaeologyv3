
#include "recursorGridGenerator.h"
// recursorGridGenerator.c
// See header for details

// TODO debug
#include <stdio.h>

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
    if(x < 0 || x >= width || y < 0 || y >= width) return 0;
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


// Lookup table for below function
int gatePossessionLookup[][4] = {
    {0, 0, 0, 0},   // V_SHAPE
    {1, 0, 0, 0},   // E_SHAPE
    {1, 1, 0, 0},   // L_SHAPE
    {1, 0, 1, 0},   // I_SHAPE
    {1, 1, 1, 0},   // TL_SHAPE
    {1, 1, 1, 0},   // TI_SHAPE
    {1, 1, 1, 1},   // XL_SHAPE
    {1, 1, 1, 1},   // XI_SHAPE
};

// Helper for computing sheath data; determines if the given parcel possesses the given absolute gate, based on its shape
static int hasGate(enum parcelShapes shape, struct gridTransform *t, int index){
    // 1) Get local gate index
    int throwaway;
    int localIndex = getGateIndex(t, index, &throwaway);

    // 2) Perform lookup in above table
    return gatePossessionLookup[shape][localIndex];
}




// ==================== IDEATOR ====================

void recursorGridIdeator(struct parcel *parcel, struct recursorGridSignature *signature){

    // Shape has no meaning as yet
    // Parameters mean nothing yet

    // 1) Set the realizer and data fields
    parcel->realizer = &recursorGridRealizer;
    parcel->data = (void *)malloc(sizeof(struct recursorGridDataStruct));
    struct recursorGridDataStruct *dataStruct = (struct recursorGridDataStruct *)(parcel->data);
    // Copy over signature
    dataStruct->signature = *signature;


    // 2) Allocate child memory
    parcel->childCount = signature->width * signature->height;
    parcel->children = (void *)malloc(sizeof(struct parcel) * parcel->childCount);

    // 3) Set child types and parameters based on the signature
    // 4) ...and generate children in same loop
    // 5) ...and transform children in same loop
    for(int i = 0; i < parcel->childCount; i++){

        parcel->children[i].shape = signature->shapes[i];
        // TODO parameter division
        // TODO temp fractal deepener for debug
        parcel->children[i].parameters.recursionDepth = parcel->parameters.recursionDepth + 1;
        parcel->children[i].parameters.gateWidth = parcel->parameters.gateWidth;
        parcel->children[i].parameters.pathWidth = parcel->parameters.pathWidth;

        // Generate child
        (*(signature->populatorFunctions[i]))(&(parcel->children[i]));

        // Transform (rotate and mirror, not translate yet) child from signature
        parcel->children[i].transform.rotation = signature->rotations[i];
        parcel->children[i].transform.flipH = signature->flipHs[i];
        parcel->children[i].transform.flipV = signature->flipVs[i];

        // Set child height
        parcel->children[i].transform.z = (i % 3) - 1;   // TODO HEIGHT SELECTOR
        //parcel->children[i].transform.z = 0;   // TODO HEIGHT SELECTOR
    }


    // 6) Compute sheath data
    // This loop could probably be single, but better to be sure as the human than efficient as the computer
    dataStruct->sheathes = (void *)malloc(sizeof(struct sheathData) * signature->width * signature->height);
    for(int y = 0; y < signature->height; y++){
        for(int x = 0; x < signature->width; x++){

            int curIndex = (y * signature->width) + x;
            struct parcel *child = &(parcel->children[curIndex]);

            // TODO add L-case corrector!
            int topoAdj[4] = {1, 1, 1, 1};
            if(y > 0 && !hasGate(child->shape, &(child->transform), 3)) topoAdj[0] = 0;   // Top gate
            if(x > 0 && !hasGate(child->shape, &(child->transform), 0)) topoAdj[3] = 0;   // Left gate
            
            

            // Compute height-adj with a helper
            int heightAdj[9];
            heightAdj[8] = child->transform.z;
            heightAdjHelper(parcel->children, x, y, signature->width, signature->height, heightAdj);

            // Hand data off to the sheath solver
            computeSheathData(&(dataStruct->sheathes[curIndex]), topoAdj, heightAdj);
        }
    }


    // 7) Compute row/col min dimensions and flex scores
    int minColDims[signature->width];
    int minRowDims[signature->height];
    float xFlexes[signature->width];
    float yFlexes[signature->height];
    computeMinDimsAndFlexScores(parcel->children, dataStruct->sheathes, minColDims, minRowDims, xFlexes, yFlexes, signature->width, signature->height);

    // 8) Establish overall minimum dimensions and flex scores
    parcel->minWidth = 0;
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
struct ascoCell edgeLookup[4] = {
    {TILE_BLOCKAGE, 0, 0, 0},   // Blockage
    {TILE_UNRESOLVED, 0, 0, 0}, // Flat
    {TILE_CLIFF, 1, 2, 0},      // Up
    {TILE_CLIFF, 1, 0, -1}      // Down
};

struct ascoCell gateLookup[4] = {
    {TILE_BLANK, 0, 0, 0},      // Blockage
    {TILE_BLANK, 0, 0, 0},      // Flat
    {TILE_STAIR, 1, 2, 0},      // Up
    {TILE_STAIR, 1, 0, -1}      // Down
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
        if(self == SHEATH_CORNER_BLOCKAGE){
            // Corner is a blockage
            cornerCell.rotation = 0;
        } else {
            // Corner is concave from below
            cornerCell.tile = TILE_CLIFF;
            cornerCell.variant = 3;
            // Inherits rotation
            cornerCell.z = -1;
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
    struct ascoTileMap *map = (struct ascoTileMap *)context;
    struct recursorGridDataStruct *dataStruct = (struct recursorGridDataStruct *)(parcel->data);
    struct recursorGridSignature *signature = &(dataStruct->signature);


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

    // 3) Distribute dimensional increases across individual child parcels
    // 4) ...and translate children while we have a loop open for old time's sake
    int translationCursorX = 0; // TODO self's walkway
    int translationCursorY = 0;
    // 5) ...and realize the child parcels as well, come to think of it
    for(int y = 0; y < signature->height; y++){
        for(int x = 0; x < signature->width; x++){

            int curIndex = (y * signature->width) + x;
            struct parcel *child = &(parcel->children[curIndex]);

            // Copy out old transform
            oldTransforms[curIndex] = child->transform;

            // *Part 6 sneaks in here* gazumption needs a precompute step to account for rotation inheritance
            // If the child has no gate zero, no gazumption.
            if(child->shape != V_SHAPE){
                // Otherwise, store its neighbor's index and appropriate gate for gazumption
                switch(child->transform.rotation){
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

            // 3d) Apply width and height to old Transform as well
            oldTransforms[curIndex].width = newWidth;
            oldTransforms[curIndex].height = newHeight;



            // 4a) Apply child translation, modulated by child's sheath
            int sheathCoreX, sheathCoreY;
            getSheathCoreOffset(&(dataStruct->sheathes[curIndex]), &sheathCoreX, &sheathCoreY);
            child->transform.x = translationCursorX + sheathCoreX;
            child->transform.y = translationCursorY + sheathCoreY;

            // Move the "translation cursor" to the top-left corner of the next cell in the row
            translationCursorX += colDims[x];


            // 5a) Finalize child's transform by inheriting from ours, placing it into our space (which is made global by our invoker)
            gTInherit(&(parcel->transform), &(child->transform));
            // 5b) Realize child parcel
            child->realizer(context, child);
        }

        // Reset "translation cursor" to first column and move down a row
        translationCursorX = 0; // TODO self's walkway
        translationCursorY += rowDims[y];
    }


    // 6) The Gate-gazumption (and walkway realization) process
    for(int i = 0; i < signature->width * signature->height; i++){
        if(gazumperIndices[i] == -1){

            // Borders edge (or malformed)
            realizeWalkwayAndShield(map, &(parcel->children[i]), &(parcel->children[i].gates[0]), &(parcel->children[i].gates[0]));

        } else {
            struct gate gazumpGate = getGate(parcel->children[gazumperIndices[i]].gates, &(oldTransforms[gazumperIndices[i]]), gazumptionGateIndices[i]);

            // Transform the gate based on the sheathes of gazumper and gazumpee
            int sheathEdgeIndex = (oldTransforms[i].rotation & 1) ? 0 : 3;
            gazumpGate.position += ((dataStruct->sheathes[gazumperIndices[i]].edges[sheathEdgeIndex] == SHEATH_EDGE_NONE) ? 0:1) - 
                    ((dataStruct->sheathes[i].edges[sheathEdgeIndex] == SHEATH_EDGE_NONE) ? 0:1);


            // In order to correct the gate for gazumpee's flips, etc. we need to write the gate to the gazumpee
            // Store the original gate first
            struct gate innerGate = parcel->children[i].gates[0];
            // Then clobber
            // ...argh, another kludge for compatibility. Save on funcs/relogic though:
            int receiverGateIndex = 3 - ((oldTransforms[i].rotation + 3) % 4);
            setGate(parcel->children[i].gates, &(oldTransforms[i]), receiverGateIndex, &gazumpGate);
            
            // Finally, realize the walkway and shield of the child
            realizeWalkwayAndShield(map, &(parcel->children[i]), &innerGate, &(parcel->children[i].gates[0]));
        }
    }
    // After this loop is run, every child's gate zero has been gazumped.

    
    // 7) Realize sheathes
    int cursorX = 0;    // TODO self's walkway
    int cursorY = 0;
    for(int y = 0; y < signature->height; y++){
        for(int x = 0; x < signature->width; x++){
            
            int curIndex = (y * signature->width) + x;
            struct parcel *child = &(parcel->children[curIndex]); 

            struct sheathData *sheath = &(dataStruct->sheathes[curIndex]);
            struct gridTransform sheathTransform = newGridTransform();
            sheathTransform.x = cursorX;
            sheathTransform.y = cursorY;
            sheathTransform.z = child->transform.z;
            sheathTransform.width = colDims[x];
            sheathTransform.height = rowDims[y];
            gTInherit(&(parcel->transform), &sheathTransform);

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
        cursorX = 0;    // TODO self's walkway
        cursorY += rowDims[y];
    }




    // 8) Generate my own residuals

    // Set walkway (TODO don't forget to account for this in the initial min width and cursor init and reset conds)
    parcel->walkwayWidth = 0; 

    // Set shield (no shield should exist in a grid recursor at all)
    parcel->shieldHeight = 0;




    // TODO the below are affected by fork logic if present....?
    // TODO gates from pattern (remember to make all gates well-formed!)
    // TODO gate width param
    parcel->gates[0].position = parcel->transform.height - 3;
    parcel->gates[0].size = 0;

    // Note: up above we have a helper called hasGate; but now, thanks to our assumptions about well-formed children,
    // the scan can simply rely on whether the received gate has size zero or not (no double fetch and no lookup)

    if(parcel->shape == L_SHAPE || parcel->shape == TL_SHAPE || parcel->shape > TI_SHAPE){
        // Scan bottom children for exit 1 and gazump it
        int offset = 0;
        for(int i = 0; i < signature->width; i++){
            int curIndex = ((signature->height - 1) * signature->width) + i;
            struct parcel *child = &(parcel->children[curIndex]);
            struct gate g = getGate(child->gates, &(oldTransforms[curIndex]), 1);
            if(g.size > 0){
                // Found a gate, gazump
                g.position += offset + (dataStruct->sheathes[curIndex].edges[3] == SHEATH_EDGE_NONE ? 0 : 1);
                parcel->gates[1] = g;
                break;  // Look no further
            }
            offset += colDims[i];
        }
    } else {
        parcel->gates[1].size = 0;
    }

    
    // ...juuust too much spec code to be worth refactoring
    if(parcel->shape == I_SHAPE || parcel->shape >= TI_SHAPE){
        // Scan right for exit 2 and gazump it
        int offset = 0;
        for(int i = 0; i < signature->height; i++){
            int curIndex = (i * signature->width) + (signature->height - 1);
            struct parcel *child = &(parcel->children[curIndex]);
            struct gate g = getGate(child->gates, &(oldTransforms[curIndex]), 2);
            if(g.size > 0){
                // Found a gate, gazump
                g.position += offset + (dataStruct->sheathes[curIndex].edges[0] == SHEATH_EDGE_NONE ? 0 : 1);
                parcel->gates[2] = g;
                break;  // Look no further
            }
            offset += rowDims[i];
        }
    } else {
        parcel->gates[2].size = 0;
    }


    if(parcel->shape > TI_SHAPE){
        // Scan top for exit 3 and gazump it
        int offset = 0;
        for(int i = 0; i < signature->width; i++){
            int curIndex = i;
            struct parcel *child = &(parcel->children[curIndex]);
            struct gate g = getGate(child->gates, &(oldTransforms[curIndex]), 3);
            if(g.size > 0){
                // Found a gate, gazump
                g.position += offset + (dataStruct->sheathes[curIndex].edges[3] == SHEATH_EDGE_NONE ? 0 : 1);
                parcel->gates[3] = g;
                break;  // Look no further
            }
            offset += colDims[i];
        }
    } else {
        parcel->gates[3].size = 0;
    }
    


    // Lastly, deallocate child memory
    free(parcel->children);
    // ... and data struct
    free(dataStruct->sheathes);
    free(parcel->data);
}

