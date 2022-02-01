#ifndef GRID_HEIGHT_SELECTOR
#define GRID_HEIGHT_SELECTOR

#include "parcel.h"
#include "recursorGridSignature.h"

/* gridHeightSelector.h
*
*   Sets the relative heights of the children of a grid recursor
*   TODO document strategy
*
* written January 2022
*/

void gridHeightSelect(struct parcel *parcel, const struct recursorGridSignature *signature);

#endif
