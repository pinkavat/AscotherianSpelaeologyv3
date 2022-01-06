
#include "recursorGridGenerator.h"
// recursorGridGenerator.c
// See header for details


// ==================== HELPERS ====================

// TODO strategy one for flex score amalgamation: INDIVIDUAL row/col score is AVERAGE of all subscores.
// Helper for both ideator and realizer; computes row/col min dimensions and flex scores of children
static void computeMinDimsAndFlexScores(struct parcel *children, int *minColDims, int *minRowDims, float *xFlexes, float *yFlexes, int width, int height){

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
            if(parcelAbsMinWidth(child) > minColDims[x]) minColDims[x] = parcelAbsMinWidth(child);
            if(parcelAbsMinHeight(child) > minRowDims[y]) minRowDims[y] = parcelAbsMinHeight(child);

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
        int addedIncrease = ((float)increase * (flexes[i] / sumFlexes));
        dimensions[i] += addedIncrease;
        // ...and subtract it from the remainder
        remainder -= addedIncrease;
    }
    
    // 3) In the event that the increase doesn't divide perfectly, the first cell eats the remainder
    dimensions[0] += remainder;
}




// ==================== IDEATOR ====================

void recursorGridIdeator(struct parcel *parcel, struct recursorGridSignature *signature){
    /*TODO notes
        The current procedure for the grid ideator is roughly:
        1) Choose height offsets (height choice step), and compute sheath data and minimum sheath sizes and store in data struct (sheathing step)
        2) Based on the pattern, divide the metrics. (metric div step)
        3) DONE Based on the pattern, generate children.
        4) DONE Based on the pattern, rotate and mirror children.
        5) DONE Compute and store per-row/per-col minimum dimensions in data struct
        6) DONE Compute and store per-row/per-col flex scores in data struct
        7) DONE Compute overall flex scores for whole parcel (unclear flex amalgam step)
    */

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
        // TODO parameter division
        parcel->children[i].shape = signature->shapes[i];
        // TODO temp fractal deepener for debug
        parcel->children[i].parameters.recursionDepth = parcel->parameters.recursionDepth + 1;

        // Generate child
        (*(signature->populatorFunctions[i]))(&(parcel->children[i]));

        // Transform (rotate and mirror, not translate yet) child from signature
        parcel->children[i].transform.rotation = signature->rotations[i];
        parcel->children[i].transform.flipH = signature->flipHs[i];
        parcel->children[i].transform.flipV = signature->flipVs[i];
    }


    // 6) Compute row/col min dimensions and flex scores
    int minColDims[signature->width];
    int minRowDims[signature->height];
    float xFlexes[signature->width];
    float yFlexes[signature->height];
    // TODO this function will need a handle for sheath data also
    computeMinDimsAndFlexScores(parcel->children, minColDims, minRowDims, xFlexes, yFlexes, signature->width, signature->height);

    // 7) Establish overall minimum dimensions and flex scores
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









// ==================== REALIZER ====================

void recursorGridRealizer(void *context, struct parcel *parcel){
    /*TODO notes
    The current procedure for the grid realizer is roughly:
        1) DONE divide target dimensional increases among rows and columns of grid (flex div step)
        2) DONE Position children accordinly (translate, then inherit)
        3) DONE realize child parcels
        4) TRANSFORM CHILD GATES AND RESIDUAL WALKWAY/BLOCKAGES BY CHILD'S GRID TRANSFORM TO BRING THEM INTO ABSOLUTE SPACE
        5) perform gate-gazumption
        6) realize sheathes and child walkway/blockages (sheathing step)

    */

    // Cast context
    struct ascoTileMap *map = (struct ascoTileMap *)context;
    struct recursorGridDataStruct *dataStruct = (struct recursorGridDataStruct *)(parcel->data);
    struct recursorGridSignature *signature = &(dataStruct->signature);


    // 1) Recompute row/col dimensions and flex scores
    int colDims[signature->width];
    int rowDims[signature->height];
    float xFlexes[signature->width];
    float yFlexes[signature->height];
    // TODO this function will need a handle for sheath data also
    computeMinDimsAndFlexScores(parcel->children, colDims, rowDims, xFlexes, yFlexes, signature->width, signature->height);


    // 2) Distribute dimensional increases across rows and columns
    distributeDimensionalIncreases(parcel->transform.width - parcel->minWidth, colDims, xFlexes, signature->width);     // x
    distributeDimensionalIncreases(parcel->transform.height - parcel->minHeight, rowDims, yFlexes, signature->height);  // y


    // 3) Distribute dimensional increases across individual child parcels
    // 4) ...and translate children while we have a loop open for old time's sake
    int translationCursorX = 0;
    int translationCursorY = 0;
    // 5) ...and realize the child parcels as well, come to think of it
    // 6) ...and why not round it off by realizing the child's walkway and shield here too
    for(int y = 0; y < signature->height; y++){
        for(int x = 0; x < signature->width; x++){

            struct parcel *child = &(parcel->children[(y * signature->width) + x]);

            // 3a) Fetch new width and height
            int newWidth = colDims[x];
            int newHeight = rowDims[y];

            // 3b) Transform to match child
            if(child->transform.rotation & 1){
                int temp = newWidth;
                newWidth = newHeight;
                newHeight = temp;
            }

            // 3c) Apply width and height to child
            child->transform.width = newWidth;
            child->transform.height = newHeight;



            // 4a) Apply child translation
            // TODO note! this is where the child's sheath affects its offset
            child->transform.x = translationCursorX;
            child->transform.y = translationCursorY;

            // Move the "translation cursor" to the top-left corner of the next cell in the row
            translationCursorX += colDims[x];


            // 5a) Finalize child's transform by inheriting from ours, placing it into our space (which is made global by our invoker)
            gTInherit(&(parcel->transform), &(child->transform));
            // 5b) Realize child parcel
            child->realizer(context, child);

            // 6a) Handle child's walkway and shield
            gTInherit(&(child->transform), &(child->walkway));
            gTInherit(&(child->transform), &(child->shield));
            // TODO gate params
            realizeWalkwayAndShield(map, &(child->walkway), &(child->shield), &(child->walkway), &(child->walkway));

        }

        // Reset "translation cursor" to first column and move down a row
        translationCursorX = 0;
        translationCursorY += rowDims[y];
    }


    // 7) Generate my own residuals

    // Set walkway (TODO)
    parcel->walkway = newGridTransform();
    parcel->walkway.width = 0;
    parcel->walkway.height = 0;

    // Set shield (no shield should exist in a grid recursor at all)
    parcel->shield = newGridTransform();

    // TODO gates



    // Lastly, deallocate child memory
    free(parcel->children);
    // ... and data struct
    free(parcel->data);
}

