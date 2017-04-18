#ifndef _LINKLIST_H_
#define _LINKLIST_H_

/**
 * @file const.h
 * @brief Provide function prototype for custom singly linked list library
 */


struct List
{
    char* data;
    struct List* next;
};

/**
 * Create new node
 * @param Data in new node
 * @return Pointer to new node
 */
struct List* newList(char* data);

/**
 * Insert a node to the head of linked list
 * @param Pointer to the first node of linked list
 * @param Pointer to the node that need to be inserted
 */
void insertList(struct List** list, struct List* node);

/**
 * Print linked list
 * @param Pointer to the first node of linked list
 */
void printList(struct List* list);

/**
 * Free listed list
 * @param Pointer to the first node of linked list
 */
void freeList(struct List** list);

#endif // _LINKLIST_H_
