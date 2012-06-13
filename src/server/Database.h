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

#ifndef guard_database_h
#define guard_database_h

#include "../sqlite3x/sqlite3x.hpp"

class Database; // forward declaration

/**
* DatabaseConnection.
* Represents a database connection and wraps together a database
* together with an sqlite connection.
*/
class DatabaseConnection
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param database the database this connection is linked to
	* @return instance
	*/
	DatabaseConnection(Database *database) {
		m_database = database;
	}

	static const int BUSYTIMEOUT;

	/**
	* Get the database this connection is linked to.
	* @return the database this connection is linked to
	*/
	Database* getDatabase() {
		return m_database;
	}

	/**
	* Get the sqlite connection.
	* @return the sqlite connection
	*/
	sqlite3x::sqlite3_connection& getSqliteConn() {
		return m_sqliteConn;
	}

	/**
	* Quote a string, ensuring that any quote
	* characters are made safe for usage in database queries.
	* @param s the string to quote
	* @return the database quoted string
	*/
	std::string quote(const std::string &s) {
		return boost::replace_all_copy(s,"'","''");
	}

	/**
	* Quote a wide string, ensuring that any quote
	* characters are made safe for usage in database queries.
	* @param s the wide string to quote
	* @return the database quoted wide string
	*/
	std::wstring quote(const std::wstring &s) {
		return boost::replace_all_copy(s,"'","''");
	}

private:
	Database *m_database;

	sqlite3x::sqlite3_connection m_sqliteConn;
};

/**
* Database.
* Represents an sqlite database.
*/
class Database
{
public:
	/**
	* Constructor.
	* @return instance
	*/
	Database() : m_synchronous(false),
		m_writeLocked(false) 
	{

	}

	/**
	* Destructor.
	*/
	~Database() {
		std::list<DatabaseConnection*>::iterator iter;
		for ( iter=m_connections.begin(); iter!=m_connections.end(); iter++ ) {
			delete *iter;
		}
	}

	/**
	* Create a new database connection.
	* The newly created connection will not be idle and must be returned
	* to the database after usage.
	* @return the database connection or NULL if no connection could be created
	*/
	DatabaseConnection* newConnection();

	/**
	* Push a database connection to the idle pool.
	* @param conn the database connection to push to the pool
	*/
	void pushConnection(DatabaseConnection *conn);

	/**
	* Pop a database connection from the idle pool.
	* @return the database connection or NULL if no connections are available
	*/
	DatabaseConnection* popConnection();

	/**
	* Get the name of the database.
	* @return the name of the database
	*/
	const std::string& getName() const {
		return m_name;
	}

	/**
	* Get the path to the database file
	* @return the path to the database file
	*/
	const std::string& getPath() const {
		return m_path;
	}

	/**
	* Get whether the database is synchrous or not.
	* See sqlite documentation for details.
	* @return true if the database is synchronous
	*/
	const bool isSynchronous() const {
		return m_synchronous;
	}

	/**
	* Get whether the database write locked.
	* @return true if the database is write locked
	*/
	const bool isWriteLocked() const {
		return m_writeLocked;
	}

	/**
	* Set the name of the database.
	* @param name the name of the database
	*/
	void setName(std::string name) {
		m_name = name;
	}

	/**
	* Set the path to the database file.
	* @param path the path to the database file
	*/
	void setPath(std::string path) {
		m_path = path;
	}

	/**
	* Set whether the database should be synchronous.
	* See the sqlite documentation for details.
	* @param synchronous true if the database is synchronous
	*/
	void setSynchronous(bool synchronous) {
		m_synchronous = synchronous;
	}

	/**
	* Set whether the database should be write locked.
	* A write locked database means that it is locked both for reading and writing.
	* Write locking the databases are handled by the DatabaseManager.
	* @param writeLocked true if the database is write locked
	*/
	void setWriteLocked(bool writeLocked) {
		m_writeLocked = writeLocked;
	}

private:
	std::list<DatabaseConnection*> m_connections;

	std::stack<DatabaseConnection*> m_idleConnections;

	std::string m_name;
	std::string m_path;

	bool m_synchronous;
	bool m_writeLocked;
};


#endif
