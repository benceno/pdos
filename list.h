
/*
 * Shortest Job First
 */
struct
{
    struct listproc *process;

} LOW_QUEUE;

/*
 * First-Come, First-Served
 */
struct
{
  struct listproc *process;

} REALTIME_QUEUE;

struct listproc
{
    struct procnode *head;
    struct procnode *tail;
    int size;
};
struct procnode{
    struct proc *proc;
    struct procnode *next;
};