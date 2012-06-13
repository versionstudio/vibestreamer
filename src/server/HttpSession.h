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

#ifndef guard_httpsession_h
#define guard_httpsession_h

#include <boost/shared_ptr.hpp>

class HttpSessionManager; // forward declaration

/**
* HttpSession.
* Represents a connected session on a HttpServer.
*/
class HttpSession
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param userGuid the guid of the user logged on as this session
	* @param userAgent the user agent user by the client connected to this session
	* @param remoteAddress the remote address of the client connected to this session
	*/
	HttpSession(std::string userGuid,
		std::string userAgent,std::string remoteAddress) : m_dbId(0),
		m_manager(NULL),
		m_references(0)
	{
		m_userGuid = userGuid;
		m_userAgent = userAgent;
		m_remoteAddress = remoteAddress;

		m_guid = Util::CryptoUtil::generateGuid();

		m_creationTime = Util::TimeUtil::getCalendarTime();

		touch();
	}

	typedef boost::shared_ptr<HttpSession> Ptr;

	/**
	* Add a reference to the session.
	*/
	void addReference() {
		m_references++;
	}

	/**
	* Remove the attribute with the given name.
	* This method temporary locks the session using the
	* mutex pool owned by the manager.
	* @param name the attribute to remove
	*/
	void removeAttribute(std::string name);

	/**
	* Remove a reference to the session.
	*/
	void removeReference() {
		m_references--;
	}

	/**
	* Touch the session to update the sessions last accessed time.
	* This prevents the session from timeout.
	*/
	void touch() {
		m_lastAccessedTime = Util::TimeUtil::getCalendarTime();
	}

	/**
	* Get the attribute with the given name.
	* This method temporary locks the session using the
	* mutex pool owned by the manager.
	* @param name the attribute to retrieve. The name is case sensitive
	* @param value out parameter for the found attribute value
	* @return true if the attribute was found
	*/
	bool getAttribute(std::string name,std::string *value);

	/**
	* Get the time when this session was created.
	* @return the time when this session was created
	*/
	const time_t getCreationTime() { 
		return m_creationTime; 
	}

	/**
	* Get the database id.
	* @return the database id
	*/
	const uint64_t getDbId() const { 
		return m_dbId;
	}

	/**
	* Get the session guid.
	* @return the session guid
	*/
	const std::string getGuid() { 
		return m_guid; 
	}

    /**
     * Get the manager within which this session is valid.
	 * @return the manager within which this session is valid
     */
	HttpSessionManager* getManager() {
		return m_manager;
	}

	/**
	* Get the last time the session was accessed.
	* @return the last time the session was accessed
	*/
	const time_t getLastAccessedTime() { 
		return m_lastAccessedTime; 
	}

	/**
	* Get the presentation name for the sesssion.
	* If a valid user is associated with the session it will be the users
	* name, otherwise the session guid.
	* @return the presentation name for the session
	*/
	const std::string getPresentationName() const;

	/**
	* Get the number of references there are to the session.
	* As long as there are more than zero references the session
	* is not allowed to expire.
	*/
	const getReferences() const {
		return m_references;
	}

	/**
	* Get the remote address of the client owning this session.
	* @return the remote address of the client owning this session
	*/
	const std::string& getRemoteAddress() { 
		return m_remoteAddress; 
	}

	/**
	* Get the user agent of the client owning this session.
	* @return the user agent of the client owning this session
	*/
	const std::string& getUserAgent() {
		return m_userAgent;
	}

	/**
	* Get the user guid of the user logged on as this session.
	* @return the user guid of the user logged on as this session
	*/
	const std::string getUserGuid() const {
		return m_userGuid;
	}

	/**
	* Set the attribute with the given name.
	* This method temporary locks the session using the
	* mutex pool owned by the manager.
	* @param the header name
	* @param the header value
	*/
	void setAttribute(std::string name,std::string value);

	/**
	* Set the database id.
	* @param dbId the database id
	*/
	void setDbId(uint64_t dbId) {
		m_dbId = dbId;
	}

    /**
     * Set the manager within which this session is valid.
	 * @param manager the manager within which this session is valid
     */
	void setManager(HttpSessionManager *manager) {
		m_manager = manager;
	}

private:
	HttpSessionManager *m_manager;

	std::map<std::string,std::string> m_attributes;

	std::string m_guid;
	std::string m_remoteAddress;
	std::string m_userAgent;
	std::string m_userGuid;

	time_t m_creationTime;
	time_t m_lastAccessedTime;

	uint64_t m_dbId;

	unsigned int m_references;
};

#endif
