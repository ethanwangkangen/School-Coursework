#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#include "bitmap.h"
#include <stdbool.h>


char _heap[MEMSIZE] = {0};            // The actual heap memory
unsigned char bitmap[MEMSIZE / 8] = {0}; // Bitmap 

// Now need a data structure to store the size of each allocation.
typedef struct {
    void *ptr;     // Pointer to the allocated memory
    size_t size;   // Size of the allocated memory
} AllocationInfo;

// Example table that tracks allocations
AllocationInfo allocation_table[MEMSIZE];
int allocation_count = 0; // Current number of allocations

// Do not change this. Used by the test harness.
// You may however use this function in your code if necessary.
long get_index(void *ptr) {
    if(ptr == NULL)
        return -1;
    else
        return (long) ((char *) ptr - &_heap[0]);
}

void print_memlist() {
    // Implement this to call print_map from bitmap.c
    print_map(bitmap, MEMSIZE/8);
}

// Allocates size bytes of memory and returns a pointer
// to the first byte.
void *mymalloc(size_t size) {

    // Find the index of the first bit to be allocated
    long idx = search_map(bitmap, MEMSIZE/8, size);

    if (idx == -1) {
        // No sufficient free space
        return NULL;
    }


    // Then set the bits to 'allocated' in the bitmap
    allocate_map(bitmap, idx, size);

    // Get pointer to allocated memory (address of first bit)
    void *ptr = &_heap[idx];

    // Store pointer and size in allocation table
    if (ptr != NULL) {
        bool pointerInTable = false;

        // First check if pointer is already in the table.

        for (int i = 0; i < allocation_count; i++) {
            if (allocation_table[i].ptr == ptr) {
                pointerInTable = true;
                allocation_table[i].size += size;
                return ptr;
            }
        }
        
        // Pointer not in table.
        if (!pointerInTable) {
            allocation_table[allocation_count].ptr = ptr;
            allocation_table[allocation_count].size = size;
            allocation_count++;
        }
        
    }

    // Return the pointer
    return ptr;
}

// Frees memory pointer to by ptr.
void myfree(void *ptr) {
    // Need to get the size of the allocation. Get it by looping through allocation table
    for (int i = 0; i < allocation_count; i++) {
        if (allocation_table[i].ptr == ptr) {
            // Get size
            long size = allocation_table[i].size;

            // printf("\nSize is %ld\n", size);
            
            // Get the index of the pointer
            long startIndex = (long)((char *)ptr - &_heap[0]);

            // Found the allocation, free it
            free_map(bitmap, startIndex, size);

            // Reset the size
            allocation_table[i].size = 0;


            return;
        }
    }
}

