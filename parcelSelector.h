#ifndef PARCEL_SELECTOR
#define PARCEL_SELECTOR

#include "parcel.h"
#include "parcelGenParameters.h"
#include "recursorPatternSelector.h"

#include "parcelGenerators.h"
#include "recursorGridGenerator.h"

/* parcelSelector.h
*
*   Part of the AscoSpel v3 recursive generation process
*   Given an unset parcel to operate upon, uses its existing shape and parameter fields to choose the
*   appropriate ideator to populate the parcel's other fields.
*
*   A key part of making the algorithm produce interesting structures is picking the right patterns.
*
*   TODO further documentation.
*   TODO make data-driven
*
* written January 2022
*/

#define ABSOLUTE_MAXIMUM_RECURSIVE_DEPTH 20     // Stopgap against infinite recursion (probably never encountered in well-formed situations)

void selectAndApplyParcelGenerator(struct parcel *parcel);

#endif
