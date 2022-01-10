#ifndef SHEATH
#define SHEATH

/* sheath.h
*
*   Code for computing "sheath data"; the description of what surrounds a parcel in tilespace in order to ensure that its height and topological
*   adjacencies are appropriately buffered.
*
*   Topological adjacency is represented as four booleans, from the top clockwise, describing whether the adjacent region in that direction is to be
*       topologically connected or not.
*   Height adjacency is represented as nine integers, clockwise from top-left, with the self's height at the end (element 8)
*       containing the heights of the associated regions relative to their parent region (so we can plug-and-play from a recursor sans correction)
*
*   The sheathing process is conceptually simple:
*       If adjacent parcels differ in height from the self, the sheath adds a "buffer zone" so that a height-changing tile (AscoSpel's cliffs, for instance)
*       can be placed to allow transition.
*       If the adjacent parcels are of the same height but not topologically adjacent, a blockage is placed to keep the two regions disconnected.
*       If the parcel is both adjacent and of the same height, the edge is flat floor.
*       If there is no need for an edge, it is pruned (SHEATH_EDGE_NONE); that is, when drawn, there is no tile gap.
*
*   In AscoSpel v2, sheathes were their own parcels, to take advantage of parcel-resident transform logic. In v3, due to flexibility assumptions,
*   Sheathes are now handled by the grid recursor, and operate using the recursor's transform logic. This is more conceptually complex, probably
*   even less efficient than v2, but never mind -- it allows us to write parcel generators that handle their own resizing perfectly, without needing
*   to burden the sheath or corrupt the pure recursive structure.
*
* written January 2022
*/


enum sheathEdgeTypes {
    // Enum ordering strictly enforced because the realizer uses a lookup table to decide corners
    SHEATH_EDGE_BLOCKAGE = 0,   // The edge this side is flat, but not topologically connected, so it must be blocked.
    SHEATH_EDGE_FLAT = 1,       // The edge this side is flat, unobstructed floor
    SHEATH_EDGE_UP = 2,         // The edge this side leads up to a higher adjacent area
    SHEATH_EDGE_DOWN = 3,       // The edge this side leads down to a lower adjacent area
    SHEATH_EDGE_NONE = 4        // No edge this side at all
};

enum sheathCornerTypes{
    SHEATH_CORNER_BLOCKAGE, // As above, but note that corners are either blocked or height-changing.
    SHEATH_CORNER_CHANGE    // Improvement from v2: corners are either blocked, pruned, or matching their adjacent edges
                            // and if there are no edges to match, it MUST be concave down, since zero-height parcels are definitionally unsheathed.
};


struct sheathData {
    enum sheathEdgeTypes edges[4];                // Clockwise from top
    enum sheathCornerTypes corners[4];            // Clockwise from top-left
};


// Computes the sheath data for a region with the given height and topological adjacencies (see top)
void computeSheathData(struct sheathData *sheath, int topoAdj[4], int heightAdj[9]);

// Computes the extra size in tiles that a sheath adds to the thing it is sheathing
void getSheathMinSize(struct sheathData *sheath, int *xPtr, int *yPtr);

// Computes the offset of the thing the sheath is sheathing, based on the thickness of the sheath's edges
void getSheathCoreOffset(struct sheathData *sheath, int *xPtr, int *yPtr);


#endif
