#include <stdio.h>
#include <assert.h>
#include "bitmap.h"

#define LEN 5

void print_result(unsigned char *map, int len, long length, long expected) {
    long ndx = search_map(map, len, length);
    printf("Length: %ld, Expected: %ld, Actual: %ld\n", length, expected, ndx);

    // This will cause a crash if the index returned does not match what is expected
    assert(ndx == expected);
}

int main() {
    unsigned char map[LEN] = {0b11001000, 0b00011111, 0b00001100, 0b11000000, 0b00000001};
    int ndx;

    printf("\n");
    print_result(map, LEN, 2, 2);
    print_result(map, LEN, 4, 5);
    print_result(map, LEN, 6, 5);
    print_result(map, LEN, 12, 26); 
    print_result(map, LEN, 128, -1); 

    // Allocate map
    printf("\nAllocating 2 bytes\n");
    printf("BEFORE: ");
    print_map(map, LEN);
    ndx = search_map(map, LEN, 2);
    allocate_map(map, ndx, 2);
    printf("AFTER:  ");
    print_map(map, LEN);
    printf("\n");

    printf("Allocating 12 bytes\n");
    printf("BEFORE: ");
    print_map(map, LEN);
    ndx = search_map(map, LEN, 12);
    allocate_map(map, ndx, 12);
    printf("AFTER:  ");
    print_map(map, LEN);
    printf("\n");
    print_result(map, LEN, 12, -1); 

    printf("Freeing 12 bytes\n");
    printf("BEFORE: ");
    print_map(map, LEN);
    free_map(map, ndx, 12);
    printf("AFTER:  ");
    print_map(map, LEN);
    printf("\n");
    print_result(map, LEN, 12, 26); 
}
