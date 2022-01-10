#ifndef RECT_GATE
#define RECT_GATE

#include "gridTransform.h"

/* rectGate.h
*
*   Functionality for manipulating four "gates", one on each edge of a transformed rectangular grid (see "gridTransform.h")
*
*   The gates are laid out thus in a "gate array", for an unrotated, unflipped grid.
*   Counterclockwise from left is not similar to the rotation data used in the grid transform (clockwise from top). This is 
*   a conceptual muddle that persists because I'm too dense to reason about gates in any orientation but this one (it seems, by experiment)
*   and I'll be damned if I have to rebuild the grid transform logic (can't even remember how it works)
*
*           |--3--|
*        ---#######v###---
*         | ########### |
*         0 ########### 2
*         | ########### |
*        ---###########---
*           >#########<
*           #######^###
*           |--1--|
*
*   Created for AscoSpel v3, almost entirely from code used in AscoSpel v2, as an adjunct for parcel.h
*
* written January 2022
*/


struct gate {
    int position;
    int size;
};


typedef struct gate gateSet[4];     // Counterclockwise from left, as above


// Transforms a gate index; returns the ACTUAL gate (from the transform's POV) that appears to be the CANONICAL gate (from the outside world's POV)
// along with whether said gate is flipped from what we expect.
// Written as a helper for getGate and setGate; only exposed because we can kitbash this transform logic to help the topo-adj computer in recursor grid.
int getGateIndex(struct gridTransform *t, int index, int *invertPtr);

// Return the gate of the index specified (as seen from without), selecting the actual gate (as seen from within) to return based on the transform
struct gate getGate(gateSet set, struct gridTransform *t, int index);

// Ditto, but setting the gate of the specified index
void setGate(gateSet set, struct gridTransform *t, int index, struct gate *newGate);

#endif
