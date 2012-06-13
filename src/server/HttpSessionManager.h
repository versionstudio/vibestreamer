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

#ifndef guard_httpsessionmanager_h
#define guard_httpsessionmanager_h

#include <ace/synch.h>

#include "eventbroadcaster.h"
#include "httpsession.h"
#include "mutexpool.h"
#include "thread.h"

/**
* HttpSessionManagerListener.
* Abstract class containing event definitions for the HttpSessionManager class.
*/
class HttpSessionManagerListener
{
public:
	template<int I>	struct X { enum { TYPE = I };  };

	typedef X<0> SessionAdded;
	typedef X<1> SessionRemoved;

	struct SessionRemovedEventArgs
	{
		enum Reason {
			INVALIDATED,
			KICKED,
			TIMEDOUT
		};

		SessionRemovedEventArgs(Reason reason) {
			this->reason = reason;
		}

		Reason reason;
	};

	/**
	* Fired when a session is added to the session manager.
	* @param sessionPtr the session that was added to the session manager
	*/
	virtual void on(SessionAdded,HttpSession::Ptr sessionPtr) = 0;

	/**
	* Fired when a session is removed from the session manager.
	* @param sessionPtr the session that was removed from the session manager
	* @param e contains the reason for the session being removed
	*/
	virtual void on(SessionRemoved,HttpSession::Ptr sessionPtr,SessionRemovedEventArgs e) = 0;
};

/**
* HttpSessionManager.
* Singleton class that manages the connected sessions.
*/
class HttpSessionManager : public EventBroadcaster<HttpSessionManagerListener>,
						   public Runnable
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	HttpSessionManager() : 
		m_maxSessions(0),
		m_sessionTimeout(0),
		m_started(false),
		m_thread(this)
	{
		m_mutexPool.init(10); // create mutex pool (for locking individual sessions)
	}

	/**
	* Start the session manager service and prepare for incoming sessions.
	* @return true if service was started successfully
	*/
	bool start();

	/**
	* Stop the session manager service and kick any connected sessions.
	*/
	void stop();

	/**
	* @override
	*/
	void run();

	/**
	* Lock the given session.
	* Should only be used by a session instance for temporary locking.
	* @param session the session to lock
	*/
	void lockSession(HttpSession *session) {
		m_mutexPool.lock(session);
	}

	/**
	* Release lock on the given session.
	* Should only be used by a session instance for temporary locking.
	* @param session the session to release the lock on
	*/
	void releaseSession(HttpSession *session) {
		m_mutexPool.release(session);
	}

	/**
	* Reference the given session telling the session manager
	* that the session is currently in use and cannot be expired
	* due to inactivity until dereferenced.
	*/
	void referenceSession(HttpSession::Ptr sessionPtr);

	/**
	* Dereference the given session telling the session manager
	* that the session no longer is in use and can now be expired
	* due to inactivity.
	*/
	void dereferenceSession(HttpSession::Ptr sessionPtr);

	/**
	* Add session to the manager.
	* @param sessionPtr the session to add
	* @return true if the session was added
	*/
	bool addSession(HttpSession::Ptr sessionPtr);

	/**
	* Kick session from the manager.
	* @param sessionPtr the session to kick
	*/
	void kickSession(HttpSession::Ptr sessionPtr);

	/**
	* Invalidate session.
	* This method logs out the session.
	* @param sessionPtr the session to invalidate
	*/
	void invalidateSession(HttpSession::Ptr sessionPtr);

	/**
	* Get a session by guid.
	* @param guid the guid of the session to look for
	* @return the found session. Ptr will be NULL if no session was found.
	*/
	HttpSession::Ptr findSessionByGuid(std::string guid);

	/**
	* Match the given information with a connected session.
	* All given parameters must be a match.
	* @param guid the session guid
	* @param remoteAddress the remote address of the client owning the session
	* @return the matching session. Ptr will be NULL if no session could be matched.
	*/
	HttpSession::Ptr matchSession(std::string guid,std::string remoteAddress);

	/**
	* Get the number of connected sessions.
	* @return the number of connected sessions
	*/
	const size_t getSessionCount() {
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex);
		return m_sessions.size();
	}

	/**
	* Get all connected sessions.
	* @return a collection of all connected sessions
	*/
	std::vector<HttpSession::Ptr> getSessions() {
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex);
		return m_sessions;
	}

private:
	/**
	* Check for expired sessions.
	*/
	void checkExpire();

	/**
	* Prepare a database entry for the given session.
	* @param sessionPtr the session to prepare a database entry for
	* @return true if the database entry was successfully prepared
	*/
	bool prepareDbEntry(HttpSession::Ptr sessionPtr);

	/**
	* Delete the database entry for the given session
	* @param user the user to delete the database entry for
	*/
	void deleteDbEntry(const HttpSession::Ptr sessionPtr);

	ACE_Mutex m_mutex;

	Thread m_thread;

	MutexPool m_mutexPool;

	std::vector<HttpSession::Ptr> m_sessions;

	int m_maxSessions;
	int m_sessionTimeout;

	bool m_started;
};

#endif
