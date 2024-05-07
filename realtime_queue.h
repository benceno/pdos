#ifndef REALTIME_QUEUE_H
#define REALTIME_QUEUE_H

#include "list.h"
/*
 * First-Come, First-Served
 */
typedef struct realtime_queue
{
    struct proc *current;
    struct listproc *list;

} realtime_queue;


struct proc *nextProcRealTime(realtime_queue *queue);

#endif