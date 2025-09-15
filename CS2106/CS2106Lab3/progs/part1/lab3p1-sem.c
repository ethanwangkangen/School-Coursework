#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define NUM_PROCESSES 5

int main() {

    int i, j;

    int shmid, pid;

    sem_t *sem;

    // Allocate shared memory for 5 semaphores
    shmid = shmget(IPC_PRIVATE, 5 * sizeof(sem_t), IPC_CREAT | 0600);

    // Attach shared memory
    sem = (sem_t *) shmat(shmid, NULL, 0);

    // Sem now is pointer to block of contiguous memory holding 5 sem_t objects.
    // Can treat it as array since array indexing in C (sem[i]) is like pointer arithmetic
    // Sem[i] same as *(sem + i)
    // Recall that this means i * sieof(sem_t) is added to sem.

    // Initialise 5 semaphores in array
    for (int i = 0; i < 5; ++i) {
        if (i == 0) {
            sem_init(&sem[i], 1, 1); // Can enter first one
        } else {
            sem_init(&sem[i], 1, 0);
        }
        
    }
    
    
    for(i=0; i<NUM_PROCESSES; i++)
    {
        if((pid = fork()) == 0) {
            break;
        }
    }

    if(pid == 0) {
        sem_wait(&sem[i]);

        printf("I am child %d\n", i);

        for(j = i*10; j<i*10 + 10; j++){
            printf("%d ", j);
            fflush(stdout);
            usleep(250000);
        }

        printf("\n\n");

        //sem_destroy(sem[i]);

        sem_post(&sem[i+1]);
    }
    else {
        for(i=0; i<NUM_PROCESSES; i++) 
            wait(NULL);

        shmdt(sem);
        shmctl(shmid, IPC_RMID, 0);
    }

}

