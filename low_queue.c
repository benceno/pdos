#include "low_queue.h"
#include "list.h"

struct proc *nextProcLow(low_queue *queue)
{
    listproc *list = queue->list;
    if (list->size == 0)
    {
        return 0;
    }
    
    procnode *aux = list->head;
    struct proc *proc = aux->proc;
    removeProc(list, proc);
    return proc;
}