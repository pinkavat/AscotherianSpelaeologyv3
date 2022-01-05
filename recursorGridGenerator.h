#ifndef RECURSOR_GRID_GENERATOR
#define RECURSOR_GRID_GENERATOR

#include "mapHelpers.h"
#include "recursorGridSignature.h"

/* recursorGridGenerator.h
*
*   The meat and bread of the AscoSpel algorithm is the recursive parcel generator, which in v3 comes in the form of a regularized grid.
*   Recursive parcels generated according to this scheme have a lot of responsibility; they have to
*       - manage the memory of their child parcels
*       - amalgamate the minimum dimensions and flexibility scores of their children
*       - redistribute target dimensions based on those flexibility scores in the realization step
*       - generate and distribute height changes and map-border adjacency
*       - compute, store, and realize sheathes for each child
*       - perform gate-gazumption and realization of gates
*       - handle inheritance for and command the realization of each child's walkway and shield (thankfully aided by a mapHelper)
*
*   Their appearance is not unlike a parcelGenerator, however, due to their extra signature parameter, they have to have special handling, and
*   can only be called by the selector or an invoking context.
*
* written January 2022
*/


// ========== DATA STRUCT ==========

//  TODO: must store a copy of the signature, minimum row/column dimensions, row/column flex scores, and sheath data for each cell

// ==========   IDEATOR   ==========

void recursorGridIdeator(struct parcel *parcel, struct recursorGridSignature *signature);

// ==========   REALIZER  ==========

void recursorGridRealizer(void *context, struct parcel *parcel);

#endif
