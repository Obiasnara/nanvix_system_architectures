
/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _ASM_FILE_

#include <nanvix/config.h>
#include <nanvix/const.h>

#ifdef SEM_ENABLED
#ifndef SEM_H_
#define SEM_H_


/**
 * @brief Command values for semaphores.
 */
/**@{*/
#define GETVAL 0   /**< Returns the value of a semaphore. */
#define SETVAL 1   /**< Sets the value of a semaphore.    */
#define IPC_RMID 3 /**< Destroys a semaphore.            */
/**@}*/

/* Forward definitions. */
extern int semget(unsigned);
extern int semctl(int, int, int);
extern int semop(int, int);
 
#define SEM_INVALID  0 /**< Invalid semaphore. */
#define SEM_VALID    1 /**< Valid semaphore.   */

struct sem
{
	int valid; /* Whether this entry in the semaphore table is valid (initialized) */
	int key; /* The name identifying this semaphore	*/
	int n; /* The semaphore's value. If zero, down() will be blocking */
	/**
	* @brief Chain of processes waiting on this semaphore
	*/
	struct process *sleep_chain;
};

// Forward definitions
EXTERN struct sem sems[SEM_MAX];

#endif /* SEM_H_ */
#endif /* SEM_ENABLED */
#endif /* _ASM_FILE_ */