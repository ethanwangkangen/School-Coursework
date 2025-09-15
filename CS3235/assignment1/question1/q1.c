#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_stdin();
void parse_hex_from_stdin(char*);
void _print_secret();

int main()
{
    // DO NOT MODIFY THE PROGRAM
    //
    // Task: Construct an input string that if provided to this program as stdin, 
    //       will cause it to print out the secret_key in GDB debug mode.
    //
    // Setup: 
    //   - OnlineGDB, Debug mode; or gcc 11.4.0 (for x64)
    //   - Extra compiler flags: -fno-stack-protector -O0 -g -no-pie
    //   - No Command line arguments.
    //   - Standard Input: choose `text` and fill in your input string.
    //
    // Check it your exploit works:
    //   1. click on the `Debug` button
    //   2. In GDB console, type `run` and execute.
    //   4. Check your stdout between markers `=== START ===` and `=== DONE ===`
    //
    // Expected stdout (between markers):
    //   === START ===
    //   ...
    //   The secret is: cs3235@2025-2026
    //   === DONE ===
    //  Note that '=== DONE ===' should also be present in the output.
    
    printf("=== START ===\n");
    parse_stdin();
    printf("=== DONE ===\n");
    exit(0);
}

void parse_stdin() {
    char buffer[16];
    parse_hex_from_stdin(buffer);
}

void parse_hex_from_stdin(char* buf) {
    int count = 0;
    // read 4 chars (e.g., '\\x9d') at a time, until no more chars to read
    while (1) {
        char hex[5];
        if (scanf("%4s", hex) != 1) break;
        if (hex[0] != '\\' || hex[1] != 'x') exit(3);
        char to_write = (char)strtol(hex+2, NULL, 16);
        printf("buf[%d]@%p: %02hhX -> %02hhX\n", count, &buf[count], buf[count], to_write);
        buf[count++] = to_write;
    }
    printf("Read Count: %d\n", count);
}

void _print_secret() {
    printf("The secret is: cs3235@2025-2026\n");
}


