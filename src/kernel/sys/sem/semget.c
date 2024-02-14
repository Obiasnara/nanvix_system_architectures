#include <sys/sem.h>
#include <nanvix/pm.h>
#include <errno.h>

#ifdef SEM_ENABLED
int create(int key){
    for(int i = 0; i<SEM_MAX;i++){
         // Find the first invalid semaphore (empty space in the table)
        if(!sems[i].valid) {
            // Initialize the semaphore
            sems[i].valid = 1;
            sems[i].key = key;
            sems[i].n = 0;
            sems[i].sleep_chain = NULL;
            return i; 
        }
    }
    curr_proc->errno = ENOSPC; // All semaphores are in use
    return -1;
}

int sys_semget(int key){
    // Look for a semaphore with the given key
    for (int i = 0; i < SEM_MAX; i++){
        if(!sems[i].valid){ // If the semaphore is not valid, continue
            continue;
        }
        if (sems[i].key == key){
            return i; // Found, return its index (id)
        }
    }
    // No semaphore with this key in the table, create it
    return create(key);
}
#endif // SEM_ENABLED