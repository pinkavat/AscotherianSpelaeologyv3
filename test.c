#include <stdio.h>
#include <stdlib.h>

#include "ascotherianTileMap.h"
#include "gridTransform.h"
#include "parcel.h"
#include "parcelGenerators.h"
#include "mapHelpers.h"
#include "parcelSelector.h"

// TODO makefile

// Cairo renderer
// Compile with -I/opt/local/include/cairo -L/opt/local/lib -lcairo -lm -std=c11 cairoRenderer/cairoRenderWithID.c
//#include "cairoRenderer/cairoRenderWithID.h"

void bork(struct parcel *a){
    printf("Callback from type %d\n", a->shape);
}


int main(void){

    // 1) Sample grid signature
    enum parcelShapes shapes[4] = {L_SHAPE, L_SHAPE, L_SHAPE, L_SHAPE};
    unsigned int rotations[4] = {0, 1, 3, 2};
    unsigned int flipHs[4] = {0, 0, 0, 0};
    unsigned int flipVs[4] = {0, 0, 0, 0};
    
    //cellPopulatorFunctionPtr popFuncs[4] = {&selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator, &selectAndApplyParcelGenerator};
    cellPopulatorFunctionPtr popFuncs[4] = {&selectAndApplyParcelGenerator, &baseCaseIdeator, &baseCaseIdeator, &selectAndApplyParcelGenerator};
    //cellPopulatorFunctionPtr popFuncs[4] = {&bork, &bork, &bork, &bork};

    struct recursorGridSignature gridSig = {
        2, 2,
        shapes,
        popFuncs,
        rotations,
        flipHs,
        flipVs
    };


    // 2) Run grid ideator on new parcel
    struct parcel jimmy;
    jimmy.shape = I_SHAPE;

    recursorGridIdeator(&jimmy, &gridSig);
    //selectAndApplyParcelGenerator(&jimmy);

    printf("%d, %d\n%f, %f\n", jimmy.minWidth, jimmy.minHeight, jimmy.flexX, jimmy.flexY);
/*

    // 3) Set target dimensions
    jimmy.transform.width = 30;
    jimmy.transform.height = 20;

    // 4) Prep a blank map
    struct ascoTileMap *map = newAscoTileMap(gTAbsWidth(&(jimmy.transform)), gTAbsHeight(&(jimmy.transform)));
    // (set with unknowns to check, as void is hard to see)
    for(int y = 0; y < map->height; y++){
        for(int x = 0; x < map->width; x++){
            map->cells[(y * map->width) + x].tile = TILE_UNKNOWN;
        }
    }

    // 5) Realize parcel into map
    jimmy.realizer(map, &jimmy);

    // 6) Handle residuals
    gTInherit(&(jimmy.transform), &(jimmy.walkway));
    gTInherit(&(jimmy.transform), &(jimmy.shield));
    realizeWalkwayAndShield(map, &(jimmy.walkway), &(jimmy.shield), &(jimmy.walkway), &(jimmy.walkway));

    
    // 7) Render
    printAscoTileMap(map);
    //cairoRenderMap(map);
    freeAscoTileMap(map);
*/

    /*
    struct ascoTileMap *map = newAscoTileMap(10, 10);
    for(int y = 0; y < 10; y++){
        for(int x = 0; x < 10; x++){
            map->cells[(y * map->width) + x].tile = TILE_UNKNOWN;
        }
    }

    // Make a mapwide parent for jerry
    struct gridTransform parent = newGridTransform();
    parent.width = 10;
    parent.height = 10;

    // Make jerry
    struct parcel jerry;
    jerry.shape = I_SHAPE;

    selectAndApplyParcelGenerator(&jerry);



    // Set transform of jerry
    jerry.transform = newGridTransform();
    jerry.transform.width = 7;
    jerry.transform.height = 8;
    jerry.transform.x = 1;
    jerry.transform.y = 1;

    // Jerry inherits from parent
    gTInherit(&parent, &(jerry.transform));

    // Realize jerry
    jerry.realizer(map, &jerry);

    // Handle jerry's residuals
    // Inherit from jerry
    gTInherit(&(jerry.transform), &(jerry.walkway));
    gTInherit(&(jerry.transform), &(jerry.shield));
    realizeWalkwayAndShield(map, &(jerry.walkway), &(jerry.shield), &(jerry.walkway), &(jerry.walkway));
    */

}
