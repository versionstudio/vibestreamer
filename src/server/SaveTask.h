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

#ifndef guard_savetask_h
#define guard_savetask_h

#include "persistentmanager.h"
#include "taskrunner.h"

/**
* SaveTask.
* Task class used for saving a persistent manager.
*/
class SaveTask : public Task
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param persistentManager the persistent manager to save
	* @return instance
	*/
	SaveTask(PersistentManager *persistentManager) {
		m_persistentManager = persistentManager;
	}

	/**
	* @override
	*/
	virtual void run();

private:
	PersistentManager *m_persistentManager;
};

#endif
