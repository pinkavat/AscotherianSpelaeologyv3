#ifndef PARCEL_GENERATOR_LEDGE
#define PARCEL_GENERATOR_LEDGE

#include "../parcel.h"      // and gridTransform.h and parcelGenParameters.h thereby
#include "../mapHelpers.h"  // and ascotherianTileMap.h thereby
#include "../ascoGenContext.h"

/* ledge.h
*
*   Parcel generator for AscoSpel v3
*
*   Description:
*       The ledge generator generates one-way ledges, and, due to the fact that its nature is bound up with loops in level topology, its 
*       parcel parametrization is a little bit different.
*       Ledge parcels can only meaningfully be of L, I, TLS, or TI shape, with some extra walkway-clashing limitations, because the parcel algorithm
*       only accounts for one entrance to a parcel (and ledge loops require two)
*       If the parcel is of L or I shape, the ledge forces one-way traversal from the entry to the exit (as expected).
*       If the parcel is of TLS shape, the ledge is (arbitrarily chosen to be) forcing one-way traversal from the noncritical "exit" to the critical path.
*       If the parcel is of TI shape, the ledge has to force one-way traversal from the critical path to the noncritical exit.
*       There is no X-ledge, though there is not necessarily any reason why, only that cases are too poorly defined for it to be worthwhile. 
*       TLI-shaped parcels were introduced later and still TODO need addition to ledge logic.
*
*   TODO discuss "treasure mode" (treasure passed in by parameter)
*
* written January 2022
*/

// ========== DATA STRUCT ==========

//  No data struct (TODO treasure mode treasure reference...? (that presumably doesn't need heap-resident data))

// ==========   IDEATOR   ==========

void ledgeIdeator(struct parcel *parcel);

// ==========   REALIZER  ==========

void ledgeRealizer(void *context, struct parcel *parcel);


#endif
