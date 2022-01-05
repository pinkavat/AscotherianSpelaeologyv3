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


int main(void){

    struct ascoTileMap *map = newAscoTileMap(10, 10);
    for(int y = 0; y < 10; y++){
        for(int x = 0; x < 10; x++){
            map->cells[(y * map->width) + x].tile = TILE_UNKNOWN;
        }
    }

    /*
    struct ascoCell testCell = {TILE_WATER, 0, 0, 0};
    struct gridTransform t = newGridTransform();
    t.width = 4;
    t.height = 3;
    t.rotation = 1;
    fillRectAuto(map, &testCell, &t, 0, 0, 4, 3, 1);
    */


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


    printAscoTileMap(map);

    //cairoRenderMap(map);

    freeAscoTileMap(map);


/*
    struct gridTransform parent = {1, 0, 0,     0, 0, 9, 4, 0};
    struct gridTransform a = {1, 0, 0,         5, 0, 4, 3, 0};
    gTInherit(&parent, &a);
    
    for(int y = 0; y < 3; y++){
        for(int x = 0; x < 4; x++){
            int m, n;
            gTCoordinate(&a, x, y, &m, &n);
            printf("(%d, %d) -> (%d, %d)\n", x, y, m, n);
        }
        printf("\n");
    }
*/

}
