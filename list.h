#ifndef LISTPROC_H
#define LISTPROC_H


typedef struct listproc
{
    struct procnode *head;
    struct procnode *tail;
    int size;
} listproc;

typedef struct procnode
{
    struct proc *proc;
    struct procnode *next;
} procnode;

listproc *createListProc();
void insertProc(listproc *list, struct proc *proc);
void removeProc(listproc *list, struct proc *proc);
#endif