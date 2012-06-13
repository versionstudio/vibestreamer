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

#ifndef guard_accesslogger_h
#define guard_accesslogger_h

#include <ace/synch.h>

class HttpServerResponse; // forward declaration
class HttpServerRequest; // forward declaration

/**
* AccessLogger.
* Used by a site instance in order to handle access logging per request.
*/
class AccessLogger
{
public:
	/**
	 * Constructor used for creating a new instance.
	 * @param path the path to the access log file
	 * The path can include any format specifiers supported by strftime()
	 * @return instance
	 */
	AccessLogger(std::string path) {
		m_path = path;
	}

	/**
	 * Log an entry to the access log
	 * @param httpRequest the request associated with the logging
	 * @param httpResponse the http response associated with the logging
	 */
	void log(HttpServerRequest *httpRequest,HttpServerResponse *httpResponse);

	/**
	 * Get the path of the access log.
	 * @return the path of the access log
	 */
	const std::string getPath() const {
		return m_path;
	}

private:
	ACE_Mutex m_mutex;

	std::string m_path;
};

#endif
