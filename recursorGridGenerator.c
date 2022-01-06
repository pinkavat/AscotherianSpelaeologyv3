
#include "recursorGridGenerator.h"
// recursorGridGenerator.c
// See header for details


// TODO static memory managers for the data struct


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
    //parcel->data = new

    // 2) Allocate child memory
    parcel->childCount = signature->width * signature->height;
    parcel->children = (void *)malloc(sizeof(struct parcel) * parcel->childCount);

    // 3) Set child types and parameters based on the signature
    // 4) ...and generate children in same loop
    // 5) ...and transform children in same loop
    for(int i = 0; i < parcel->childCount; i++){
        // TODO parameter division
        parcel->children[i].shape = signature->shapes[i];

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


void recursorGridRealizer(void *context, struct parcel *parcel){
    /*TODO notes
    The current procedure for the grid realizer is roughly:
        1) divide target dimensional increases among rows and columns of grid (flex div step)
        2) Position children accordinly (translate, then inherit)
        3) realize child parcels
        4) TRANSFORM CHILD GATES AND RESIDUAL WALKWAY/BLOCKAGES BY CHILD'S GRID TRANSFORM TO BRING THEM INTO ABSOLUTE SPACE
        5) perform gate-gazumption
        6) realize sheathes and child walkway/blockages (sheathing step)

    */

    // TODO deallocate child memory
}

