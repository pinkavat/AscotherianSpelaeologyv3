#ifndef RECURSOR_PATTERN_SELECTOR
#define RECURSOR_PATTERN_SELECTOR

#include <stdlib.h>

/* recursorPatternSelector.h
*
*   Defines the possible recursive patterns that the AscoSpel generator can come up with on its own, and
*   provides a basic weighted random distribution sampling system for selecting which pattern to use.
*
*   TODO algorithm currently uses linear weight summation; switch to something like the Alias Method as an optimization later
*
* written January 2022
*/

#define NUM_PATTERN_TYPES 9     // The size of the below enum

// The possible kinds of recursive pattern
// (enum explicit as order matters to the selector algorithm, and I still don't trust C standards (can't imagine why...))
enum recursivePatternTypes {
    PATTERN_TERMINATE = 0,          // Attempt to stop recursing and bottom out.
    PATTERN_LENGTHEN = 1,           // Pattern contains multiple units on the same critical path, but nothing else
    PATTERN_VOID_INCURSION = 2,     // Pattern contains an empty hole, to add aesthetic variety
    PATTERN_FORK = 3,               // Path bifurcates using a shortcut
    PATTERN_LEDGE_STRUCTURAL = 4,   // Path loops back on itself using a ledge
    PATTERN_LEDGE_PUZZLE = 5,       // Path branches, then branch loops back using a ledge; branch has reward on upper side of ledge
    PATTERN_FLANGE_FALSE = 6,       // Path branches, but branch has nothing at the end
    PATTERN_FLANGE_REWARD = 7,      // Path branches, branch has reward a the end.
    PATTERN_BRIDGE = 8              // Path crosses over/under itself using a bridge
};


// Patterns are chosen by consulting a probability table, which describes relative frequencies of patterns as a function of recursive depth.
struct patternProbabilityTable {
    int (*weights)[NUM_PATTERN_TYPES];  // Array of the below length, containing weights for each pattern in order.
                                        // If all weights are zero, the selector will return PATTERN_TERMINATE.

    unsigned int length;                // The number of rows in the table, each corresponding to a recursive depth. Asking for a depth below this
                                        // (i.e. an out-of-bounds array access) will return PATTERN_TERMINATE.
};


// Choose a random pattern, using the provided weighted probability table at the provided depth.
enum recursivePatternTypes selectPattern(struct patternProbabilityTable *table, int depth);


#endif
