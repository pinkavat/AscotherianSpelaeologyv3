
#include "rectGate.h"
// rectGate.c
// See header for details


// Helper for the below
// Public because topological adjacency determiner in the sheathing step has a good use for it as a kitbash
int getGateIndex(struct gridTransform *t, int index, int *invertPtr){
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



// Lookup table for below functions
int gatePossessionLookup[][4] = {
    {0, 0, 0, 0},   // V_SHAPE
    {1, 0, 0, 0},   // E_SHAPE
    {1, 1, 0, 0},   // L_SHAPE
    {1, 0, 1, 0},   // I_SHAPE
    {1, 1, 1, 0},   // TL_SHAPE
    {1, 1, 1, 0},   // TI_SHAPE
    {1, 1, 1, 1},   // XL_SHAPE
    {1, 1, 1, 1},   // XI_SHAPE
};

int otherHasGate(int shape, struct gridTransform *t, int index){
    // 1) Get local gate index
    int throwaway;
    int localIndex = getGateIndex(t, index, &throwaway);

    // 2) Perform lookup in above table
    return gatePossessionLookup[shape][localIndex];
}

int selfHasGate(int shape, int index){
    return gatePossessionLookup[shape][index];
}
