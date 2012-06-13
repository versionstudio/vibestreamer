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

#ifndef guard_mutexpool_h
#define guard_mutexpool_h

#include <ace/mutex.h>

/**
* MutexPool.
* A class representing a pool of allocated mutexes that can be reused
* for more optimized mutex locking.
*/
class MutexPool
{
public:
	/**
	* Default construcotor.
	* @return instance
	*/
	MutexPool() : m_size(0) {

	}

	/**
	* Destructor.
	*/
	~MutexPool() {
		dispose();
	}

	/**
	* Initialize the pool and allocate the given number of indexes.
	* Any mutexes already allocated in the pool will be disposed of before
	* the new ones are allocated.
	* @param size the number of mutexes that should be allocated for the pool.
	*/
	void init(int size) 
	{
		dispose();

		m_size = size;

		if ( m_size>0 ) {
			m_mutexes = new ACE_Mutex*[m_size+1];
			for ( int i=0; i<m_size; i++ ) {
				m_mutexes[i] = new ACE_Mutex();
			}
		}
	}

	/**
	* Dispose of all allocated mutexes in the pool.
	*/
	void dispose() 
	{
		if ( m_size>0 ) {
			for ( int i=0; i<m_size; i++ ) {
				delete m_mutexes[i];
			}
			delete[] m_mutexes;
		}
	}

	/**
	* Lock the given address through a mutex in the pool.
	* An address that has been locked must be released when done with 
	* through the release method in order to prevent any deadlocks.
	* @param address the memory address to lock towards
	*/
	void lock(void *address)
	{
		if ( m_size>0 ) {
			int index = (int)((unsigned long)address % m_size);
			m_mutexes[index]->acquire();
		}
	}

	/**
	* Release the mutex locking the given address.
	*/
	void release(void *address)
	{
		if ( m_size>0 ) {
			int index = (int)((unsigned long)address % m_size);
			m_mutexes[index]->release();
		}
	}

private:
	ACE_Mutex** m_mutexes;

	int m_size;
};

#endif
