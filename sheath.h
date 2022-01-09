#ifndef SHEATH
#define SHEATH

/* sheath.h
*
*   Code for computing "sheath data"; the description of what surrounds a parcel in tilespace in order to ensure that its height and topological
*   adjacencies are appropriately buffered.
*
*   In AscoSpel v2, sheathes were their own parcels, to take advantage of parcel-resident transform logic. In v3, due to flexibility assumptions,
*   Sheathes are now handled by the grid recursor, and operate using the recursor's transform logic. This is more conceptually complex, probably
*   even less efficient than v2, but never mind -- it allows us to write parcel generators that handle their own resizing perfectly, without needing
*   to burden the sheath or corrupt the pure recursive structure.
*
* written January 2022
*/


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


// TODO document
void computeSheathData(struct sheathData *sheath/*TODO params*/);

// Computes the extra size in tiles that a sheath adds to the thing it is sheathing
void getSheathMinSize(struct sheathData *sheath, int *xPtr, int *yPtr);

// Computes the offset of the thing the sheath is sheathing, based on the thickness of the sheath's edges
void getSheathCoreOffset(struct sheathData *sheath, int *xPtr, int *yPtr);


#endif
