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

#ifndef guard_eventbroadcaster_h
#define guard_eventbroadcaster_h

#include <ace/synch.h>
#include <algorithm>

/**
* EventBroadcaster.
* A class can inherit from this class to become an event broadcaster.
*/
template<typename EventListener>
class EventBroadcaster
{
public:
	/**
	* Add an event listener.
	* @param listener the event listener instance
	*/
	void addListener(EventListener *listener) 
	{
		ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

		if ( std::find(m_listeners.begin(),m_listeners.end(),listener)==m_listeners.end() ) {
			m_listeners.push_back(listener);
		}
	}

	/**
	* Remove an event listener.
	* @param listener the event listener instance
	*/
	void removeListener(EventListener *listener) 
	{
		ACE_Write_Guard<ACE_Mutex> guard(m_mutex); 

		std::list<EventListener*>::iterator iter = find(m_listeners.begin(),m_listeners.end(),listener);
		if ( iter!=m_listeners.end() ) {
			m_listeners.erase(iter);
		}
	}

protected:
	/**
	* Fire an event.
	* @param type the event type
	*/
	template<typename TO>
	void fireEvent(TO type)
	{
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex); 
		
		std::list<EventListener*>::iterator iter;
		for ( iter=m_listeners.begin(); iter!=m_listeners.end(); iter++ ) {
			(*iter)->on(type);
		}
	}

	/**
	* Fire an event.
	* @param type the event type
	* @param p1 event parameter
	*/
	template<typename TO,class T1>
	void fireEvent(TO type,const T1 &p1)
	{
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex); 	

		std::list<EventListener*>::iterator iter;
		for ( iter=m_listeners.begin(); iter!=m_listeners.end(); iter++ ) {
			(*iter)->on(type,p1);
		}
	}

	/**
	* Fire an event.
	* @param type the event type
	* @param p1 event parameter
	* @param p2 event parameter
	*/
	template<typename TO,class T1,class T2>
	void fireEvent(TO type,const T1 &p1,const T2 &p2)
	{
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex); 	

		std::list<EventListener*>::iterator iter;
		for ( iter=m_listeners.begin(); iter!=m_listeners.end(); iter++ ) {
			(*iter)->on(type,p1,p2);
		}
	}

	/**
	* Fire an event.
	* @param type the event type
	* @param p1 event parameter
	* @param p2 event parameter
	* @param p3 event parameter
	*/
	template<typename TO,class T1,class T2,class T3>
	void fireEvent(TO type,const T1 &p1,const T2 &p2,const T3 &p3)
	{
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex); 	

		std::list<EventListener*>::iterator iter;
		for ( iter=m_listeners.begin(); iter!=m_listeners.end(); iter++ ) {
			(*iter)->on(type,p1,p2,p3);
		}
	}

	/**
	* Fire an event.
	* @param type the event type
	* @param p1 event parameter
	* @param p2 event parameter
	* @param p3 event parameter
	* @param p4 event parameter
	*/
	template<typename TO,class T1,class T2,class T3,class T4>
	void fireEvent(TO type,const T1 &p1,const T2 &p2,const T3 &p3,const T4 &p4)
	{
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex); 	

		std::list<EventListener*>::iterator iter;
		for ( iter=m_listeners.begin(); iter!=m_listeners.end(); iter++ ) {
			(*iter)->on(type,p1,p2,p3,p4);
		}
	}

private:
	ACE_Mutex m_mutex;

	std::list<EventListener*> m_listeners;
};

#endif
