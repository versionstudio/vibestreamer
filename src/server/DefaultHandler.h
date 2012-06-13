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

#ifndef guard_defaulthandler_h
#define guard_defaulthandler_h

#include "httprequesthandler.h"

/**
* DefaultHandler.
* Handles all requests that don't get handled by any other request handler.
*/
class DefaultHandler : public HttpRequestHandler
{
public:
	static const std::string DEFAULT_MIME_TYPE;

	static const int IO_BUFFER_SIZE;

	/**
	* @override
	*/
	virtual bool init() { return true; }

	/**
	* @override
	*/
	virtual void cleanup() {}

	/**
	* @override
	*/
	virtual bool handleRequest(HttpWorker *worker,
		HttpServerRequest &httpRequest,HttpServerResponse &httpResponse);

private:
	/**
	 * Handle a file response.
	 * @param worker the worker handling the request
	 * @param httpRequest the request
	 * @param httpResponse the response
	 * @return true if the request was successfully handled
	 */
	bool handleFileResponse(HttpWorker *worker,
		HttpServerRequest &httpRequest,HttpServerResponse &httpResponse);

	/**
	 * Handle a redirect response.
	 * @param worker the worker handling the request
	 * @param httpRequest the request
	 * @param httpResponse the response
	 * @return true if the request was successfully handled
	 */
	bool handleRedirectResponse(HttpWorker *worker,
		HttpServerRequest &httpRequest,HttpServerResponse &httpResponse);

	/**
	 * Handle an error response.
	 * @param worker the worker handling the request
	 * @param httpRequest the request
	 * @param httpResponse the response
	 * @return true if the request was successfully handled
	 */
	bool handleErrorResponse(HttpWorker *worker,
		HttpServerRequest &httpRequest,HttpServerResponse &httpResponse);
};

#endif
