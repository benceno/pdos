#ifndef MEDIUM_QUEUE_H
#define MEDIUM_QUEUE_H

#include "list.h";
#include "proc.h"
/*
 * O(1) Scheduling Normal Processes
 */
typedef struct medium_queue
{
    struct proc *current;
    int currentVRInitial;
    int currentListIndex;
    struct listproc listActive[20];
    struct listproc listExpired[20];
} medium_queue;

struct proc *nextProcMedium(medium_queue queue);

#endif