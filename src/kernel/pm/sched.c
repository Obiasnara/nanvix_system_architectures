/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/clock.h>
#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <nanvix/pm.h>
#include <signal.h>
#include <nanvix/klib.h> // For random number generator (krand)
/**
 * @brief Schedules a process to execution.
 *
 * @param proc Process to be scheduled.
 */
PUBLIC void sched(struct process *proc)
{
	proc->state = PROC_READY;
	proc->counter = 0;
}

/**
 * @brief Stops the current running process.
 */
PUBLIC void stop(void)
{
	curr_proc->state = PROC_STOPPED;
	sndsig(curr_proc->father, SIGCHLD);
	yield();
}

/**
 * @brief Resumes a process.
 *
 * @param proc Process to be resumed.
 *
 * @note The process must stopped to be resumed.
 */
PUBLIC void resume(struct process *proc)
{
	/* Resume only if process has stopped. */
	if (proc->state == PROC_STOPPED)
		sched(proc);
}

int total_ticket = 0;
/**
 * @brief Yields the processor.
 */
PUBLIC void yield(void)
{
	struct process *p;		  /* Working process.     */
	struct process *candidat; /* candidat process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;
		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	candidat = IDLE;

	// Initialize the number of tickets
	if (total_ticket == 0)
	{
		for (p = FIRST_PROC; p <= LAST_PROC; p++)
		{
			if (p->state != PROC_READY)
				continue;
			p->ntickets = 1000 / ((p->utime + p->ktime) + 1);
			total_ticket += p->ntickets; // collect total number of tickets
		}
	}

	int random = krand() % total_ticket + 1; // pick a random ticket right now (without new tickets update)
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		if (p->state != PROC_READY)
			continue;
		if (candidat == IDLE)
		{ // If previous candidate is IDLE, then candidat is p
			candidat = p;
			continue;
		}
		if (p->ntickets > random)
		{
			candidat = p;
			break;
		}
		else
		{
			p->ntickets = 1000 / ((p->utime + p->ktime) + 1);
			total_ticket += p->ntickets; // collect total number of tickets
		}
	}

	/* Switch to candidat process. */
	candidat->priority = PRIO_USER;
	candidat->state = PROC_RUNNING;
	candidat->counter = PROC_QUANTUM;
	if (curr_proc != candidat)
		switch_to(candidat);
}

// PUBLIC void yieldLottery(void)
// {
// 	struct process *p;		  /* Working process.     */
// 	struct process *candidat; /* candidate process to run. */
// 	int total_tickets = 0;

// 	/* Re-schedule process for execution. */
// 	if (curr_proc->state == PROC_RUNNING)
// 		sched(curr_proc);

// 	/* Remember this process. */
// 	last_proc = curr_proc;

// 	/* Check alarm. */
// 	for (p = FIRST_PROC; p <= LAST_PROC; p++)
// 	{
// 		/* Skip invalid processes. */
// 		if (!IS_VALID(p))
// 			continue;

// 		/* Alarm has expired. */
// 		if ((p->alarm) && (p->alarm < ticks))
// 			p->alarm = 0, sndsig(p, SIGALRM);
// 	}

// 	/* Choose a process to run next. */
// 	for (p = FIRST_PROC; p <= LAST_PROC; p++) {
// 		if (p->state != PROC_READY)
// 			continue;
// 		p->ntickets = 10;
// 		total_tickets += p->ntickets; // collect total number of tickets
// 	}

// 	candidat = IDLE;
// 	if (total_tickets > 0) {
// 		int ticket = CURRENT_TIME % total_tickets; // pick a random ticket
// 		int sum_tickets = 0;
// 		for (p = FIRST_PROC; p <= LAST_PROC; p++)
// 		{
// 			// Skip non-ready process.
// 			if (p->state != PROC_READY)
// 				continue;
// 			// When we reach the ticket drawn, process is picked
// 			sum_tickets += p->ntickets;
// 			if (sum_tickets > ticket)
// 			{
// 				candidat = p;
// 				break;
// 			}
// 		}
// 	}
// 	/* Switch to candidat process. */
// 	candidat->priority = PRIO_USER;
// 	candidat->state = PROC_RUNNING;
// 	candidat->counter = PROC_QUANTUM;
// 	if (curr_proc != candidat)
// 		switch_to(candidat);
// }

PUBLIC void yieldPriorityWorkingAndVerifiedByTeacher(void)
{
	struct process *p;		  /* Working process.     */
	struct process *candidat; /* candidat process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	/* Choose a process to run candidat. */
	candidat = IDLE;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		// Skip non-ready process.
		if (p->state != PROC_READY)
			continue;
		if (candidat == IDLE)
		{ // If previous candidate is IDLE, then candidat is p
			candidat = p;
			continue;
		}
		/*
			We are not using the priority field of the process struct
			because the only non waiting priority is PRIO_USER (40)
			so every process that is ready has the same priority.

			We also use ktime and utime to calculate the time that the process
			instead of the counter field. This is because the counter field is
			used to implement the round robin algorithm with FIFO order and we
			prefer to use the 'past time' of kernel or user time usage to help
			us chose between 2 process.

		*/

		// Combine nice value and counter
		if (p->nice < candidat->nice)
		{ // Nice comparison, the p process has higher nice priority
			candidat = p;
		}
		else if (p->nice == candidat->nice)
		{
			if (p->utime + p->ktime < candidat->utime + candidat->ktime)
			{
				candidat = p;
			}
		}
	}
	/* Switch to candidat process. */
	candidat->priority = PRIO_USER;
	candidat->state = PROC_RUNNING;
	candidat->counter = PROC_QUANTUM;
	if (curr_proc != candidat)
		switch_to(candidat);
}
PUBLIC void yieldPriorityAndNice(void)
{
	struct process *p;		  /* Working process.     */
	struct process *candidat; /* candidat process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	/* Choose a process to run candidat. */
	candidat = IDLE;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		// Skip non-ready process.
		if (p->state != PROC_READY)
			continue;
		if (candidat == IDLE)
		{ // If previous candidate is IDLE, then candidat is p
			candidat = p;
			continue;
		}
		else if (p == IDLE) // If the next candidate is IDLE, then skip
		{
			continue;
		}

		// Combine priority and nice value and counter
		if (p->priority < candidat->priority)
		{ // Priority comparison
			candidat->counter++;
			candidat = p;
		}
		else if (p->priority == candidat->priority)
		{
			if (p->nice < candidat->nice)
			{ // Nice comparison, the p process has higher nice priority
				candidat->counter++;
				candidat = p;
			}
			else if (p->nice == candidat->nice)
			{
				if (p->counter > candidat->counter)
				{ // Counter comparison
					candidat->counter++;
					candidat = p;
				}
				else
				{ // Maintain the FIFO order
					p->counter++;
				}
			}
			else
			{ // Maintain the FIFO order
				p->counter++;
			}
		}
		else
		{ // Maintain the FIFO order
			p->counter++;
		}
	}

	/* Switch to candidat process. */
	candidat->nice = candidat->nice + 1; // Increase the nice value by 1 to avoid starvation

	candidat->priority = PRIO_USER;
	candidat->state = PROC_RUNNING;
	candidat->counter = PROC_QUANTUM;
	if (curr_proc != candidat)
		switch_to(candidat);
}
PUBLIC void yieldConditionalPrioriry(void)
{
	struct process *p;		  /* Working process.     */
	struct process *candidat; /* candidat process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	/* Choose a process to run candidat. */
	candidat = IDLE;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		// Skip non-ready process.
		if (p->state != PROC_READY)
			continue;
		if (candidat == IDLE)
		{ // If previous candidate is IDLE, then candidat is p
			candidat = p;
			continue;
		}
		else if (p == IDLE) // If the next candidate is IDLE, then skip
		{
			continue;
		}

		// Combine priority and nice value and counter
		if (p->priority < candidat->priority)
		{ // Priority comparison
			candidat->counter++;
			candidat = p;
		}
		else if (p->priority == candidat->priority)
		{
			if (p->nice < candidat->nice)
			{ // Nice comparison
				candidat->counter++;
				candidat = p;
			}
			else if (p->nice == candidat->nice)
			{
				if (p->counter > candidat->counter)
				{ // Counter comparison
					candidat->counter++;
					candidat = p;
				}
				else
				{ // Maintain the FIFO order
					p->counter++;
				}
			}
			else
			{ // Maintain the FIFO order
				p->counter++;
			}
		}
		else
		{ // Maintain the FIFO order
			p->counter++;
		}
	}

	/* Switch to candidat process. */
	candidat->priority = PRIO_USER;
	candidat->state = PROC_RUNNING;
	candidat->counter = PROC_QUANTUM;
	if (curr_proc != candidat)
		switch_to(candidat);
}
PUBLIC void yieldPriority(void)
{
	struct process *p;		  /* Working process.     */
	struct process *candidat; /* candidat process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	/* Choose a process to run candidat. */
	candidat = IDLE;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		// Skip non-ready process.
		if (p->state != PROC_READY)
			continue;
		if (candidat == IDLE)
		{ // If previous candidate is IDLE, then candidat is p
			candidat = p;
			continue;
		}
		else if (p == IDLE) // If the next candidate is IDLE, then skip
		{
			continue;
		}

		// Process with lower nice value found.
		if (p->priority < candidat->priority)
		{
			candidat->counter++;
			candidat = p;
		}
		else if (p->priority == candidat->priority && p->counter > candidat->counter)
		{
			candidat->counter++;
			candidat = p;
		}
		else
		{
			p->counter++;
		}
	}

	/* Switch to candidat process. */
	candidat->priority = PRIO_USER;
	candidat->state = PROC_RUNNING;
	candidat->counter = PROC_QUANTUM;
	if (curr_proc != candidat)
		switch_to(candidat);
}
PUBLIC void yieldBasicCounter(void)
{
	struct process *p;		  /* Working process.     */
	struct process *candidat; /* candidat process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	/* Choose a process to run candidat. */
	candidat = IDLE;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;

		/*
		 * Process with higher
		 * waiting time found.
		 */
		if (p->counter > candidat->counter)
		{
			candidat->counter++;
			candidat = p;
		}

		/*
		 * Increment waiting
		 * time of process.
		 */
		else
			p->counter++;
	}

	/* Switch to candidat process. */
	candidat->priority = PRIO_USER;
	candidat->state = PROC_RUNNING;
	candidat->counter = PROC_QUANTUM;
	if (curr_proc != candidat)
		switch_to(candidat);
}