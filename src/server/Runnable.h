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

#ifndef guard_runnable_h
#define guard_runnable_h

/**
* Runnable.
* This abstract class should be implemented by any class whose instances are intended 
* to be executed by a thread or task runner.
*/
class Runnable
{
public:
	/**
	* Runnable method called by either a thread or task runner.
	* Sub classes should override this method.
	*/
	virtual void run() = 0;

private:

};

#endif
