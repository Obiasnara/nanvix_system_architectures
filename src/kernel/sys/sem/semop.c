#include <sys/sem.h>
#include <nanvix/pm.h>

#ifdef SEM_ENABLED
int up(int semid) {
    struct sem *sem = &sems[semid];
    if (!(sem->valid)) return -1; // Skip invalid semaphore
    if (sem->n <= 0 && sem->sleep_chain) {
        wakeup_one(&(sem->sleep_chain));
    } else {
        sem->n++;
    }
    return 0;
}

int down(int semid) {
    struct sem *sem = &sems[semid];
    if (!(sem->valid)) return -1; // Skip invalid semaphore
    if (sem->n <= 0) {
        sleep(&(sem->sleep_chain), curr_proc->priority);
    } else {
        sem->n--;
    }
    return 0;
}

int sys_semop(int semid, int op){
    return (op >= 0) ? up(semid) : down(semid);
}
#endif // SEM_ENABLED