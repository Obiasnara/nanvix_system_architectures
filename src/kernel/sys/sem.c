#include <nanvix/config.h>
#include <nanvix/const.h>
#include <sys/sem.h>
/**
 * @brief Defining the semaphore table.
 * All semaphores are stored in this table.
 * (its always full of non valid semaphores at the beginning)
 */
PUBLIC struct sem sems[SEM_MAX];

