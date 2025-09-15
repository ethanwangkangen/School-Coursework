#include <stdio.h>
#include <assert.h>
#include "mymalloc.h"

void *runmalloc(size_t len, long expected_ndx) {

    long ndx;
    void *ptr;

    printf("Allocate %ld bytes.\n", len);
    printf("Before: ");
    print_memlist();
    printf("\n");
    ptr = mymalloc(len);
    ndx = get_index((void *) ptr);
    printf("EXPECTED: %ld ACTUAL: %ld\n", expected_ndx, ndx);
    printf("After:  ");
    print_memlist();
    printf("\n");
    assert(ndx == expected_ndx);
    return ptr;
}

void runfree(void *ptr) {
    printf("Free\n");
    printf("Before: ");
    print_memlist();
    printf("\n");
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
    ptr2 = runmalloc(12, 38);
    ptr4 = runmalloc(24, 50);
    ptr5 = runmalloc(18, 74);
    runfree(ptr1);
    ptr1 = runmalloc(2, 92);
    runfree(ptr1);
    runfree(ptr2);
    runfree(ptr3);
    runfree(ptr4);
    runfree(ptr5);
}


