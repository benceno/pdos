#include "high_queue.h"

int max(int a , int b){
    return a>b? a: b;
}
int min (int a, int b){
    return a<b? a: b;
}
struct proc *nextProcHigh(high_queue *queue)
{
    struct avl_tree *tree = queue->tree;
    struct avl_node *node = tree->root;
    if (node == 0)
    {
        return 0;
    }
    while (node->left != 0)
    {
        node = node->left;
    }
    struct proc *proc = node->proc;
    // free(node);
    return proc;
}

int height (struct avl_node * node){
    if(node == 0){
        return 0;
    }
    return node->height;
}
struct avl_node *rightRotate(avl_node *node){
    avl_node *left = node->left;
    avl_node *right = left->right;
    left->right = node;
    node->left = right;
    node->height = max(height(node->left), height(node->right)) + 1;
    left->height = max(height(left->left), height(left->right)) + 1;
    return left;
}
struct avl_node* leftRotate(avl_node * node)
{
    avl_node *right = node->right;
    avl_node *left = right->left;
    right->left = node;
    node->right = left;
    node->height = max(height(node->left), height(node->right)) + 1;
    right->height = max(height(right->left), height(right->right)) + 1;
    return right;
};
int getBalance(struct avl_node *node)
{
    if (node == 0)
        return 0;
    return height(node->left) - height(node->right);
}
struct avl_node *insertRec(avl_node *current, avl_node *new)
{
    if (current == 0)
    {
        return new;
    }
    if (new->priority < current->priority)
    {
        current->left = insertRec(current->left, new);
    }
    else
    {
        current->right = insertRec(current->right, new);
    }
    current->height = 1 + max(height(current->left), height(current->right));

    int balance = getBalance(current);
    if (balance > 1 && new->priority < current->left->priority)
    {
        return rightRotate(current);
    }
    if (balance < -1 && new->priority > current->right->priority)
    {
        return leftRotate(current);
    }
    if (balance > 1 && new->priority > current->left->priority)
    {
        current->left = leftRotate(current->left);
        return rightRotate(current);
    }
    if (balance < -1 && new->priority < current->right->priority)
    {
        current->right = rightRotate(current->right);
        return leftRotate(current);
    }
    return current;
}

void insertProcHigh(high_queue *queue, struct proc *proc)
{
    avl_tree *tree = queue->tree;
    avl_node *node = 0;
    // = (avl_node *)malloc(sizeof(avl_node));
    node->proc = proc;
    node->left = 0;
    node->right = 0;
    node->height = 1;
    if (tree->root == 0)
    {
        tree->root = node;
    }
    else
    {
        tree->root = insertRec(tree->root, node);
    }
    tree->size++;
}
