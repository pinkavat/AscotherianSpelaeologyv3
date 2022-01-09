
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

    // 2) Compute sheath data
    dataStruct->sheathes = (void *)malloc(sizeof(struct sheathData) * signature->width * signature->height);
    // TODO compute sheath data
    for(int i = 0; i < signature->width * signature->height; i++) computeSheathData(&(dataStruct->sheathes[i]));

    
    // 3) Allocate child memory
    parcel->childCount = signature->width * signature->height;
    parcel->children = (void *)malloc(sizeof(struct parcel) * parcel->childCount);

    // 4) Set child types and parameters based on the signature
    // 5) ...and generate children in same loop
    // 6) ...and transform children in same loop
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


    // 7) Compute row/col min dimensions and flex scores
    int minColDims[signature->width];
    int minRowDims[signature->height];
    float xFlexes[signature->width];
    float yFlexes[signature->height];
    // TODO this function will need a handle for sheath data also
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









// ==================== GATE-GAZUMPTION HELPERS ====================

// There really has to be a cheaper way to do this
static struct gate gazumperHelper(struct parcel *gazumpee, struct parcel *gazumper, struct sheathData *gazumpeeSheath, struct sheathData *gazumperSheath, int gazumperGateIndex){
    // Transform gazumper's gate out of gazumper-sheath-space into grid-space
    // TODO CORRECT FOR INVERSION (if invert is true, gate is flipped)
    // TODO WE STILL NEED A VARIABLE TO ACCOUNT FOR THE ORIGINAL SHEATH ORIENTATION
    //          AAAAAAAAAAAAAAAAAAAAAAAAAA

    // Transform it out of grid-space into gazumpee-sheath-space
    // TODO

    // Flip if necessary (NOTE: BOTH SELF AND OTHER FLIP.........)
    // TODO
    struct gate newGate = gazumper->gates[gazumperGateIndex];
    printf("Returning gate %d: %d, %d\n", gazumperGateIndex, newGate.position, newGate.size);
    return newGate; // Sometimes I feel like I'm in it
}

    // TODO HUGE CONCEPTUAL FLAW: THE GAZUMPER RUNS ACCORDING TO THE CHILD'S RELATIVE ROTATION, BUT THE CHILD HAS BEEN ABSOLUTELY ROTATED BY GTINHERIT.
    // Option 1: save child rotations elsewhere (wastes space)
    // Option 2: prerun function to compute gazumped gates in childspace (This one)
    // HERE WE GO: The gate we're gazumping from; i.e. the index, tells us how the sheath affects. 

    // TODO EVEN HUGER CONCEPTUAL FLAW: GATES ARE NOT SET UNTIL REALIZATION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // SO: CHANGE THE CODE COMPLEEEETELYYYYY!! IT MUST BE DONE PRE FOR TRANSFORMS BUT POST FOR DATA!!! AAAAAAA!!!

    // TODO: THE SHEATHER IS BORKED; IT NEEDS TO KNOW THE ORIGINAL TRANSFORM OF EACH PARCEL TO PLACE THE GATES CORRECTLY!!!!!

    /* TODO notes
                The grid recursor iterates over every child parcel.
                gTInherit all the child's gates, bringing them into parentspace.
                    (do we check for shape or just blindly run gTInherit on all gates? Checking might save us a deal of work)
                If the parcel is (of type V or) of rotation 0 and on the left edge of the grid, sheathe it and move on (no other parcel is imposing its will)
                (otherwise)
                Find the parcel facing this parcel's gate zero (index translation to get the direction, then a bounds check perhaps (though this would only
                come into play in malformed parcels, no?)
                Find the gate facing this parcel in the other parcel (index translation of the other parcel)
                Hand both these gates to the walkway realizer.
                Hand the second parcel's gate to the sheather as the first parcel's gate zero (this can be where the sheather is invoked; be sure also to invoke
                it if the parcel doesn't inherit (is on the edge or is type V))

        // New notes
        For every grid child parcel:
        1) The gazumpee's gate in question is gate zero; if the parcel is V_SHAPE or faces an edge, handle in some other way*
        2) If the gazumpee faces up, gazumper's target gate is 1, etc. (gazumperGate = 3 - (gazumpeeGate + 2)%4)
        3) If the gazumpee is flipped (H if 1 or 3, V if 0 or 2), then the gazumper's gate inverts.
        4) Take the two resulting gates and feed them to the gazumpee's walkway realizer.
        5) Clobber the gazumpee's gate zero with the gazumper's gate. Now ready for sheathing pass (which could, in theory, be lumped here also)



    // Hand over to gazumpee's walkway realizer
    
    */



// ==================== REALIZER ====================

void recursorGridRealizer(void *context, struct parcel *parcel){
    /*TODO notes
    The current procedure for the grid realizer is roughly:
        1) DONE divide target dimensional increases among rows and columns of grid (flex div step)
        2) DONE Position children accordinly (translate, then inherit)
        3) DONE realize child parcels
        4) OBVIATED TRANSFORM CHILD GATES AND RESIDUAL WALKWAY/BLOCKAGES BY CHILD'S GRID TRANSFORM TO BRING THEM INTO ABSOLUTE SPACE
        5) perform gate-gazumption
        6) realize sheathes and DONE child walkway/blockages (sheathing step)

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
    int translationCursorX = 0;
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
        translationCursorX = 0;
        translationCursorY += rowDims[y];
    }


    // 6) The Gate-gazumption (and walkway realization) process
    for(int i = 0; i < signature->width * signature->height; i++){
        if(gazumperIndices[i] == -1){
            // Borders edge (or malformed)
        } else {
            /*
            struct gate outerGate = gazumperHelper(&(parcel->children[i]), &(parcel->children[gazumperIndices[i]]), 
                &(dataStruct->sheathes[i]), &(dataStruct->sheathes[gazumperIndices[i]]),
                gazumptionGateIndices[i]);
            realizeWalkwayAndShield(map, &(parcel->children[i]), &(parcel->children[i].gates[0]), &outerGate);
            */
            struct gate gazumpGate = getGate(parcel->children[gazumperIndices[i]].gates, &(oldTransforms[gazumperIndices[i]]), gazumptionGateIndices[i]);
            // In order to correct the gate for gazumpee's flips, etc. we need to write the gate to the gazumpee
            // Store the original gate first
            struct gate innerGate = parcel->children[i].gates[0];
            // Then clobber
            // ...argh, another kludge for compatibility. Save on funcs/relogic though:
            int receiverGateIndex = 3 - ((oldTransforms[i].rotation + 3) % 4);
            setGate(parcel->children[i].gates, &(oldTransforms[i]), receiverGateIndex, &gazumpGate);
            
            realizeWalkwayAndShield(map, &(parcel->children[i]), &innerGate, &(parcel->children[i].gates[0]));

        }
    }

    
    // TODO sheathing step



    // 7) Generate my own residuals

    // Set walkway (TODO don't forget to account for this in the initial min width!)
    parcel->walkwayWidth = 0; 

    // Set shield (no shield should exist in a grid recursor at all)
    parcel->shieldHeight = 0;


    // TODO gates


    // Lastly, deallocate child memory
    free(parcel->children);
    // ... and data struct
    free(dataStruct->sheathes);
    free(parcel->data);
}

