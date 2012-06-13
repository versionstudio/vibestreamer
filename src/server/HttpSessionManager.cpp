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
#include "httpsessionmanager.h"

#define LOGGER_CLASSNAME "HttpSessionManager"

#include "configmanager.h"
#include "databasemanager.h"
#include "logmanager.h"

bool HttpSessionManager::start()
{
	if ( m_started ) {
		return false;
	}

	m_maxSessions = ConfigManager::getInstance()->getInt(ConfigManager::HTTPSERVER_SESSIONMANAGER_MAXSESSIONS);
	m_sessionTimeout = ConfigManager::getInstance()->getInt(ConfigManager::HTTPSERVER_SESSIONMANAGER_SESSIONTIMEOUT);

	if ( !m_thread.start() ) {
		return false;
	}

	m_started = true;

	return true;
}

void HttpSessionManager::stop()
{
	if ( !m_started ) {
		return;
	}

	m_thread.cancel();
	m_thread.join();
	m_mutex.acquire();

	std::vector<HttpSession::Ptr> invalidatedSessions;

	// invalidate all connected sessions
	std::vector<HttpSession::Ptr>::iterator iter;
	for ( iter=m_sessions.begin(); iter!=m_sessions.end(); ) {
		invalidatedSessions.push_back(*iter);
		iter = m_sessions.erase(iter);
	}

	m_mutex.release();

	for ( iter=invalidatedSessions.begin(); iter!=invalidatedSessions.end(); iter++ ) 
	{
		deleteDbEntry(*iter);
		fireEvent(HttpSessionManagerListener::SessionRemoved(),*iter,
			HttpSessionManagerListener::SessionRemovedEventArgs::Reason::INVALIDATED);
	}

	m_started = false;
}

void HttpSessionManager::run()
{
	while ( true )
	{
		if ( m_thread.isCancelled() ) {
			break;
		}

		checkExpire();

		m_thread.sleep(m_sessionTimeout);
	}
}

void HttpSessionManager::referenceSession(HttpSession::Ptr sessionPtr)
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	sessionPtr->touch();
	sessionPtr->addReference();
}

void HttpSessionManager::dereferenceSession(HttpSession::Ptr sessionPtr)
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	sessionPtr->touch();
	sessionPtr->removeReference();
}

bool HttpSessionManager::addSession(HttpSession::Ptr sessionPtr)
{
	bool success = false;

	if ( prepareDbEntry(sessionPtr) )
	{
		m_mutex.acquire();

		if ( m_sessions.size()<m_maxSessions ) {
			sessionPtr->setManager(this);
			m_sessions.push_back(sessionPtr);
			success = true;
		}

		m_mutex.release();

		if ( success )
		{
			LogManager::getInstance()->info(LOGGER_CLASSNAME,
				"User \"%s\" (%s) logged on",sessionPtr->getPresentationName().c_str(),sessionPtr->getRemoteAddress().c_str());

			fireEvent(HttpSessionManagerListener::SessionAdded(),sessionPtr);
		}
	}

	return success;
}

void HttpSessionManager::kickSession(HttpSession::Ptr sessionPtr)
{
	bool success = false;

	m_mutex.acquire();

	std::vector<HttpSession::Ptr>::iterator iter;
	for ( iter=m_sessions.begin(); iter!=m_sessions.end(); iter++ )
	{
		if ( (*iter)->getGuid()==sessionPtr->getGuid() ) {
			m_sessions.erase(iter);
			success = true;
			break;
		}
	}

	m_mutex.release();

	if ( success ) 
	{
		deleteDbEntry(sessionPtr);

		LogManager::getInstance()->info(LOGGER_CLASSNAME,
			"User \"%s\" (%s) was kicked",sessionPtr->getPresentationName().c_str(),sessionPtr->getRemoteAddress().c_str());

		fireEvent(HttpSessionManagerListener::SessionRemoved(),sessionPtr,
			HttpSessionManagerListener::SessionRemovedEventArgs::Reason::KICKED);
	}
}

void HttpSessionManager::invalidateSession(HttpSession::Ptr sessionPtr)
{
	bool success = false;

	m_mutex.acquire();

	std::vector<HttpSession::Ptr>::iterator iter;
	for ( iter=m_sessions.begin(); iter!=m_sessions.end(); iter++ )
	{
		if ( (*iter)->getGuid()==sessionPtr->getGuid() ) {
			m_sessions.erase(iter);
			success = true;
			break;
		}
	}

	m_mutex.release();

	if ( success )
	{
		deleteDbEntry(sessionPtr);

		LogManager::getInstance()->info(LOGGER_CLASSNAME,
			"User \"%s\" (%s) logged out",sessionPtr->getPresentationName().c_str(),sessionPtr->getRemoteAddress().c_str());

		fireEvent(HttpSessionManagerListener::SessionRemoved(),sessionPtr,
			HttpSessionManagerListener::SessionRemovedEventArgs::Reason::INVALIDATED);
	}
}

HttpSession::Ptr HttpSessionManager::findSessionByGuid(std::string guid)
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	HttpSession::Ptr sessionPtr;

	std::vector<HttpSession::Ptr>::iterator iter;
	for ( iter=m_sessions.begin(); iter!=m_sessions.end(); iter++ ) {
		if ( (*iter)->getGuid()==guid ) {
			sessionPtr = *iter;
			break;
		}
	}

	return sessionPtr;
}

HttpSession::Ptr HttpSessionManager::matchSession(std::string guid,std::string remoteAddress)
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	HttpSession::Ptr sessionPtr;

	std::vector<HttpSession::Ptr>::iterator iter;
	for ( iter=m_sessions.begin(); iter!=m_sessions.end(); iter++ ) 
	{
		if ( (*iter)->getGuid()==guid && (*iter)->getRemoteAddress()==remoteAddress ) {
			sessionPtr = *iter;
			break;
		}
	}

	return sessionPtr;
}

void HttpSessionManager::checkExpire()
{
	time_t currentTime;
	time(&currentTime);

	std::vector<HttpSession::Ptr> expiredSessions;

	m_mutex.acquire();

	std::vector<HttpSession::Ptr>::iterator iter;
	for ( iter=m_sessions.begin(); iter!=m_sessions.end(); ) 
	{
		HttpSession::Ptr sessionPtr = *iter;

		if ( sessionPtr->getReferences()==0 
			&& difftime(currentTime,sessionPtr->getLastAccessedTime())>(m_sessionTimeout/1000) ) 
		{
			expiredSessions.push_back(sessionPtr);
			iter = m_sessions.erase(iter);
		}
		else {
			iter++;
		}
	}

	m_mutex.release();

	for ( iter=expiredSessions.begin(); iter!=expiredSessions.end(); iter++ )
	{
		HttpSession::Ptr sessionPtr = *iter;

		deleteDbEntry(sessionPtr);

		LogManager::getInstance()->info(LOGGER_CLASSNAME,
			"User \"%s\" (%s) timed out",sessionPtr->getPresentationName().c_str(),sessionPtr->getRemoteAddress().c_str());

		fireEvent(HttpSessionManagerListener::SessionRemoved(),sessionPtr,
			HttpSessionManagerListener::SessionRemovedEventArgs(HttpSessionManagerListener::SessionRemovedEventArgs::Reason::TIMEDOUT));
	}
}

bool HttpSessionManager::prepareDbEntry(HttpSession::Ptr sessionPtr)
{
	bool success = false;

	DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_SERVER,true);
	if ( conn!=NULL )
	{
		try	{
			conn->getSqliteConn().executenonquery("INSERT INTO [sessions] (guid) "
				"VALUES ('" + conn->quote(sessionPtr->getGuid()) + "')");

			sessionPtr->setDbId(conn->getSqliteConn().insertid());
			success = true;
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,
				"Could not create database entry for session '%s' [%s]",sessionPtr->getGuid().c_str(),ex.what());
		}

		DatabaseManager::getInstance()->releaseConnection(conn);
	}

	return success;
}

void HttpSessionManager::deleteDbEntry(HttpSession::Ptr sessionPtr)
{
	DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_SERVER,true);
	if ( conn!=NULL )
	{
		try {
			conn->getSqliteConn().executenonquery("DELETE FROM [sessions] WHERE sessionId=" 
				+ Util::ConvertUtil::toString(sessionPtr->getDbId()));
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,
				"Could not delete database entry for session '%s' [%s]",sessionPtr->getGuid().c_str(),ex.what());
		}

		DatabaseManager::getInstance()->releaseConnection(conn);
	}
}
