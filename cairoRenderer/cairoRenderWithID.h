#ifndef CAIRO_RENDER_WITH_ID
#define CAIRO_RENDER_WITH_ID

#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>

#include "../ascotherianTileMap.h"


/* cairoRenderWithID.h
*
*   Nicked from AscoSpel v1 and retrofitted with brute force
*
*   Uses cairo; compile with
*   -I/opt/local/include/cairo -L/opt/local/lib -lcairo -lm -std=c11
*
*/

void cairoRenderMap(struct ascoTileMap *map);

#endif
