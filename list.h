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

struct listproc* createListProc();
void destroyListProc(struct listproc *list);
void insertProc(struct listproc *list, struct proc *proc);
void removeProc(struct listproc *list, struct proc *proc);