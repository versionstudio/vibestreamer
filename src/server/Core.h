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

#ifndef guard_core_h
#define guard_core_h

/**
* Core.
* This is where it all begins.
*/
class Core
{
public:
	/**
	* Initialize the core. This must be run first.
	* This method creates and initializes all instances handled by the core.
	*/
	static void init();

	/**
	* Startup the core.
	* Run after initializing in order to startup the core.
	* This loads all configurations and starts upp all instances handled by the core.
	* @param callback an optional callback method for handling startup status
	* @param arg an optional argument to be sent to the callback method
	* @return zero if startup was successfull. Anything above zero indicates a startup failure
	*/
	static int startup(void (*callback)(void*,const std::string&)=NULL,void *arg=NULL);

	/**
	* Cleanup the core.
	* Run this to shutdown and clean up the core.
	* This saves all configurations and cleans up all instances handled by the core.
	* @param callback an optional callback method for handling cleanup status
	* @param param an optional parameter to be sent to the callback method
	* @return zero if cleanup was successfull. Anything above zero indicates a cleanup failure
	*/
	static int cleanup(void (*callback)(void*,const std::string&)=NULL,void *param=NULL);
};

#endif
