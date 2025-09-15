#define MEMSIZE 64 * 1024        // Size of memory in bytes

long get_index(void *ptr);
void *mymalloc(size_t);
void myfree(void *);
