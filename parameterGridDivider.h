#ifndef PARAMETER_GRID_DIVIDER
#define PARAMETER_GRID_DIVIDER

#include "parcel.h"
#include "recursorGridSignature.h"
#include "parcelGenParameters.h"

/* parameterGridDivider.h
*
*   Functionality for dividing parcelGenParameters among the children of a recursor grid as defined by a grid signature.
*   TODO document
*
* written January 2022
*/

// Divide the source parameters up among the indicated children, according to the provided grid signature.
// Use case: to be invoked by the grid recursor.
void divideParametersByGrid(struct parcelGenParameters *sourceParameters, struct recursorGridSignature *signature, struct parcel *children);

#endif
