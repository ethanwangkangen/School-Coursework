#include <stdio.h>
#include "bitmap.h"

// IMPLEMENTED FOR YOU
// Utility function to print out an array of char as bits
// Print the entire bitmap. Arguments: The bitmap itself, and the length of the bitmap 
// Each bit of the bitmap represents 1 byte of memory. 0 = free, 1 = allocated.
// map: The bitmap itself, an array of unsigned char.
//      Each bit of the bitmap represents one byte of memory
// len: The length of the bitmap array in characters
//
// Returns: Nothing
void print_map(unsigned char *map, int len) {
    int i, j;

    for(i=0; i<len; i++) {

        unsigned char mask = 0b10000000;
        for(j=0; j<8; j++) {
            if(map[i] & mask)
                printf("1");
            else
                printf("0");
            mask = mask >> 1;
        }
        printf(" ");
    }
    printf("\n");
}

// Search the bitmap for the required number of zeroes (representing
// free bytes of memory). Returns index of first stretch of 0s
// that meet the criteria. You can use this as an index into
// an array of char that represents the process heap
// bitmap = Bitmap declared as an array of unsigned char
// len = Length of bitmap in characters
// num_zeroes = Length of string of 0's required
// Returns: Index to stretch of 0's of required length, -1 if no such stretch can be found


// bitmap is array of CHAR, so stores each element in the array represents a BYTE

long search_map(unsigned char *bitmap, int len, long num_zeroes) {
    int currLength = 0;
    int bitIndex = 0;

    for (int byte = 0; byte < len; byte++) { // Loop through each byte
        unsigned char mask = 0b10000000;

        for (int bit = 0; bit < 8; bit++) { // Loop through the bits within each byte
            // Cannot access each bit, but can keep shifting the mask to check bit by bit.

            if ((bitmap[byte] & mask) == 0) { // Bit is free (0)
                currLength++;
                if (currLength == num_zeroes) {
                    return bitIndex - num_zeroes + 1;
                }
            } else { // Bit is allocated (1)
                currLength = 0;
            }
            mask >>= 1; // Check the next bit.
            bitIndex++;
        }
    }
    return -1;
}

// Set map bits to 0 or 1 depending on whether value is non-zero
// map = Bitmap, declared as an array of unsigned char
// start = Starting index to mark as 1 or 0
// length = Number of bits to mark
// value = Value to mark the bits as. value = 0 marks the bits
//          as 0, non-zero marks the bits as 1
// Returns: Nothing

void set_map(unsigned char *map, long start, long length, int value) {
    for (long i = 0; i < length; i++) {
        long bitPosition = start + i;
        int byteIndex = bitPosition / 8;
        int bitOffset = 7 - (bitPosition % 8); // Bit position within the byte

        if (value) {
            map[byteIndex] |= (1 << bitOffset); // Set bit to 1
        } else {
            map[byteIndex] &= ~(1 << bitOffset); // Clear bit to 0
        }
    }
}

// IMPLEMENTED FOR YOU
// Marks a stretch of bits as "1", representing allocated memory
// map = Bitmap declared as array of unsigned char
// start = Starting index to mark
// length = Number of bits to mark as "1"
void allocate_map(unsigned char *map, long start, long length) {

    set_map(map, start, length, 1);

}

// IMPLEMENTED FOR YOU
// Marks a stretch of bits as "0", representing allocated memory
// map = Bitmap declared as array of unsigned char
// start = Starting index to mark
// length = Number of bits to mark as "0"
void free_map(unsigned char *map, long start, long length) {
    set_map(map, start, length, 0);
}

