
#include "recursorPatternSelector.h"
// recursorPatternSelector.c
// See header for details


enum recursivePatternTypes selectPattern(struct patternProbabilityTable *table, int depth){

    if(depth >= table->length) return PATTERN_TERMINATE;    // If we're below the table's deepest depth, try stopping recursion.
    
    // Otherwise, for the appropriate row:

    // 1) Sum row weights
    int sum = 0;
    for(int i = 0; i < NUM_PATTERN_TYPES; i++) sum += table->weights[depth][i];

    // 2) If sum is zero, try stopping recursion.
    if(sum == 0) return PATTERN_TERMINATE;


    // 3) Choose a random number otherwise, and find the associated case in the distribution
    int r = (rand() % sum) + 1;
    enum recursivePatternTypes i = 0;
    for(; r > 0; i++) r -= table->weights[depth][i];
    i--;

    return i;
}
