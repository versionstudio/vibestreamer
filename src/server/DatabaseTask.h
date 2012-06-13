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

#ifndef guard_databasetask_h
#define guard_databasetask_h

#include "taskrunner.h"

/**
* DatabaseTask.
* Task class used for executing database queries as a background task.
*/
class DatabaseTask : public Task
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param name the name of the database to connect and query towards
	* @param query the database query
	* @param writeLock true if the used database connection should be write locked
	* @return instance
	*/
	DatabaseTask(std::string name,std::string query,const bool writeLock = false) {
		m_name = name;
		m_query = query;
		m_writeLock = writeLock;
	}

	/**
	* @override
	*/
	virtual void run();

private:
	std::string m_name;
	std::string m_query;

	bool m_writeLock;
};

#endif
