#ifndef COORD_QUEUE
#define COORD_QUEUE

/* coordQueue.h
 *
 * No-frills FIFO ArrayList for 2D cartesian coordinates
 * adapted September 2020 from code written the previous June
*/


struct coordQueue {
    int *xlist, *ylist;
    unsigned int head, tail, backingArrSize;
};

// Allocates space for and returns a pointer to a new coordinate queue
//  (with reserved backing array space of the given size)
struct coordQueue *newCoordQueue(unsigned int initialSize);

// Frees the indicated coordinate queue
void freeCoordQueue(struct coordQueue *queue);

// Enqueues the coordinate given by (x, y) to the indicated queue
void enCoordQueue(struct coordQueue *queue, int x, int y);

// Dequeues a coordinate from the indicated queue, storing it in (x, y)
// Returns falsehood if the queue was empty, truth otherwise
unsigned int deCoordQueue(struct coordQueue *queue, int *x, int *y);

#endif
