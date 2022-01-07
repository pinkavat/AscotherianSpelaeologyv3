
#include "rectGate.h"
// rectGate.c
// See header for details


// Helper for the below functions; computes the new index and whether the coordinate has to be inverted
// TODO this is lifted wholesale from v2; there must be a better way.
static int getGateIndex(struct gridTransform *t, int index, int *invertPtr){
    // 1) Rotate
    int newGateIndex = (index + (t->rotation)) % 4;
    
    // 2) Flip
    const int flipHIndexLookup[4] = {2, 1, 0, 3};
    if(t->flipH) newGateIndex = flipHIndexLookup[newGateIndex]; 
    const int flipVIndexLookup[4] = {0, 3, 2, 1};
    if(t->flipV) newGateIndex = flipVIndexLookup[newGateIndex];


    // 3) Invert if necessary
    // TODO replace with simpler logic (should be some XOR trickery we can pull to mitigate branching)
    if(t->flipH != t->flipV){ // flipH XOR flipV
        // Flip if the upper bit matches
        *invertPtr = (index & 2) == (newGateIndex & 2);
    } else {
        // Flip if the upper bit doesn't match
        *invertPtr = (index & 2) != (newGateIndex & 2);
    }

    return newGateIndex;
}


struct gate getGate(gateSet set, struct gridTransform *t, int index){
    int inverted = 0;
    int newIndex = getGateIndex(t, index, &inverted);
    struct gate out;
    if(inverted){
        out.position = ((newIndex & 1) ? t->width : t->height) - (set[newIndex].position + set[newIndex].size);
    } else {
        out.position = set[newIndex].position;
    }
    out.size = set[newIndex].size;
    return out;
}


void setGate(gateSet set, struct gridTransform *t, int index, struct gate *newGate){
    int inverted = 0;
    int newIndex = getGateIndex(t, index, &inverted);
    set[newIndex].size = newGate->size;
    if(inverted){
        set[newIndex].position = ((newIndex & 1) ? t->width : t->height) - (newGate->position + newGate->size);
    } else {
        set[newIndex].position = newGate->position;
    }
}

