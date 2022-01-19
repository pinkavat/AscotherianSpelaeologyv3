
#include "sheath.h"
// sheath.c
// See header for details


void computeSheathData(struct sheathData *sheath, int topoAdj[4], int heightAdj[9]){
    // Lifted wholesale from v2, pretty much. It works.
  
    unsigned char canCullEdge[4] = {1, 1, 1, 1};    // Initially, it's possible to cull all edges
    
    // Start all corners marked with blockage and all edges marked with flat
    for(int i = 0; i < 4; i++){
        sheath->corners[i] = SHEATH_CORNER_BLOCKAGE;
        sheath->edges[i] = SHEATH_EDGE_FLAT;
    }

    // 1) Set edges
    for(int i = 0; i < 4; i++){
        if(heightAdj[(2 * i) + 1] != heightAdj[8]){
            if(heightAdj[8] != 0) { // Only set height-change borders in non-zero-height sheathes
                sheath->edges[i] = (heightAdj[(2 * i) + 1] > heightAdj[8]) ? SHEATH_EDGE_UP : SHEATH_EDGE_DOWN;
                canCullEdge[i] = 0;
            }
        } else {
            // Same height; check for topological adjacency
            // Both zero-height and non-zero-height sheathes can set blockage borders
            if(!topoAdj[i]){
                sheath->edges[i] = SHEATH_EDGE_BLOCKAGE;
                canCullEdge[i] = 0;
            }
        }
    }

    // 2) Set corners (iff nonzero height)
    if(heightAdj[8] != 0){
        // Check corner heights; any corner not of the same height is marked for height-change and causes its
        //    two adjacent edges to be uncullable

        // Juuuust enough modulo footling to avoid putting this all in a loop (compiler'd unroll it anyway)
        if(heightAdj[0] != heightAdj[8] && heightAdj[7] == heightAdj[1]){
            sheath->corners[0] = (heightAdj[8] < 0) ? SHEATH_CORNER_UP : SHEATH_CORNER_DOWN;
            canCullEdge[3] = 0;
            canCullEdge[0] = 0;
        }
        if(heightAdj[2] != heightAdj[8] && heightAdj[1] == heightAdj[3]){
            sheath->corners[1] = (heightAdj[8] < 0) ? SHEATH_CORNER_UP : SHEATH_CORNER_DOWN;
            canCullEdge[0] = 0;
            canCullEdge[1] = 0;
        }
        if(heightAdj[4] != heightAdj[8] && heightAdj[3] == heightAdj[5]){
            sheath->corners[2] = (heightAdj[8] < 0) ? SHEATH_CORNER_UP : SHEATH_CORNER_DOWN;
            canCullEdge[1] = 0;
            canCullEdge[2] = 0;
        }
        if(heightAdj[6] != heightAdj[8] && heightAdj[5] == heightAdj[7]){
            sheath->corners[3] = (heightAdj[8] < 0) ? SHEATH_CORNER_UP : SHEATH_CORNER_DOWN;
            canCullEdge[2] = 0;
            canCullEdge[3] = 0;
        }
    }

    // 3) Cull any edges that can be culled
    for(int i = 0; i < 4; i++){
        if(canCullEdge[i]) sheath->edges[i] = SHEATH_EDGE_NONE;
    }
}



// Lifted wholesale from AscoSpel v2, minus the "landing strip" (which is still the responsibility of the individual parcels)
void getSheathMinSize(struct sheathData *sheath, int *xPtr, int *yPtr){
    *xPtr = ((sheath->edges[1] == SHEATH_EDGE_NONE) ? 0 : 1) + ((sheath->edges[3] == SHEATH_EDGE_NONE) ? 0 : 1); 
    *yPtr = ((sheath->edges[0] == SHEATH_EDGE_NONE) ? 0 : 1) + ((sheath->edges[2] == SHEATH_EDGE_NONE) ? 0 : 1);
}

void getSheathCoreOffset(struct sheathData *sheath, int *xPtr, int *yPtr){
    *xPtr = (sheath->edges[3] == SHEATH_EDGE_NONE) ? 0 : 1;
    *yPtr = (sheath->edges[0] == SHEATH_EDGE_NONE) ? 0 : 1;
}




