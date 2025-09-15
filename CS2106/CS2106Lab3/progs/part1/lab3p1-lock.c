#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define NUM_PROCESSES 5

int main() {

    int i, j, pid;

    // We create a new shared variable for our lock
    int *lock;
    int shmid;
    
    shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    lock = shmat(shmid, NULL, 0);

    // If lock is 1, we get to run our code.
    lock[0] = 1;

    for(i=0; i<NUM_PROCESSES; i++)
    {
        if((pid = fork()) == 0) {
            break;
        }
    }

    if(pid == 0) {

        // Spin lock until lock is 1
        while(lock[0] == 0);

        // Claim to lock
        lock[0] = 0;
        printf("I am child %d\n", i);

        for(j = i*10; j<i*10 + 10; j++){
            printf("%d ", j);
            fflush(stdout);
            usleep(250000);
        }

        printf("\n\n");

        // Release the lock
        lock[0] = 1;
    }
    else {
        for(i=0; i<NUM_PROCESSES; i++) 
            wait(NULL);

        shmdt(lock);
        shmctl(shmid, IPC_RMID, 0);
    }

}

