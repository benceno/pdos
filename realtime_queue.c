#include "realtime_queue.h"
#include "proc.h"

struct proc *nextProcRealTime(realtime_queue *queue)
{
    listproc *list = queue->list;
    if (list->size == 0)
    {
        return 0;
    }
    if (queue->current->state == RUNNING)
    {
        return queue->current;
    }

    procnode *aux = list->head;
    struct proc *proc = aux->proc;
    removeProc(list, proc);
    queue->current = proc;
    return proc;
}