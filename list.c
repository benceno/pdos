#include "list.h"

listproc *createListProc()
{
    // listproc *list = (listproc *)malloc(sizeof(listproc));
    listproc *list = 0;
    list->head = 0;
    list->tail = 0;
    list->size = 0;
    return list;
}

void insertProc(listproc *list, struct proc *proc)
{
    procnode *node = 0;
    // procnode *node = (procnode *)malloc(sizeof(procnode));
    node->proc = proc;
    node->next = 0;
    if (list->size == 0)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
}

void removeProc(listproc *list, struct proc *proc)
{
    procnode *aux = list->head;

    if (aux->proc == proc)
    {
        list->head = aux->next;
        // free(aux);
        list->size--;
        return;
    }
    
    procnode *auxPrev = list->head;
    for (int i = 1; i < list->size; i++)
    {
        aux = aux->next;
        if (aux->proc == proc)
        {
            auxPrev->next = aux->next;
            // free(aux);
            list->size--;
            return;
        }
        auxPrev = aux;
    }   
}