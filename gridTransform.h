#ifndef GRID_TRANSFORM
#define GRID_TRANSFORM

/* gridTransform.h
*
*   Basic functionality for rotating and mirroring a discrete 2D grid. Designed to allow grid-based procgen algorithms to operate
*   in local space as part of Project AscoSpel v3.
*   (Because of this use case, we sneak in a z-coordinate. Much easier putting it here)
*
*   A lesson in hubris; the previous version of this file had what I thought was a clever way to sidestep the bloated core of the inheritance algorithm.
*   I was wrong. Code lifted from AscoSpel v2 is instead used; though it is inelegant, it is efficient (hopefully, though I've probably overlooked an
*   incredibly simple means of implementation). Modularizing the grid transform (as opposed to having it be an inextricable part of v2's parcelTrees) does
*   have some benefits, though, so it's not all effort wasted.
*
* written December 2021
*/

struct gridTransform {

    unsigned int rotation;      // Clockwise from top in 90 degree increments (int for speed -- any reason for packing?)
    unsigned int flipH, flipV;  // Booleans; whether the grid is mirrored about the y- or x-axis (respectively) BEFORE rotation

    int x, y, z;                // Coordinates of the top-left corner of the grid (NOTE: Z is tacked on from the use case)
    int width, height;          // The width and height of the grid (though negative dimensions may lead to undefined behaviour, this saves on casting)

    unsigned int anchored;      // Internal kludge flag; set this to zero upon struct creation and do not touch.
                                // If a "bare" grid is used (i.e. not inheriting from a parent) then the results of transforms are wrong, as the
                                // inheritance process adjusts the grid's top-left corner point. AscoSpel v2 used a "dummy parent" to counteract this;
                                // in order to conceal this behaviour for v3 modularization we will use a secret flag, checked whenever transforms are sought.
};


// Macros for determining the dimensions in absolute space of a transformed grid
// (e.g. if a grid is rotated 90 degrees, what it believes to be its width is its height from an outside view)
#define gTAbsWidth(g) (((g)->rotation & 1) ? (g)->height : (g)->width)
#define gTAbsHeight(g) (((g)->rotation & 1) ? (g)->width : (g)->height)


// Return a blank grid transform (a whole struct, mind; not a pointer to something heap-resident)
struct gridTransform newGridTransform();


// Modify child grid transform to place it in parent grid transform's space (i.e. the child is now relative to the parent's frame of reference)
void gTInherit(struct gridTransform *parent, struct gridTransform *child);


// Given a grid transform and a coordinate (x, y) within that grid, set (m, n) to the equivalent coordinate in absolute space
void gTCoordinate(struct gridTransform *t, int x, int y, int *m, int *n);



// "Legacy" function: this is almost certainly just as inefficient as calling gTCoordinate over and over.
// It boils down to "Function Call -> Branch -> Math -> Branch -> Math" versus "Branch -> Branch -> Function Call -> Math -> Math" for each grid square.
//
// Callback-based iterator; invokes the provided callback function for every grid square in a region of the given grid, transformed appropriately
// The parameters of the callback function are as follows:
//  - the void * is the parameter to which the "context" parameter of gTRegionIterate will be passed unmodified (something like, say, a destination tilemap)
//  - the first two ints are the (x, y) coordinates within the grid, in unrotated, unpositioned relative space.
//  - the last two ints are the (x, y) absolute coordinates (within the context, perhaps) achieved by transforming the first two coordinates.
//
//  a, b, w, and h define the rectangular region of the grid that will be iterated over; top-left x, top-left y, width, and height, respectively.
//      note that the x and y given to the callback are relative to the grid entire and not to this rectangle.
void gTRegionIterate(struct gridTransform *t, void *context, void(*callback)(void *, int, int, int, int), int a, int b, int w, int h);


#endif
