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
*
* written January 2022
*/

void selectAndApplyParcelGenerator(struct parcel *parcel);

#endif
