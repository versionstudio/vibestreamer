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
#include "taskrunner.h"

#define LOGGER_CLASSNAME "TaskRunner"

#include <ace/high_res_timer.h>

bool TaskRunner::start()
{
	if ( m_started ) {
		return false;
	}

	if ( !m_thread.start() ) {
		return false;
	}

	m_started = true;

	return true;
}

bool TaskRunner::stop(bool graceful)
{
	if ( !m_started ) {
		return false;
	}

	m_thread.cancel();
	m_thread.join();

	ACE_Time_Value currentTime = ACE_High_Res_Timer::gettimeofday();

	while ( !m_queue.empty() ) 
	{
		Task *task = popQueue();
		if ( graceful && currentTime>= task->getNextExecutionTime() ) {
			task->run();
		}
		delete task;
	}

	m_started = false;

	return true;
}

void TaskRunner::run()
{
	while ( true )
	{
		if ( m_thread.isCancelled() ) {
			break;
		}

		Task *task = popQueue();
		if ( task!=NULL )
		{
			ACE_Time_Value currentTime = ACE_High_Res_Timer::gettimeofday();
			ACE_Time_Value nextExecutionTime = task->getNextExecutionTime();

			if ( currentTime>=nextExecutionTime ) 
			{
				task->run();
				if ( task->getPeriod()>0 ) {
					reschedule(task);
				}
				else {
					delete task;
				}
			}
			else 
			{
				ACE_Time_Value waitTime = nextExecutionTime-currentTime;
				m_thread.wait(waitTime.msec());
				putQueue(task);
			}
		}
		else {
			m_thread.wait();
		}
	}
}

void TaskRunner::schedule(Task *task)
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	task->setNextExecutionTime(ACE_High_Res_Timer::gettimeofday()+
		ACE_Time_Value(task->getDelay(),0));

	putQueue(task);

	m_thread.notify();
}

void TaskRunner::schedule(Task *task,unsigned long delay,unsigned long period)
{
	task->setDelay(delay);
	task->setPeriod(period);

	schedule(task);
}

Task* TaskRunner::popQueue()
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	Task *task = NULL;

	if ( !m_queue.empty() ) {
		task = m_queue.front();
		m_queue.pop_front();
	}

	return task;
}

void TaskRunner::putQueue(Task *task)
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	std::list<Task*>::iterator iter;
	for ( iter=m_queue.begin(); iter!=m_queue.end(); iter++ )
	{
		if ( (*iter)->getNextExecutionTime()>task->getNextExecutionTime() ) {
			m_queue.insert(iter,task);
			return;
		}
	}

	m_queue.push_back(task);
}

void TaskRunner::reschedule(Task *task)
{
	task->setNextExecutionTime(ACE_High_Res_Timer::gettimeofday()+
		ACE_Time_Value(task->getPeriod(),0));

	putQueue(task);
}
