
#include "sheath.h"
// sheath.c
// See header for details

/*
enum sheathEdgeTypes {
    SHEATH_EDGE_NONE,       // No edge this side at all
    SHEATH_EDGE_FLAT,       // The edge this side is flat, unobstructed floor
    SHEATH_EDGE_BLOCKAGE,   // The edge this side is flat, but not topologically connected, so it must be blocked.
    SHEATH_EDGE_UP,         // The edge this side leads up to a higher adjacent area
    SHEATH_EDGE_DOWN        // The edge this side leads down to a lower adjacent area
};

enum sheathCornerTypes{
    SHEATH_CORNER_BLOCKAGE, // As above, but note that corners are either blocked or height-changing.
    SHEATH_CORNER_UP,
    SHEATH_CORNER_DOWN
};


struct sheathData {
    enum sheathEdgeTypes edges[4];                // Clockwise from top
    enum sheathCornerTypes corners[4];            // Clockwise from top-left
};

*/

void computeSheathData(struct sheathData *sheath/*TODO params*/){
    // TODO placeholder code
  
    /* 
    sheath->edges[0] = SHEATH_EDGE_BLOCKAGE;
    sheath->edges[1] = SHEATH_EDGE_BLOCKAGE;
    sheath->edges[2] = SHEATH_EDGE_BLOCKAGE;
    sheath->edges[3] = SHEATH_EDGE_BLOCKAGE;
    sheath->corners[0] = SHEATH_CORNER_BLOCKAGE;
    sheath->corners[1] = SHEATH_CORNER_BLOCKAGE;
    sheath->corners[2] = SHEATH_CORNER_BLOCKAGE;
    sheath->corners[3] = SHEATH_CORNER_BLOCKAGE;
    */
    sheath->edges[0] = SHEATH_EDGE_NONE;
    sheath->edges[1] = SHEATH_EDGE_NONE;
    sheath->edges[2] = SHEATH_EDGE_NONE;
    sheath->edges[3] = SHEATH_EDGE_NONE;
    
    // TODO old v2 code for reference
    /*
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
        // TODO WAIT; this logic is wrong. Consider flat corners, they place no burden on their edges.
        // Maybe this: if the corner's height is the same as either one but not both of its edge neighbors, it's ok to cull.
        // FUTURE OPTIMIZATION; for now, we can fail to cull these and waste a bit of space.
        for(int i = 0; i < 4; i++){
            if(heightAdj[2 * i] != heightAdj[8]){
                sheath->corners[i] = (heightAdj[2 * i] > heightAdj[8]) ? SHEATH_CORNER_UP : SHEATH_CORNER_DOWN;
                canCullEdge[i == 0 ? 3 : (i - 1)] = 0;
                canCullEdge[i] = 0;
            }
        }
    }

    // 3) Cull any edges that can be culled
    for(int i = 0; i < 4; i++){
        if(canCullEdge[i]) sheath->edges[i] = SHEATH_EDGE_NONE;
    }

    */
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




