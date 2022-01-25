#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // For seeding PRNG

#include "ascotherianTileMap.h"
#include "ascoGenContext.h"
#include "gridTransform.h"
#include "parcel.h"
#include "parcelGenerators.h"
#include "mapHelpers.h"
#include "walkwayAndShield.h"
#include "parcelSelector.h"
#include "postProcessing.h"

// TODO makefile (with define for cairo rendering)

// TODO split into "upsampler" (everything we've written so far) which is precompiled and invoked from here

// Cairo renderer
// Compile with -I/opt/local/include/cairo -L/opt/local/lib -lcairo -lm -std=c11 cairoRenderer/cairoRenderWithID.c
#include "cairoRenderer/cairoRenderWithID.h"


int main(int argc, char **argv){
    int seed = time(0) % 999999;    // to keep it re-typeable
    if(argc > 1) seed = atoi(argv[1]);
    fprintf(stderr, "\e[1mSeed: %d\e[0m\n", seed);
    srand(seed);




    // 0) Pattern prob table
    #define TABLE_LENGTH 3
    int tableWeights [TABLE_LENGTH][NUM_PATTERN_TYPES] = {
        //Term  Length   Void    Fork   SLedge  Pledge  Fflange Rflange  Bridge 
        {   0,      0,      0,      0,      0,      0,      0,      0,      0},
        {   0,      1,      1,      0,      0,      0,      0,      0,      0},
        {   0,      0,      1,      2,      0,      0,      0,      0,      0}
    };
    struct patternProbabilityTable probTable = {
        tableWeights,
        TABLE_LENGTH
    };



     
    // 1) Sample grid signature
   
    /* 
    enum parcelShapes shapes[4] = {L_SHAPE, L_SHAPE, L_SHAPE, L_SHAPE};
    unsigned int rotations[4] = {3, 0, 2, 1};
    //unsigned int rotations[4] = {1, 2, 0, 3};
    unsigned int flipHs[4] = {0, 0, 0, 0};
    unsigned int flipVs[4] = {0, 0, 0, 0};
    int pathGroups[4] = {1, 1, 1, 1};
    
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
        pathGroups,
        1,
        0,
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };
    */

    // 1) Another sample grid signature
    enum parcelShapes shapes[9] = {V_SHAPE, E_SHAPE, V_SHAPE, V_SHAPE, L_SHAPE, L_SHAPE, E_SHAPE, I_SHAPE, L_SHAPE};
    unsigned int rotations[9] = {0, 3, 0,   0, 2, 1,    2, 0, 2};
    unsigned int flipHs[9] =    {0, 0, 0,   0, 0, 1,    0, 0, 1};
    unsigned int flipVs[9] =    {0, 0, 0,   0, 0, 0,    0, 0, 0};
    //int pathGroups[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    enum parameterDivisionTypes divTypes[9] = {TERMINAL, TERMINAL, TERMINAL, TERMINAL, CRITICAL_PATH, CRITICAL_PATH, TERMINAL, CRITICAL_PATH, CRITICAL_PATH};
    cellPopulatorFunctionPtr popFuncs[9] = {
                                            &voidBubbleIdeator, &doorIdeator, &voidBubbleIdeator, 
                                            &voidBubbleIdeator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator,
                                            &doorIdeator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator
                                            };
    
    struct recursorGridSignature gridSig = {
        3, 3,
        shapes,
        popFuncs,
        rotations,
        flipHs,
        flipVs,
        divTypes,
        //pathGroups,
        //1,
        //0,
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };
    
    
    

    // 2) Run grid ideator on new parcel
    struct parcel jimmy;
    jimmy.shape = V_SHAPE;
    jimmy.parameters.recursionDepth = 0;
    jimmy.parameters.pathWidth = 1;
    jimmy.parameters.gateWidth = 2;
    jimmy.parameters.patternProbabilities = &probTable;

    recursorGridIdeator(&jimmy, &gridSig);
    //selectAndApplyParcelGenerator(&jimmy);
    //baseCaseIdeator(&jimmy);


    
    //printf("Ideated! %d, %d\n%f, %f\n", jimmy.minWidth, jimmy.minHeight, jimmy.flexX, jimmy.flexY);

    // 3) Set target dimensions
    jimmy.transform.width = jimmy.minWidth * 1;
    jimmy.transform.height = jimmy.minHeight * 1;
    jimmy.transform.x = 2;  // Sheath outer comp.
    jimmy.transform.y = 2;
    jimmy.transform.z = -1; // Sheath comp.


    // 4) Prep a blank map
    struct ascoTileMap *map = newAscoTileMap(gTAbsWidth(&(jimmy.transform)) + 4, gTAbsHeight(&(jimmy.transform)) + 4);
    // (set with unknowns to check, as void is hard to see)
    /*
    for(int y = 0; y < map->height; y++){
        for(int x = 0; x < map->width; x++){
            map->cells[(y * map->width) + x].tile = TILE_VOID;
        }
    }*/
    
    printf("Realizing map: %d by %d\n", map->width, map->height);

    struct ascoGenContext context = {map};

    // 5) Sheath entire map (NOT appropriate to use a parcel func to do this...)
    struct ascoCell borderCell = {TILE_CLIFF, 0, 0, 0};
    struct gridTransform t = newGridTransform();
    t.width = map->width;
    t.height = map->height;
    fillRectAuto(map, &borderCell, &t, 0, 0, map->width, map->height, 0);
    borderCell.z--;
    fillRectAuto(map, &borderCell, &t, 1, 1, map->width - 2, map->height - 2, 0); 

    // 6) Realize parcel into map
    jimmy.realizer(&context, &jimmy);

    // 7) Handle residuals
    realizeWalkwayAndShield(map, &jimmy, &(jimmy.gates[0]), &(jimmy.gates[0]));

    // TODO temp preprint
    //printAscoTileMap(map);
 
    // 8) Post-process
    tempPostProcess(map);
    
    // 9) Print/Render
    printAscoTileMap(map);
    cairoRenderMap(map);

    // 10) Clean up
    freeAscoTileMap(map);

}
