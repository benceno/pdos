#ifndef HGH_QUEUE_H
#define HGH_QUEUE_H

typedef struct avl_node
{
    struct proc *proc;
    struct avl_node *left;
    struct avl_node *right;
    int priority;
    int height;
} avl_node;
typedef struct avl_tree
{
    int size;
    struct avl_node *root;
} avl_tree;
/*
 * CFS (Completely Fair Scheduler)
 */
typedef struct high_queue
{
    struct proc *proc;
    struct avl_tree *tree;

} high_queue;

struct proc *nextProcHigh(high_queue *queue);
void insertProcHigh(high_queue *queue, struct proc *proc);
#endif