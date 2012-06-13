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

#include "common.h"
#include "thread.h"

#include <ace/os.h>

ACE_TSS<ThreadStorage> Thread::m_tss;

bool Thread::start()
{
	ACE_Guard<ACE_Mutex> guard(m_mutex);

	if ( m_alive ) {
		return false;
	}

	m_states = State::INVALID;
	m_alive = true;

	if ( ACE_Thread::spawn(ACE_THR_FUNC(threadFunc),this,
		THR_NEW_LWP|THR_JOINABLE,&m_thread)!=0 ) 
	{
		m_alive = false;
		return false;
	}

	return true;
}

bool Thread::cancel()
{
	ACE_Guard<ACE_Mutex> guard(m_mutex);

	m_states |= State::CANCELLED;

	bool interruptable = !(m_states & State::INTERRUPTED);
	if ( interruptable )
	{
		m_states |= State::INTERRUPTED;
		if ( m_waiting ) {
			m_monitor.signal();
		}
	}

	return interruptable;
}

Thread* Thread::current()
{
	return m_tss->getInstance();
}

bool Thread::interrupt()
{
	ACE_Guard<ACE_Mutex> guard(m_mutex);

	bool interruptable = !(m_states & State::INTERRUPTED);
	if ( interruptable )
	{
		m_states |= State::INTERRUPTED;
		if ( m_waiting ) {
			m_monitor.signal();
		}
		else {
			interruptable = ACE_Thread::self()!=m_thread;
		}
	}

	return interruptable;
}

bool Thread::notify()
{
	ACE_Guard<ACE_Mutex> guard(m_mutex);

	bool notifiable = !(m_states & State::INTERRUPTED);
	if ( notifiable )
	{
		m_states |= State::SIGNALED;
		if ( m_waiting ) {
			m_monitor.signal();
		}
	}

	return notifiable;
}

Thread::State Thread::wait(unsigned int timeout)
{
	State state(INVALID);

	m_mutex.acquire();
	m_waiting = true;
	m_mutex.release();

	m_mutex.acquire();

	int result = 0;
	if ( timeout==0 ) {
		m_monitor.wait();
	}
	else 
	{
		ACE_Time_Value tv = ACE_Time_Value(ACE_OS::gettimeofday() 
			+ ACE_Time_Value(0,timeout*1000));

		result = m_monitor.wait(&tv);
	}

	m_mutex.release();

	m_mutex.acquire();

	if ( result==-1 ) {
		m_states |= State::TIMEDOUT;
	}

	m_waiting = false;

	if ( m_states & State::SIGNALED ) {
		m_states &= ~(State::SIGNALED|State::TIMEDOUT);
		state = State::SIGNALED;
	}
	else if ( m_states & State::TIMEDOUT ) {
		state = State::TIMEDOUT;
	}
	else if ( m_states & State::INTERRUPTED ) {
		state = State::INTERRUPTED;
	}

	//m_monitor.reset();

	m_mutex.release();

	return state;
}

bool Thread::join(unsigned int timeout)
{
	ACE_Guard<ACE_Mutex> guard(m_mutex);

	if ( ACE_Thread::self()==m_thread ) {
		return false;
	}

	if ( !m_alive ) {
		return true;
	}

	ACE_Event *joiner = new ACE_Event();
	m_joiners.push_back(joiner);

	guard.release();

	int result = 0;
	if ( timeout==0 ) {
		joiner->wait();
	}
	else {
		ACE_Time_Value tv(0,timeout*1000);
		result = joiner->wait(&tv,0);
	}

	delete joiner;

	if ( result==-1 ) {
		return false;
	}
	else {
		return true;
	}
}

bool Thread::sleep(unsigned int timeout)
{
	if ( timeout==0 ) {
		yield();
	}
	else
	{
		State state = wait(timeout);
		if ( state==State::INTERRUPTED ) {
			return false;
		}
	}

	return true;
}

void Thread::yield()
{
	wait(1);
}

bool Thread::isAlive()
{
	ACE_Guard<ACE_Mutex> guard(m_mutex);
	
	return m_alive;
}

bool Thread::isCancelled()
{
	ACE_Guard<ACE_Mutex> guard(m_mutex);

	bool cancelled = m_states & State::CANCELLED;
	if ( cancelled && ACE_Thread::self()==m_thread ) {
		m_states &= ~State::INTERRUPTED;
	}

	return cancelled;
}

bool Thread::isInterrupted()
{
	ACE_Guard<ACE_Mutex> guard(m_mutex);

	bool interrupted = m_states & State::INTERRUPTED;
	if ( interrupted && ACE_Thread::self()==m_thread ) {
		m_states &= ~State::INTERRUPTED;
	}

	return interrupted;
}

void Thread::threadFunc(void *instance)
{
	((Thread*)instance)->process();
}

void Thread::process()
{
	if ( m_tss->getInstance()==NULL ) {
		m_tss->setInstance(this);
	}

	m_runnable->run();

	std::list<ACE_Event*> joiners;

	m_mutex.acquire();	
	m_alive = false;
	joiners = m_joiners;
	m_joiners.clear();
	m_mutex.release();

	notify();

	std::list<ACE_Event*>::iterator iter;
	for ( iter=joiners.begin(); iter!=joiners.end(); iter++ ) {
		(*iter)->signal();
	}
}
