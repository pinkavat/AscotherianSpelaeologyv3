#ifndef PARCEL_GENERATORS
#define PARCEL_GENERATORS

/* parcelGenerators.h
*
*   Inclusion collator file for all AscoSpel v3 base-case parcel generators.
*
*   A "parcel generator" is a pair of functions, consisting of
*       a) the PARCEL IDEATOR, which receives a parcel to modify. This parcel's shape and parameters fields must be populated, and act as parameters to
*           the ideator function.
*           The ideator sets the realizer func, data ptr, flex, minimum dimensions, and child array fields of the given parcel,
*           managing data and child memory as needed. The ideator also sets the grid transform of the parcel to a new blank transform.
*           The ideator may also receive further parameters as needed; such special cases are handled by the selector (for instance, the grid recursor
*           receives a grid pattern as well).
*           NOTE: all functions passed to the grid recursor through its signature are of type void func(struct parcel *).
*
*       b) the PARCEL REALIZER, which is called back from a parcel's realizer func pointer as part of the realization process. This function realizes the
*           parcel into the context map, recursively realizing child parcels, deallocating child-array and data-ptr memory if necessary, and populating the
*           parcel's residue fields (see parcel.h)
*           It is assumed that the parcel's grid transform has been adjusted by its parent(s) appropriately, so that the parcel now lies in global space.
*           The residual fields, however, remain in local space, and the realizer is given target dimensions in its local space.
*
*   To that end, each parcel generator file contains an ideator function, a realizer function, and the definition of any associated data structs handed down
*   through the parcel's data pointer or through the ideator's extra parameters.
*
* written December 2021
*/

// TODO organize by type

#include "parcelGenerators/baseCase.h"

#include "parcelGenerators/voidBubble.h"

#include "parcelGenerators/ledge.h"

#include "parcelGenerators/door.h"

#include "parcelGenerators/DFSMaze.h"

#include "parcelGenerators/icePuzzle.h"

#include "parcelGenerators/lockSmash.h"

#include "parcelGenerators/testLake.h"

#include "parcelGenerators/NPCEncounter.h"

#endif
