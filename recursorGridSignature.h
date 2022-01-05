#ifndef RECURSOR_GRID_SIGNATURE
#define RECURSOR_GRID_SIGNATURE

#include "parcel.h"
#include "parcelGenerators.h"

/* recursorGridSignature.h
*
*   The core of the AscoSpel v3 generator is the gridded recursor, in which the parcel generation algorithm recurses upon a regular grid of child parcels.
*   In order to make these grids easy to reason about, this file provides a means to describe them to the recursor. Such a struct then constitutes an
*   additional parameter to the recursive grid parcel generator, handled by the selector or invoking context.
*
* written January 2022
*/

typedef void (*cellPopulatorFunctionPtr)(struct parcel *);  // The class of functions that can be run to populate parcels in the recursor grid
                                                            // Includes the selector and all generators that have no extra parameters (i.e. all except
                                                            // the grid recursor generator)
                                                            // I wouldn't have written this out but I can't actually find what the syntax for a 
                                                            // pointer to a function pointer is and I don't wish to guess

struct recursorGridSignature {
    int width, height;                                  // The dimensions of the grid

    enum parcelShapes *shapes;                          // 2D row-major grid of parcel shapes of the above dimensions

    cellPopulatorFunctionPtr *populatorFunctions;       // 2D row-major grid of functions of the above dimensions; each function is invoked to generate
                                                        // the corresponding parcel

    unsigned int *rotations;                            // 2D row(etc...), contains the rotations of the corresponding parcels.
    unsigned int *flipHs;                               // Likewise, for horizontal flips
    unsigned int *flipVs;                               // Likewise, for vertical flips         (see gridTransform.h if in doubt)
    
    // TODO PARAMETER DIVISION CLASSES
};

#endif
