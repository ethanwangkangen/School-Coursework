#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "llist.h"

//#define DEBUG       // Enable debug printing

// Debug printer

void dbprintf(char *format, ...) {
#ifdef DEBUG
    va_list args;

    va_start(args, format);
    vprintf(format, args);
#endif
}

// Implements a double linked list.

// Create a new node. You need to
// Create your own TData node, populate
// it, then create a new node with a suitable
// key. Insertion into the link list is
// by ascending order of the key. An example key
// might be the starting address of a memory segment.

TNode *make_node(unsigned int key, TData *data) {
    TNode *node = malloc(sizeof(TNode));
    node->key = key;
    node->pdata = data;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

// Inserts a node into the correct point of the
// double linked list. The list is sorted
// in ascending order of the key. Duplicate keys
// are permitted, though not recommended.
// llist = Pointer to link list
// node = Pointer to node created by make_node
// dir = 0: Insert in ascending order
// dir = 1: Insert in descending order

void insert_node(TNode **llist, TNode *node, int dir) {
    if(*llist == NULL) {
        *llist = node;
        (*llist)->trav = *llist;
        (*llist)->tail = *llist;
    }
    else 
        if(((*llist)->key >= node->key && dir == 0) || (((*llist)->key <= node->key) && dir == 1)) {
            node->next = *llist;
            (*llist)->prev = node;
            *llist = node;
            (*llist)->trav = *llist;
        }
        else
        {
            TNode *trav = *llist;

            if(dir == 0)
                while(trav->next != NULL && trav->key < node->key) 
                    trav = trav->next;
            else if(dir == 1)
                while(trav->next != NULL && trav->key > node->key)
                    trav = trav->next;

            if(trav->next == NULL && ((trav->key < node->key && dir == 0) || 
                        (trav->key > node->key && dir == 1))) {
                trav->next = node;
                node->prev = trav;

                // Set the tail
                (*llist)->tail = node;
            } else {
                // Insert into the previous space

                node->next = trav;

                if(trav->prev != NULL) {
                    trav->prev->next = node;
                    node->prev = trav->prev;
                }

                if(trav->next != NULL) {
                }

                trav->prev = node;
            }
        }
}

// Remove a given node from the linked list
void delete_node(TNode **llist, TNode *node) {

    if(*llist == NULL || node == NULL)
        return;

    if((*llist)->key == node->key) {
        // Node to be deleted is at the front of the list.
        *llist = (*llist)->next;

        // Ensure that we don't point to it anymore.
        if(*llist != NULL)
            (*llist)->prev = NULL;
    }
    else
    {
        TNode *trav = *llist;

        while(trav != NULL && trav->key != node->key) 
            trav = trav->next;

        // We've found the deletion point
        if(trav != NULL) {
            trav->prev->next = trav->next;

            if(trav->next != NULL) 
                trav->next->prev = trav->prev;
            else
                (*llist)->tail = trav->prev;
        }

    }

    free(node);
}

// Find a node that has the value of key
// If there are duplicate keys, the first one encountered
// will be returned.
TNode *find_node(TNode *llist, unsigned int key) {
    if(llist == NULL)
        return NULL;

    TNode *trav = llist;

    while(trav != NULL && trav->key != key)
        trav = trav->next;

    return trav;
}

// Merge the node provided with either the node after or the node before.
// You need to manage merging the data in node->pdata yourself. This code just
// deletes the larger of the two nodes.
// dir = 0: Merge with node before
// dir = 1: Merge with node after

void merge_node(TNode **llist, TNode *node, int dir) {
    if (node == NULL) return;  // Ensure node is not NULL

    if (dir == PRECEDING) {
        if (node->prev == NULL) return;  // Can't merge with a NULL previous node

        // Merge with the previous node
        node->prev->pdata->partitionSize += node->pdata->partitionSize;

        // Update the linked list pointers
        if (node->next != NULL) {
            node->next->prev = node->prev;  // Link next node to the previous node
        }
        node->prev->next = node->next;  // Link previous node to the next node

        // Delete the current node
        free(node->pdata);  // Free the data in the node being deleted
        free(node);         // Free the node itself
    } else if (dir == SUCCEEDING) {
        if (node->next == NULL) return;  // Can't merge with a NULL next node

        // Merge with the next node
        node->pdata->partitionSize += node->next->pdata->partitionSize;

        // Update the linked list pointers
        if (node->next->next != NULL) {
            node->next->next->prev = node;  // Link next next node to current node
        }
        node->next = node->next->next;  // Link current node to the next next node

        // Delete the next node
        free(node->next->pdata);  // Free the data in the node being deleted
        free(node->next);          // Free the node itself
    }
}


// Go over every element of llist, and call func
// func prototype is void func(TNode *);

void process_list(TNode *llist, void (*func)(TNode *)) {
    TNode *trav = llist;
    while(trav) {
        func(trav);
        trav = trav->next;
    }

}


// Purge the entire list. You must
// free any dynamic data in the TData
// struct yourself.
void purge_list(TNode **llist) {
    TNode *trav = *llist, *tmp;
    while(trav) {
        tmp = trav->next;
        free(trav);
        trav = tmp;
    }

    *llist = NULL;
}


// Reset traverser
// where=0 START: Resets traverser to start of list
// where=1 END: Rsets
void reset_traverser(TNode *llist, int where)
{
    if(llist == NULL)
        return;

    if(where == FRONT)
        llist->trav = llist;
    else
        if(where == REAR)
            llist->trav = llist->tail;
}

// Get the next node
TNode *succ(TNode *llist)
{
    if(llist == NULL)
        return NULL;

    TNode *ret = llist->trav;

    if(llist->trav != NULL)
        llist->trav = llist->trav->next;

    return ret;
}

// Get the previous node
TNode *pred(TNode *llist)
{
    if(llist == NULL)
        return NULL;

    TNode *ret = llist->trav;

    if(llist->trav != NULL)
        llist->trav = llist->trav->prev;

    return ret;
}
