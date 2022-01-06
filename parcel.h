#ifndef PARCEL
#define PARCEL

#include "gridTransform.h"
#include "parcelGenParameters.h"

/* parcel.h
*
*   Defines a structure for recursive generation of game levels on a discrete 2D rectilinear grid
*   
*   A "parcel" is an abstract rectangular region of discrete dimensions.
*   It may recursively contain child parcels, which are transformed relative to their parent.
*   Each parcel references a realization function, which is called once the entire tree of parcels is generated, in order to convert
*   the parcel from an abstract representation to a concrete one. 
*   These functions are defined by the use case of the parcel tree.
*
*   The specific use case for which this device is here tailored is Project AscoSpel v3. As such, each parcel also requires:
*       a) a shape (see "enum parcelShapes" below)
*       b) flexibility ratings for the X and Y dimensions
*       c) minimum dimensions
*   These data are not necessarily intrinsic to the nature of a parcel, but their use is so bound up with it that they are better here.
*   Premature modularization sank the last two AscoSpel Projects. Let's make this one different!
*       NOTE: Z was snuck into the gridTransform struct. It's not strictly part of gridTransform, but putting it there simplifies some helper ops immensely.
*
*   The parcel also has space for storing "residual" data. These data are not set until realization, but need to be known by an invoking context.
*   These data include gates (whose interpretation is derived from the Parcel Shape [see below]),
*       and also two grid transforms describing the entry walkway and the optional blockage shield.
*       Due to the need for the generator to set tile-level permissions to enable smart postprocessing, the parcel cannot generate its own
*       walkway and shield; it needs to know where the adjacent parcel's exit gate connects to its entry to draw the path.
*       Hence, the parcel realizes its own content, then merely describes the area occupied by the walkway and shield as a residual, to be realized
*       by the invoking context.
*
*   NOTE: the memory allocation scheme is slightly different from v2. Parcel-generating functions now modify a preallocated space, instead of returning
*         a pointer to a freshly-allocated heap struct. This change is because we need to deal with a parcel post-realization (residue reading) and so
*         child parcels cannot be allowed to free themselves.
*
*
* written December 2021
*/

// Macros for determining the MINIMUM dimensions in absolute space of a parcel
// (lifted wholesale from gridTransforms as part of the minDims kludge)
#define parcelAbsMinWidth(p) (((p)->transform.rotation & 1) ? (p)->minHeight : (p)->minWidth)
#define parcelAbsMinHeight(p) (((p)->transform.rotation & 1) ? (p)->minWidth : (p)->minHeight)

// Ditto for the flex scores
#define parcelAbsFlexX(p) (((p)->transform.rotation & 1) ? (p)->flexY : (p)->flexX)
#define parcelAbsFlexY(p) (((p)->transform.rotation & 1) ? (p)->flexX : (p)->flexY)


/* Realization function:
*       In order to convert the parcel from an abstract representation to a concrete one, each parcel needs a realizer. This function
*       takes the parcel it is referenced in and a context (usually a tilemap) as parameters. The function then "realizes" said parcel into the context.
*
*   These functions, in general, do the following:
*       1) Precompute necessary adjustments (e.g. distribute size increases, finalize their children's positions)
*       2) Realize concrete data (base-case cores, etc.)
*       3) Recurse on their children
*       4) Process child residual data (gate-gazumption step, etc.)
*       5) Free their children
*       6) Free realizer data if dynamic
*       7) Populate their own residual data fields (gates, walkways, etc.)
*
*   Before a realizer can be run:
*       a) The parcel's transform must have its dimensions set to the target dimensions in the parcel's local space (transforming these is the invoker's
*           responsibility). These target dimensions are guaranteed to be greater than or equal to the minimum dimensions given by the parcel.
*       b) The parcel's transform must inherit from the invoker's transform.
*
*/


/* Parcel Shapes: 
*       Since the AscoSpel generation process is essentially a rewrite grammar, recursion can't change the topology of the overall structure.
*       In AscoSpel v1, this meant that all recursive subunits were of degree 2. In AscoSpel v2, the idea was refined a bit with the addition of
*       flanges: as long as the "critical path" is unambiguous and the outward topology unchanged, extra topology can be added by the recursive rewrite.
*       Parcels can therefore be lumped into "shapes" based on their topological connectivity, with subshapes resolving ambiguities in the direction of the
*       "critical path". These shapes are named after the rough appearance of the path connectivity within the parcel:
*
*       V (for "void") has no entrances or exits; this shape is created to provide empty zones in the map and more visually interesting map edges
*       E (for "end") has only one connection through gate 0; this is the end of a flange path
*       L enters through gate 0 and leaves through gate 1
*       I enters through gate 0 and leaves through gate 2
*       TL enters through gate 0 and leaves through gate 1, with gate 2 being a flange
*       TI enters through gate 0 and leaves through gate 2, with gate 1 being a flange
*       XL enters through gate 0 and leaves through gate 1, with gates 2 and 3 being flanges
*       XI enters through gate 0 and leaves through gate 2, with gates 1 and 3 being flanges
*
*/
enum parcelShapes {V_SHAPE, E_SHAPE, L_SHAPE, I_SHAPE, TL_SHAPE, TI_SHAPE, XL_SHAPE, XI_SHAPE};



struct parcel {

    // These fields are set by the initial parcel selector. Each blank parcel gets assigned a shape, and the generation parameters under which it was chosen.

    enum parcelShapes shape;                // The shape this parcel is to be
    struct parcelGenParameters parameters;  // A copy of the generation parameters used by the selector when choosing this parcel
                                            // retained here to a) reduce parameter redundancy and b) provide a hook into global parameters for the realizers


    // These fields are set by the ideator function generating this parcel

    void (*realizer)(void *context, struct parcel *parcel);        // Realization function for this parcel; see above
    void *data;                         // Pointer to supplemental data for specific parcel type (grid layouts, prefab ID, etc.) Note: memory management
                                        // for the structure hereto pointed is the responsibility of the generator/realizer (as it may be a static ref or NULL)
    
    float flexX, flexY;                 // Normalized flexibility ratings for the X and Y dimensions respectively
                                        // 0 implies the parcel is a "prefab" i.e. not flexible
                                        // 1 implies the parcel can realize to any size without loss of interesting properties

    int minWidth, minHeight;            // The minimum dimensions of the parcel in parcel-local space

    struct parcel *children;            // Heap-resident array of child parcels; managing the memory is the responsibility of the parcel generator/realizer
    unsigned int childCount;            // Size of the above array


    // These fields are set by the invoker's realizer

    struct gridTransform transform;     // 2D (plus basic Z) transform of said parcel (guaranteed to be blanked by the generator)
                                        // The transform's width and height are set to the target dimensions requested by the invoker's realizer later.

    // These fields are "residue"; populated by the realizer function and used by the invoker's realizer

    struct gridTransform gates[4];      // Up to four (number given by shape) gates, in parcel-local space, from left clockwise. 

    struct gridTransform walkway;       // A rectangular region in parcel-local space describing the walkway of the parcel.
    struct gridTransform shield;        // Likewise, for the shield.

};





#endif
