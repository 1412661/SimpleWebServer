#include <stdlib.h>
#include <stdio.h>
#include "linklist.h"

/**
 * Create new node
 * @param Data in new node
 * @return Pointer to new node
 */
struct List* newList(char* data)
{
    struct List* r = (struct List*)malloc(sizeof(struct List));
    r->data = data;
    r->next = NULL;

    return r;
};

/**
 * Insert a node to the head of linked list
 * @param Pointer to the first node of linked list
 * @param Pointer to the node that need to be inserted
 */
void insertList(struct List** list, struct List* node)
{
    node->next = *list;
    *list = node;
}

/**
 * Free listed list
 * @param Pointer to the first node of linked list
 */
void freeList(struct List** list)
{
	while (*list != NULL)
	{
        if ((*list)->data != NULL)
			free((*list)->data);
        struct List* tmp = *list;
        *list = (*list)->next;
        free(tmp);
	}
}

/**
 * Print linked list
 * @param Pointer to the first node of linked list
 */
void printList(struct List* list)
{
    struct List* tmp = list;
	while (tmp != NULL)
	{
        printf("---%s---\n\n", tmp->data);
        tmp = tmp->next;
	}
}
