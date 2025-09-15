#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

int _nproc;
sem_t *_sems;
int *_count;

void init_barrier(int numproc) {
    _nproc = numproc;

    // Allocate shared memory for semaphores and count
    int _shmid = shmget(IPC_PRIVATE, 2 * sizeof(sem_t), IPC_CREAT | 0600);
    int _shmid2 = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);

    // Attach shared memory
    _sems = (sem_t *) shmat(_shmid, NULL, 0);
    _count = (int *) shmat(_shmid2, NULL, 0);

    // Initialize shared count
    *_count = 0;

    // Initialize semaphores
    sem_init(&_sems[0], 1, 0);  // barrier
    sem_init(&_sems[1], 1, 1);  // mutex
}

void reach_barrier() {
    // Lock the mutex to modify the shared count
    sem_wait(&_sems[1]);
    (*_count)++;
    sem_post(&_sems[1]);

    // Check if all processes have reached the barrier
    if (*_count >= _nproc) {
        // Signal the barrier
        sem_post(&_sems[0]);
    } else {
        // Wait for the barrier to be released
        sem_wait(&_sems[0]);

        // Allow others to pass the barrier as well
        sem_post(&_sems[0]);
    }
}

void destroy_barrier(int my_pid) {
    if (my_pid != 0) {
        // Cleanup: destroy the semaphores
        sem_destroy(&_sems[0]);
        sem_destroy(&_sems[1]);

        // Detach and remove shared memory
        shmdt(_sems);
        shmdt(_count);
    }
}

