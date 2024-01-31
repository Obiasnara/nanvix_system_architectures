#include <sys/sem.h>

int semop(int semid, int op){
    if(semid < 0 || op < 0){
        return -1;
    }

    int semaphore = semop(semid, op);

    if (semaphore < 0){
        return -1;
    }

    return semaphore;
}