#include <stdio.h>
#include <assert.h>
#include "mymalloc.h"

void *runmalloc(size_t len, long expected_ndx) {

    long ndx;
    void *ptr;

    printf("Allocate %ld bytes.\n", len);
    printf("Before: ");
    print_memlist();
    ptr = mymalloc(len);
    printf("After:  ");
    print_memlist();
    ndx = get_index((void *) ptr);
    printf("\n");
    assert(ndx == expected_ndx);
    return ptr;
}

void runfree(void *ptr) {
    printf("Free\n");
    printf("Before: ");
    print_memlist();
    myfree(ptr);
    printf("After:  ");
    print_memlist();
    printf("\n");
}

int main() {
    
    void *ptr1, *ptr2, *ptr3, *ptr4, *ptr5;

    ptr1 = runmalloc(4, 0);
    ptr2 = runmalloc(32, 4);
    ptr3 = runmalloc(2, 36);
    runfree(ptr2);
    ptr2 = runmalloc(12, 4);
    ptr4 = runmalloc(24, 38);
    ptr5 = runmalloc(18, 16);
    runfree(ptr1);
    ptr1 = runmalloc(2, 0);
    runfree(ptr1);
    runfree(ptr2);
    runfree(ptr3);
    runfree(ptr4);
    runfree(ptr5);
}


