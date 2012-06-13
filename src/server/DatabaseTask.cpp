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
#include "databasetask.h"

#define LOGGER_CLASSNAME "DatabaseTask"

#include "databasemanager.h"
#include "logmanager.h"

void DatabaseTask::run()
{
	DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(m_name,m_writeLock);
	if ( conn!=NULL )
	{
		try {
			conn->getSqliteConn().executenonquery(m_query);
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to execute query [%s]",ex.what());
		}

		DatabaseManager::getInstance()->releaseConnection(conn);
	}
}
