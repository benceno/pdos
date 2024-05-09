#include "medium_queue.h"

struct proc *nextProcMedium(medium_queue queue){
    if (queue.current != 0)
    {
        /* TODO: atualizar a prioritade do processo */
        int newPriority = (queue.current.ticks - queue.currentVRInitial)%20;

        insertProc(queue.listExpired[newPriority], queue.current);
        queue.current = 0;
    }
    
    for (int i = queue.currentListIndex; i < 20; i++)
    {
        if (queue.listActive[i].size > 0)
        {
            procnode *aux = queue.listActive[i].head;
            struct proc proc = aux.proc;
            removeProc(&queue.listActive[i], proc);
            queue.current = proc;
            queue.currentVRInitial = proc.ticks;
            queue.currentListIndex = i;
            return proc;
        }
    }

    queue.currentListIndex = 0;
    struct listproc aux[20] = queue.listExpired;
    queue.listExpired = queue.listActive;
    queue.listActive = aux;
    free(aux);

    for (int i = queue.currentListIndex; i < 20; i++)
    {
        if (queue.listActive[i].size > 0)
        {
            procnode aux = queue.listActive[i].head;
            proc proc = aux.proc;
            removeProc(queue.listActive[i], proc);
            queue.current = proc;
            queue.currentVRInitial = proc.ticks;
            queue.currentListIndex =i;
            return proc;
        }
    }
    return 0;
}