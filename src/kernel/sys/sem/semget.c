#include <sys/sem.h>
// 64 sem at max
int sys_semget(int key){
    if(key < 0 || nsems < 0 || semflg < 0){
        return -1;
    }

    int semaphore = semget(key, nsems, semflg);

    if (semaphore < 0){
        return -1;
    }

    return semaphore;
}