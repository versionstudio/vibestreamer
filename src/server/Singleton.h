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

#ifndef guard_singleton_h
#define guard_singleton_h

/**
* Singleton.
* Base class for all singletons.
*/
template<typename T>
class Singleton {
public:
	/**
	* Default constructor.
	*/
	Singleton() { 
	
	}

	/**
	* Virtual destructor.
	*/
	virtual ~Singleton() { }

	/**
	* Get the singleton instance.
	* @return the singleton instance
	*/
	static T* getInstance() {
		return instance;
	}
	
	/**
	* Create the singleton instance. Must be run 
	* before accessing the singleton.
	*/
	static void newInstance() {
		deleteInstance();
		instance = new T();
	}
	
	/**
	* Destroy the singleton instance. Should be run when singleton
	* is no longer needed.
	*/
	static void deleteInstance() 
	{
		if ( instance ) {
			delete instance;
		}

		instance = NULL;
	}

protected:
	static T* instance;

private:
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);
};

template<class T> T* Singleton<T>::instance = NULL;

#endif
