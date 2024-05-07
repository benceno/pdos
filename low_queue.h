#ifndef LOW_QUEUE_H
#define LOW_QUEUE_H

/*
 * Round Robin
 */
typedef struct low_queue
{
    struct listproc *list;
} low_queue;

struct proc *nextProcLow(low_queue *queue);

#endif