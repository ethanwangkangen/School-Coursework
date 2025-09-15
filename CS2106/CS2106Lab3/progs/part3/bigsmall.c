#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "config.h"

// Sort through an array of VECT_SIZE integers

int main() {
    int vect[VECT_SIZE], i;
    clock_t start, end;
    double time_taken;

    srand(24601);
    for(i=0; i<VECT_SIZE; i++) {
        vect[i] = rand();
    }

    // Now find smallest and largest
    int smallest = INT_MAX;
    int largest = -INT_MAX;
    
    start = clock();
    for(i=0; i<VECT_SIZE; i++) {
        if(vect[i] < smallest) {
            smallest = vect[i];
        }

        if(vect[i] > largest) {
            largest = vect[i];
        }
    }
    end = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("\nNumber of items: %d\n", VECT_SIZE);
    printf("Smallest element is %d\n", smallest);
    printf("Largest element is %d\n", largest);
    printf("Time taken is %3.2f seconds\n\n", time_taken);
}

