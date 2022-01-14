#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // For seeding PRNG

#include "ascotherianTileMap.h"
#include "gridTransform.h"
#include "parcel.h"
#include "parcelGenerators.h"
#include "mapHelpers.h"
#include "walkwayAndShield.h"
#include "parcelSelector.h"

// TODO makefile (with define for cairo rendering)

// TODO split into "upsampler" (everything we've written so far) which is precompiled and invoked from here

// Cairo renderer
// Compile with -I/opt/local/include/cairo -L/opt/local/lib -lcairo -lm -std=c11 cairoRenderer/cairoRenderWithID.c
// #include "cairoRenderer/cairoRenderWithID.h"


int main(int argc, char **argv){
    int seed = time(0) % 999999;    // to keep it re-typeable
    if(argc > 1) seed = atoi(argv[1]);
    fprintf(stderr, "\e[1mSeed: %d\e[0m\n", seed);
    srand(seed);

     
    // 1) Sample grid signature
    
    enum parcelShapes shapes[4] = {L_SHAPE, L_SHAPE, L_SHAPE, L_SHAPE};
    unsigned int rotations[4] = {3, 0, 2, 1};
    //unsigned int rotations[4] = {1, 2, 0, 3};
    unsigned int flipHs[4] = {0, 0, 0, 0};
    unsigned int flipVs[4] = {0, 0, 0, 0};
    int gateSourceIndices[4] = {0, 0, 0, 0};
    int gateIsFork[4] = {0, 0, 0, 0};
    
    //cellPopulatorFunctionPtr popFuncs[4] = {&selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator};
    //cellPopulatorFunctionPtr popFuncs[4] = {&baseCaseIdeator, &baseCaseIdeator, &baseCaseIdeator, &baseCaseIdeator};
    cellPopulatorFunctionPtr popFuncs[4] = {&baseCaseIdeator, &selectAndApplyParcelGenerator, &baseCaseIdeator, &selectAndApplyParcelGenerator};
    
    struct recursorGridSignature gridSig = {
        2, 2,
        shapes,
        popFuncs,
        rotations,
        flipHs,
        flipVs,
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };
    
    

    // 2) Run grid ideator on new parcel
    struct parcel jimmy;
    jimmy.shape = L_SHAPE;
    jimmy.parameters.recursionDepth = 0;
    jimmy.parameters.pathWidth = 1;
    jimmy.parameters.gateWidth = 2;

    recursorGridIdeator(&jimmy, &gridSig);
    //selectAndApplyParcelGenerator(&jimmy);
    //baseCaseIdeator(&jimmy);

    //printf("Ideated! %d, %d\n%f, %f\n", jimmy.minWidth, jimmy.minHeight, jimmy.flexX, jimmy.flexY);


    // 3) Set target dimensions
    jimmy.transform.width = jimmy.minWidth * 2;
    jimmy.transform.height = jimmy.minHeight * 2;

    // 4) Prep a blank map
    struct ascoTileMap *map = newAscoTileMap(gTAbsWidth(&(jimmy.transform)), gTAbsHeight(&(jimmy.transform)));
    // (set with unknowns to check, as void is hard to see)
    for(int y = 0; y < map->height; y++){
        for(int x = 0; x < map->width; x++){
            map->cells[(y * map->width) + x].tile = TILE_VOID;
        }
    }

    // 5) Realize parcel into map
    jimmy.realizer(map, &jimmy);

    // 6) Handle residuals
    realizeWalkwayAndShield(map, &jimmy, &(jimmy.gates[0]), &(jimmy.gates[0]));

    
    // 7) Render
    printAscoTileMap(map);
    //cairoRenderMap(map);
    freeAscoTileMap(map);

}
