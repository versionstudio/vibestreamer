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
#include "database.h"

#define LOGGER_CLASSNAME "Database"

#include "logmanager.h"

const int DatabaseConnection::BUSYTIMEOUT = 30000;

DatabaseConnection* Database::newConnection()
{
	DatabaseConnection *conn = new DatabaseConnection(this);

	try
	{
		conn->getSqliteConn().open(m_path.c_str());
		conn->getSqliteConn().setbusytimeout(DatabaseConnection::BUSYTIMEOUT);
		m_connections.push_back(conn);
	}
	catch(exception &ex) {
		delete conn;
		conn = NULL;
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,
			"Could not create connection to database '%s' [%s]",m_name.c_str(),ex.what());
	}

	return conn;
}

void Database::pushConnection(DatabaseConnection *conn) 
{
	m_idleConnections.push(conn);
}

DatabaseConnection* Database::popConnection() 
{
	DatabaseConnection *conn = NULL;

	if ( !m_idleConnections.empty() ) {
		conn = m_idleConnections.top();
		m_idleConnections.pop();
	}
	
	return conn;		
}
