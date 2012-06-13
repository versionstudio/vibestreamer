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

#ifndef guard_databasemanager_h
#define guard_databasemanager_h

#include <ace/synch.h>

#include "configmanager.h"
#include "database.h"
#include "databasetask.h"
#include "mutexpool.h"
#include "singleton.h"

/**
* DatabaseManager.
* Singleton class that manages all databases and gives access to database connections.
*/
class DatabaseManager : public Singleton<DatabaseManager>,
						public ConfigManagerListener
{
public:
	/**
	* Default constructor.
	*/
	DatabaseManager() {
		ConfigManager::getInstance()->addListener(this);
		m_mutexPool.init(5); // create mutex pool (for locking individual databases)
	}

	/**
	* Destructor.
	*/
	~DatabaseManager() {
		ConfigManager::getInstance()->removeListener(this);
	}

	static const std::string DATABASE_INDEX;
	static const std::string DATABASE_SERVER;

	/**
	* Initialize and prepare the database manager for usage.
	* @return true if database manager was initialized successfully
	*/
	bool init();

	/**
	* Release a database connection back to the manager.
	* @param conn the database connection to release
	*/
	void releaseConnection(DatabaseConnection *conn);

	/**
	* Get a database connection to the database with the given name.
	* When a database connection is no longer it needed it must be
	* returned to the manager by a call to releaseConnection.
	* @param name the name of the database to retrieve a connection to
	* @param writeLock true if database should be write locked.
	* If a database connection is going to do UPDATE or INSERTs then it
	* must apply the write lock to prevent database errors or corruption.
	* The write lock will prevent any other requests for a write lock to
	* block until the write lock is released, which it is when the connection 
	* is returned to the manager using the releaseConnection method.
	* @return the connection to the database. NULL is returned
	* if no connection could be retrieved
	*/
	DatabaseConnection* getConnection(const std::string name,const bool writeLock = false);

	/**
	* Get a database connection to the database.
	* @param database the database to retrieve a connection to
	* @param writeLock true if database should be write locked
	* @return the connection to the database. NULL is returned
	* if no connection could be retrieved
	*/
	DatabaseConnection* getConnection(Database *database,const bool writeLock = false);

	/**
	* @override
	*/
	virtual void on(ConfigManagerListener::Load);

	/**
	* @override
	*/
	virtual void on(ConfigManagerListener::Save) {
		// not implemented
	}

private:
	/**
	* Acquire write lock on a database.
	* @param database the database to acquire write lock on
	*/
	void acquireWriteLock(Database *database);

	/**
	* Release any write lock on a database. This method
	* is always called when releasing a collection.
	* @param database the database to release any write lock on
	*/
	void releaseWriteLock(Database *database);

	/**
	* Find a database by name.
	* @param name the name of the database to get
	* @return the found database. NULL is returned if no database was found
	*/
	Database* findDatabaseByName(std::string name);

	/**
	* Tokenize the given SQL script into separate queries.
	* @param script the script to tokenize
	* @return the tokenized queries
	*/
	std::vector<std::string> DatabaseManager::tokenizeScript(const std::string &script);

	ACE_Mutex m_mutex;

	MutexPool m_mutexPool;

	std::list<Database> m_databases;
};

#endif
