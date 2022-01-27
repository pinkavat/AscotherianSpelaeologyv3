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


// Each cell in the grid (each parcel) is of one of the below types, which defines how the grid recursor will assign its parameters.
enum parameterDivisionTypes {
    TERMINAL,       // This parcel is a forced base case (i.e. its populator function is nonrecursive) so parameters are mostly irrelevant
    CRITICAL_PATH,  // Divide critical path params (obligates, etc.) serially among all parcels of this type
    SHORTCUT_PATH,  // Set backtracking lock to obligate for this parcel
    FLANGE_PUZZLE,  // TODO define reward theory
    FLANGE_REWARD,  // TODO "
    PRESET          // Parameters for this parcel given by an input list; consult said list and copy wholesale
}; 



struct recursorGridSignature {
    int width, height;                                  // The dimensions of the grid

    enum parcelShapes *shapes;                          // 2D row-major grid of parcel shapes of the above dimensions

    cellPopulatorFunctionPtr *populatorFunctions;       // 2D row-major grid of functions of the above dimensions; each function is invoked to generate
                                                        // the corresponding parcel

    unsigned int *rotations;                            // 2D row(etc...), contains the rotations of the corresponding parcels.
    unsigned int *flipHs;                               // Likewise, for horizontal flips
    unsigned int *flipVs;                               // Likewise, for vertical flips         (see gridTransform.h if in doubt)
   

    enum parameterDivisionTypes *parameterDivisionType; // How the grid solver should assign this parcel's parameters (see above)

    /* TODO LEGACY REMOVE 
    int *pathGroups;                                    // 2D row(etc...), contains the path IDs of the corresponding parcels (see above)
    
    unsigned int numCritPaths;                          // The number of different path IDs to expect (1 or greater; critical path IDs)
    unsigned int numShortcutPaths;                      // Ditto for -1 or lesser shortcut path IDs
    */

    int gateSourceIndices[4];                           // If the shape of the grid includes gate n, element n of this array is the index of the
                                                        // child that provides said gate. (i.e. child's absolute gate n becomes self's gate n)
                                                        // For now, default to zero if the grid doesn't possess such a gate; fail-bad but noncrashing.

    int gateIsFork[4];                                  // Array of booleans (size 4); Whether gate n is a fork (i.e. requires an extra walkway)
        // TODO possibly redundant
};

#endif
