#include "gridTransform.h"
// gridTransform.c
// See header for details



struct gridTransform newGridTransform(){
    struct gridTransform blank = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    return blank;
}


// I know it looks nasty. It does work, though. Spent an awful lot of time with a pad of graph paper working this one out.
// We contemplated hardcoded lookup as an alternative. Think that through? Not an urgent optimization as this op isn't terribly frequent
void gTInherit(struct gridTransform *parent, struct gridTransform *child){
    // Newly added code to conceal the need for the "dummy parent" of yore
    if(!(parent->anchored)){
        struct gridTransform dummyParent = {0, 0, 0, 0, 0, 0, 0, 0, 1};
        gTInherit(&dummyParent, parent);
        parent->anchored = 1;
    }
    child->anchored = 1;

    // 1) Based on the parent's rotation, determine where the child's "pseudo-anchor" lies in absolute space
    //      The "pseudo-anchor" is the top-left corner of the child in the unrotated parent (which would be the child's anchor if the child weren't rotated)
    int offsetX = parent->flipH ? 0 - child->x : child->x;
    int offsetY = parent->flipV ? 0 - child->y : child->y;
    switch(parent->rotation){
        case 0:
            child->x = parent->x + offsetX;
            child->y = parent->y + offsetY;
        break;
        case 1:
            child->x = parent->x - offsetY;
            child->y = parent->y + offsetX;
        break;
        case 2:
            child->x = parent->x - offsetX;
            child->y = parent->y - offsetY;
        break;
        case 3:
            child->x = parent->x + offsetY;
            child->y = parent->y - offsetX;
        break;
    }
    
    // 2) From that pseudo-anchor, find the child's actual anchor, based on the rotations of both child and parent
    // Correct for potential mirroring
    unsigned int rotCorrectionTerm = child->flipH ? (child->flipV ? 2 : 1) : (child->flipV ? 3 : 0);
    unsigned int correctedChildRotation = (child->rotation + rotCorrectionTerm) % 4;

    // Account for parent's flip in width/height calculations
    int correctedChildWidth = parent->flipH ? 0 - ((int)gTAbsWidth(child) - 1) : gTAbsWidth(child) - 1;
    int correctedChildHeight = parent->flipV ? 0 - ((int)gTAbsHeight(child) - 1) : gTAbsHeight(child) - 1;

    // Turn the pseudoanchor into the true anchor
    if(correctedChildRotation == 1 || correctedChildRotation == 2){
        switch(parent->rotation){
            case 0:
                child->x += correctedChildWidth;
            break;
            case 1:
                child->y += correctedChildWidth;
            break;
            case 2:
                child->x -= correctedChildWidth;
            break;
            case 3:
                child->y -= correctedChildWidth;
            break;
        }
    }
    if(correctedChildRotation == 2 || correctedChildRotation == 3){
        switch(parent->rotation){
            case 0:
                child->y += correctedChildHeight;
            break;
            case 1:
                child->x -= correctedChildHeight;
            break;
            case 2:
                child->y -= correctedChildHeight;
            break;
            case 3:
                child->x += correctedChildHeight;
            break;
        }
    }

    // 3) The child inherits the parent's mirroring and rotation
    if(child->rotation & 1){
        // "perpendicular" to parent, hFlip and vFlip flip
        child->flipH ^= parent->flipV;
        child->flipV ^= parent->flipH;
    } else {
        // "parallel" to parent, hFlip and vFlip persist
        child->flipH ^= parent->flipH;
        child->flipV ^= parent->flipV;
    }
    child->rotation = (parent->rotation + child->rotation) % 4;

    // Sneak in the z-inheritance here
    child->z += parent->z;
}





// Not as bad as I remember
void gTCoordinate(struct gridTransform *t, int x, int y, int *m, int *n){
    // Newly added code to conceal the need for the "dummy parent" of yore
    if(!(t->anchored)){
        struct gridTransform dummyParent = {0, 0, 0, 0, 0, 0, 0, 0, 1};
        gTInherit(&dummyParent, t);
    }

    int offsetX = t->flipH ? 0 - x : x;
    int offsetY = t->flipV ? 0 - y : y;
    switch(t->rotation){
        case 0:
            *m = t->x + offsetX;
            *n = t->y + offsetY;
        break;
        case 1:
            *m = t->x - offsetY;
            *n = t->y + offsetX;
        break;
        case 2:
            *m = t->x - offsetX;
            *n = t->y - offsetY;
        break;
        case 3:
            *m = t->x + offsetY;
            *n = t->y - offsetX;
        break;
    }
}





void gTRegionIterate(struct gridTransform *t, void *context, void(*callback)(void *, int, int, int, int), int a, int b, int w, int h){
    // Newly added code to conceal the need for the "dummy parent" of yore
    if(!(t->anchored)){
        struct gridTransform dummyParent = {0, 0, 0, 0, 0, 0, 0, 0, 1};
        gTInherit(&dummyParent, t);
    }

    // TODO input checking??
    // x and y are "sample" coordinates, in the unrotated, unflipped source t space
    // m and n are "output" coordinates, in absolute map space

    // First establish iteration increments
    int MXdelta = (t->rotation & 1) ? 0 : (t->rotation == 0) ?  1 : -1;
    int NXdelta = (t->rotation & 1) ? (t->rotation == 1) ? 1 : -1 : 0;
    int MYdelta = (t->rotation & 1) ? 1 : (t->rotation & 1 ? h : w);
    MYdelta = (t->rotation < 2) ? 0 - MYdelta : MYdelta;
    int NYdelta = (t->rotation & 1) ? (t->rotation & 1 ? w : h) : 1;
    NYdelta = (t->rotation > 0 && t->rotation < 3) ? 0 - NYdelta: NYdelta;

    // Correct for mirroring (This can probably be integrated into the above code in some more efficient way)
    // (perhaps a lookup table instead...?)
    if((t->rotation & 1) ? t->flipV : t->flipH){    // v2's "absFlipH" macro (but it's never used anywhere else)
        MXdelta = 0 - MXdelta;
        MYdelta = 0 - MYdelta;
    }
    if((t->rotation & 1) ? t->flipH : t->flipV){    // Ditto, absFlipV
        NXdelta = 0 - NXdelta;
        NYdelta = 0 - NYdelta;
    }

    // Finally, iterate
    int m, n;
    gTCoordinate(t, a, b, &m, &n);
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){

            // coordinates x, y map onto m, n
            // Invoke the callback function with said coordinates
            callback(context, x + a, y + b, m, n);

            m += MXdelta;
            n += NXdelta;
        }
        m += MYdelta;
        n += NYdelta;
    }

}
