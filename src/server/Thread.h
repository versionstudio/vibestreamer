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

#ifndef guard_thread_h
#define guard_thread_h

#include <ace/event.h>
#include <ace/synch.h>
#include <ace/thread.h>
#include <ace/tss_t.h>

#include "runnable.h"

class Thread; // forward declaration

/**
* ThreadStore.
* Thread specific storage class used for storing a pointer
* to the Thread instance representing a running thread.
*/
class ThreadStorage
{
public:
	/**
	* Default construtor.
	* @return instance
	*/
	ThreadStorage() : m_instance(NULL) {

	}

	/**
	* Get the thread instance.
	* @return the thread instance
	*/
	Thread* getInstance() {
		return m_instance;
	}

	/**
	* Set the thread instance.
	* @param instance the thread instance
	*/
	void setInstance(Thread *instance) {
		m_instance = instance;	
	}

private:
	Thread *m_instance;
};

/**
* Thread.
* Thread class wrapped around the ACE threading classes.
*/
class Thread
{
public:
	enum State 
	{
		INVALID		= 0x00000001,
		CANCELLED	= 0x00000002,
		INTERRUPTED = 0x00000004,
		SIGNALED	= 0x00000008,
		TIMEDOUT	= 0x00000010
	};

	/**
	* Constructor used for creating a new instance.
	* @param runnable the class implementing Runnable
	* @return instance
	*/
	Thread(Runnable *runnable) : m_alive(false),
		m_monitor(m_mutex),
		m_states(INVALID),
		m_waiting(false)
	{
		m_runnable = runnable;
	}

	/**
	* Start the thread.
	* @return true if the thread was successfully started
	*/
	bool start();

	/**
	* Cancel and interupt the thread.
	* @return false if the thread was previously flagged as INTERRUPTED
	*/
	bool cancel();

	/**
	* Get the Thread instance representing the current thread.
	* This method only works within a thread created through this
	* class, meaning it does not support threads created in other ways.
	* @return a thread instance, or NULL if no thread instance was found
	*/
	static Thread* current();

	/**
	* Interrupt the thread.
	* Any thread waiting for on thread will be signaled and released.
	* If no thread is waiting, a flag is set and the next attempt to wait()
	* will returned interrupted state without blocking.
	* @return false if the thread was previously flagged as INTERRUPTED
	*/
	bool interrupt();

	/**
	* Notify the thread.
	* Any thread waiting on this thread will be signaled and released.
	* If no thread is waiting, a flag is set and the next attempt to wait()
	* will return SIGNALED state without blocking.
	* @return false if the thread was previously flagged as INTERRUPTED
	*/
	bool notify();

	/**
	* Wait for a state change.
	* @param timeout the maximum time in milliseconds to block
	* If timeout is zero the wait will be forever.
	* @return INTERRUPTED if the wait was ended by a interrupt()
	*		  or SIGNALED if the wait was ended by a notify()
	*		  or TIMEOUT if the maximum wait time expired.
	*/
	State wait(unsigned int timeout=0);

	/**
	* Wait for the thread to exit.
	* @param timeout the maximum time in milliseconds to block
	* @return false if the maximum wait time expired
	*/
	bool join(unsigned int timeout=0);

	/**
	* Cause the thread to sleep for the given timeout.
	* A zero timeout will cause the thread to yield.
	* @param timeout the amount of milliseconds to block
	* @return false if the thread was interrupted while sleeping
	*/
	bool sleep(unsigned int timeout);

	/**
	* Cause the thread to pause temporarily, allowing other threads to execute.
	*/
	void yield();

	/**
	* Get the thread id.
	* @return the id of the thread.
	*/
	uint64_t getId() {
		return (uint64_t)m_thread;
	}

	/**
	* Get whether the thread is running or not.
	* @return true if the thread is running
	*/
	bool isAlive();

	/**
	* Get whether the thread is flagged as CANCELLED.
	* If the calling thread is the current thread, the interrupted status
	* will be cleared.
	* @return true if the thread is flagged as CANCELLED
	*/
	bool isCancelled();

	/**
	* Get whether the thread is flagged as INTERRUPTED.
	* If the calling thread is the current thread, the interrupted status
	* will be cleared.
	* @return true if the thread is flagged as INTERRUPTED
	*/
	bool isInterrupted();

private:
	/**
	* Callback for the thread.
	*/
	static void threadFunc(void *instance);

	/**
	* Process the thread.
	*/
	void process();

	static ACE_TSS<ThreadStorage> m_tss;

	ACE_Condition<ACE_Mutex> m_monitor;

	ACE_Mutex m_mutex;

	ACE_thread_t m_thread;

	Runnable *m_runnable;

	std::list<ACE_Event*> m_joiners;

	unsigned int m_states;

	bool m_alive;
	bool m_waiting;
};

#endif
