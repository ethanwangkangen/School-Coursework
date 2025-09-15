#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#include "llist.h"

char _heap[MEMSIZE] = {0};
TNode *_memlist = NULL; // To maintain information about length

// Do not change this. Used by the test harness.
// You may however use this function in your code if necessary.
long get_index(void *ptr) {
    if(ptr == NULL)
        return -1;
    else
        return (long) ((char *) ptr - &_heap[0]);
}

// Allocates size bytes of memory and returns a pointer
// to the first byte.
void *mymalloc(size_t size) {
    if (_memlist == NULL) { // Initialize the partition list if it doesn't exist
        TData *data = (TData *)malloc(sizeof(TData));
        data->free = true;               // Initially, the entire memory is free
        data->partitionSize = MEMSIZE;   // Full memory size for the first partition
        _memlist = make_node(0, data);   // Create the initial free partition node
    }

    // Traverse the list to find the first suitable free partition
    TNode *bestFitPtr = NULL;
    long bestSizeSoFar = 99999;

    TNode *ptr = _memlist;
    while (ptr != NULL) {
        TData *data = ptr->pdata;

        if (data->free && data->partitionSize >= size) {
            if (data->partitionSize == size) {
                // Perfect fit: mark the partition as allocated
                data->free = false;

                // Set the corresponding bits in _heap to 1
                for (long i = ptr->key; i < ptr->key + size; i++) {
                    _heap[i] = 1;
                }

                // Return pointer to the allocated memory
                return (void *)(_heap + ptr->key);
            } else if (data->partitionSize > size) {
                if (data->partitionSize < bestSizeSoFar) {
                    bestFitPtr = ptr;
                    bestSizeSoFar = data->partitionSize;
                }
            }
        }

        // Move to the next node
        ptr = ptr->next;
    }

    if (bestFitPtr != NULL) {
        TData *data = bestFitPtr->pdata;

        // From the best partition, do this
        // Split the partition: allocate and create a new free node
        int newStartAdd = bestFitPtr->key + size;
        long newPartitionSize = data->partitionSize - size;

        // Update current node as allocated and adjust partition size
        data->free = false;
        data->partitionSize = size;

        // Set the corresponding bits in _heap to 1
        for (long i = bestFitPtr->key; i < bestFitPtr->key + size; i++) {
            _heap[i] = 1;
        }

        // Create a new free partition node with the remaining space
        TData *insertData = (TData *)malloc(sizeof(TData));
        insertData->free = true;
        insertData->partitionSize = newPartitionSize;

        // Insert new node into the list for the remaining free partition
        insert_node(&_memlist, make_node(newStartAdd, insertData), ASCENDING);

        // Return pointer to the allocated memory
        return (void *)(_heap + bestFitPtr->key);

    }
                

    // No suitable partition found, return NULL
    return NULL;
}

// Frees memory pointer to by ptr.
void myfree(void *ptr) {
    // Calculate the start address in _heap
    unsigned int startAddress = (unsigned int)((char *)ptr - _heap);

    // Find the node corresponding to the start address
    TNode *node = find_node(_memlist, startAddress);
    if (node == NULL || node->pdata == NULL) {
        //dbprintf("Error: Pointer not found in memory list\n");
        return;
    }

    // Mark the partition as free
    node->pdata->free = true;

    // Update the _heap to mark the bytes as free (0)
    for (long i = startAddress; i < startAddress + node->pdata->partitionSize; i++) {
        _heap[i] = 0;
    }

    TNode *tempNode = node;

    // Attempt to merge with the preceding node if it's free
    if (node->prev != NULL && node->prev->pdata->free) {
        //dbprintf("Merging with preceding node\n");
        merge_node(_memlist, node, PRECEDING);
        node = node -> prev;
    }

    if (node == NULL) {
        dbprintf("node gone");
        node = tempNode;
    }

    // Attempt to merge with the succeeding node if it's free
    if (node->next != NULL && node->next->pdata->free) {
        //dbprintf("Merging with succeeding node\n");
        merge_node(_memlist, node, SUCCEEDING);
    }

    //dbprintf("Memory at %p freed and adjacent nodes merged if possible\n", ptr);
}

void print_memlist() {
    TNode *ptr = _memlist;
    while (ptr != NULL) {
        TData *data = ptr->pdata;

        const char *status = data->free ? "FREE" : "ALLOCATED";
        dbprintf("Status: %s Start index: %u Length: %lu\n", status, ptr->key, data->partitionSize);


        ptr = ptr->next;
    }
}

