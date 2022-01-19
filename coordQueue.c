#include <stdlib.h>

#include "coordQueue.h"
// coordQueue.c
// See header file for details

struct coordQueue *newCoordQueue(unsigned int initialSize){
    struct coordQueue *q = (struct coordQueue *)malloc(sizeof(struct coordQueue));
    q->head = 0;
    q->tail = 0;
    q->backingArrSize = initialSize;
    q->xlist = (int *)malloc(sizeof(int) * initialSize);
    q->ylist = (int *)malloc(sizeof(int) * initialSize);
    return q;
}

void freeCoordQueue(struct coordQueue *queue){
    free(queue->xlist);
    free(queue->ylist);
    free(queue);
}

void enCoordQueue(struct coordQueue *queue, int x, int y){
    if(queue->tail >= queue->head){
        // Queue is empty, reset it to blank state
        queue->tail = 0;
        queue->head = 0;
    }
    if(queue->head >= queue->backingArrSize){
        queue->backingArrSize *= 2;
        queue->xlist = (int *)realloc(queue->xlist, sizeof(int) * queue->backingArrSize);
        queue->ylist = (int *)realloc(queue->ylist, sizeof(int) * queue->backingArrSize);
    }
    queue->xlist[queue->head] = x;
    queue->ylist[queue->head] = y;
    queue->head++;
}

unsigned int deCoordQueue(struct coordQueue *queue, int *x, int *y){
    if(queue->tail >= queue->head) return 0;
    *x = queue->xlist[queue->tail];
    *y = queue->ylist[queue->tail];
    queue->tail++;
    return 1;
}
