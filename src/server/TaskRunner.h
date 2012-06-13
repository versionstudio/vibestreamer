/*
 * Copyright (C) 2005-2010 Erik Nilsson, software on versionstudio point com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef guard_taskrunner_h
#define guard_taskrunner_h

#include <ace/synch.h>

#include "runnable.h"
#include "thread.h"
#include "singleton.h"

/**
* Task.
* Represents a task that can be scheduled for a single or repeated
* execution in a TaskRunner. The class inherits from the Runnable abstract
* so all sub classes should define the run() method for the task process.
*/
class Task : public Runnable
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	Task() : m_delay(0),
		m_period(0) 
	{

	}

	/**
	* Get the number of seconds that the task should be delayed.
	* @return the number of seconds that the task should be delayed
	*/
	const unsigned long getDelay() const {
		return m_delay;
	}

	/**
	* Get the next time the task will be executed.
	* @return the next time the task will be executed
	*/
	const ACE_Time_Value getNextExecutionTime() const {
		return m_nextExecutionTime;
	}

	/**
	* Get the number of seconds between task repetitions.
	* @return the number of seconds between task repetitions
	*/
	const unsigned long getPeriod() const {
		return m_period;
	}

	/**
	* Set the number of seconds that the task should be delayed.
	* @param delay the number of seconds that the task should be delayed
	*/
	void setDelay(const unsigned long delay) {
		m_delay = delay;
	}

	/**
	* Set the next time the task should be executed.
	* @param nextExecutionTime the next time the task should be executed
	*/
	void setNextExecutionTime(const ACE_Time_Value nextExecutionTime) {
		m_nextExecutionTime = nextExecutionTime;
	}

	/**
	* Set the number of seconds between task repetitions.
	* @param period the number of seconds between task repetitions
	*/
	void setPeriod(const unsigned long period) {
		m_period = period;
	}

private:
	ACE_Time_Value m_nextExecutionTime;

	unsigned long m_delay;
	unsigned long m_period;
};

/**
* TaskRunner.
* An instance of this class can execute schedules tasks in a single thread.
* This class is also available as a singleton.
*/
class TaskRunner : public Singleton<TaskRunner>,
				   public Runnable
{
public:
	/**
	* Default constructor.
	*/
	TaskRunner() : 	
		m_thread(this),
		m_started(false)
	{
	
	}

	/**
	* Destructor.
	*/
	~TaskRunner() {
		stop();
	}

	/**
	* Start the task runner.
	* @return true if task runner was successfully started
	*/
	bool start();

	/**
	* Stop the task runner.
	* @param graceful true if all scheduled tasks for immediate execution should
	* be executed before stopping and clearing the task queue
	* @return true if the task runner was successfully stopped
	*/
	bool stop(bool graceful=true);

	/**
	* Stop the task runner and cancel all queued tasks,
	* even if there are tasks queued for immediate execution.
	* @return true if the task runner was successfully stopped.
	*/
	bool cancel();

	/**
	* @override
	*/
	virtual void run();

	/**
	* Schedule a task for execution by the task runner.
	* @param task the task to schedule for execution.
	* The task instance will be destroyed by the task runner when no longer required
	*/
	void schedule(Task *task);

	/**
	* Schedule a task for execution by the task runner.
	* @param task the task to schedule for execution.
	* The task instance will be destroyed by the task runner when no longer required
	* @param delay the number of seconds that the task execution should be delayed
	* @param period the number of seconds between task repetitions
	*/
	void schedule(Task *task,unsigned long delay,unsigned long period);

private:
	/**
	* Pop the next task in queue.
	* @return the next task in queue, NULL if no task was found
	*/
	Task* popQueue();

	/**
	* Put a task into the queue, sorted by
	* next execution time.
	* @param task the task to put into the queue
	*/
	void putQueue(Task *task);

	/**
	* Reschedule a task for repetition.
	* @param task the task to reschedule
	*/
	void reschedule(Task *task);

	ACE_Mutex m_mutex;

	Thread m_thread;

	std::list<Task*> m_queue;

	bool m_started;
};

#endif
