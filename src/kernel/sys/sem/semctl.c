#include <sys/sem.h>
#ifdef SEM_ENABLED

void destroy(struct sem *semaphore_to_destroy){
    // TODO Implement the wake up of all processes waiting on the semaphore (semaphore_to_destroy->sleep_queue)
    semaphore_to_destroy->valid = SEM_INVALID;
}

int sys_semctl(int semid, int cmd, int val)
{
    struct sem *sem = &sems[semid]; // Get the semaphore from the table using the index

    switch (cmd)
    {
    case GETVAL:
        return sem->n; // Return the value of the semaphore
    case SETVAL:
        sem->n = val; // Set the value of the semaphore
        return 0;
    case IPC_RMID:
        destroy(sem); // Destroy the semaphore
        return 0;
    }

    return -1; // Return -1 if the command is invalid
}
#endif // SEM_ENABLED