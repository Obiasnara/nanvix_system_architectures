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

		// Combine priority and nice value.
		// 3 scenarios: 1. priority is negative and nice is negative 2. priority is negative and nice is positive (vice versa) 3. priority is positive and nice is positive

		// First step out everything to positive
		int priority = p->priority;
		int nice = p->nice;
		if (priority < 0)
		{
			priority = -priority;
		}
		else
		{
			priority = priority;
		}
		if (nice < 0)
		{
			nice = -nice;
		}
		else
		{
			nice = nice;
		}
		int candidat_priority = candidat->priority;
		int candidat_nice = candidat->nice;
		if (candidat_priority < 0)
		{
			candidat_priority = -candidat_priority;
		}
		else
		{
			candidat_priority = candidat_priority;
		}
		if (candidat_nice < 0)
		{
			candidat_nice = -candidat_nice;
		}
		else
		{
			candidat_nice = candidat_nice;
		}

		// Second step, combine priority and nice value
		int p_combined = priority + nice;
		int candidat_combined = candidat_priority + candidat_nice;

		// Third step, compare
		if (p_combined > candidat_combined)
		{
			candidat->counter++;
			candidat = p;
		}
		else if (p_combined == candidat_combined && p->counter > candidat->counter)
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

		// Combine priority and nice value.
		// 3 scenarios: 1. priority is negative and nice is negative 2. priority is negative and nice is positive (vice versa) 3. priority is positive and nice is positive

		// First step out everything to positive
		int priority = p->priority;
		int nice = p->nice;
		if (priority < 0)
		{
			priority = -priority;
		}
		else
		{
			priority = priority;
		}
		if (nice < 0)
		{
			nice = -nice;
		}
		else
		{
			nice = nice;
		}
		int candidat_priority = candidat->priority;
		int candidat_nice = candidat->nice;
		if (candidat_priority < 0)
		{
			candidat_priority = -candidat_priority;
		}
		else
		{
			candidat_priority = candidat_priority;
		}
		if (candidat_nice < 0)
		{
			candidat_nice = -candidat_nice;
		}
		else
		{
			candidat_nice = candidat_nice;
		}

		// Second step, combine priority and nice value
		int p_combined = priority + nice;
		int candidat_combined = candidat_priority + candidat_nice;

		// Third step, compare
		if (p_combined > candidat_combined)
		{
			candidat->counter++;
			candidat = p;
		}
		else if (p_combined == candidat_combined && p->counter > candidat->counter)
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

		//Never let idle take if an another processus is available
		if(next == IDLE && p != NULL){
			next->counter++;
			next = p;
		}
		/*
		 * Process with higher
		 * kernel priority
		 */
		else if (p->priority < next->priority)
		{
			next->counter++;
			next = p;
		}
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