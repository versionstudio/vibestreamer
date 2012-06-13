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

#ifndef guard_httprequesthandler_h
#define guard_httprequesthandler_h

#include "httpresponse.h"
#include "httprequest.h"
#include "httpworker.h"

/**
* HttpRequestHandler.
* Abstract base class for all request handlers.
*/
class HttpRequestHandler
{
public:
	static const std::string DEFAULT_MIME_TYPE;

	/**
	 * Initialize the request handler. Subclasses should override this method.
	 * @return true if initialization was successful.
	 */
	virtual bool init() = 0;

	/**
	 * Clean up the request handler. Subclasses should override this method.
	 */
	virtual void cleanup() = 0;

	/**
	 * Handle a request. Subclasses should override this method.
	 * @param worker the worker handling the request
	 * @param httpRequest the request
	 * @param httpResponse the response
	 * @return true if the request could be handled successfully without an error
	 */
	virtual bool handleRequest(HttpWorker *worker,
		HttpServerRequest &httpRequest,HttpServerResponse &httpResponse) = 0;

	/**
	 * Get the url pattern regular expression.
	 * @return the url pattern regular expression
	 */
	const boost::regex& getUrlPatternRegex() const {
		return m_urlPatternRegex;
	}

protected:
	boost::regex m_urlPatternRegex;
};

#endif
