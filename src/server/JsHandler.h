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

#ifndef guard_jshandler_h
#define guard_jshandler_h

#include "../jsengine/engine.h"

#include "httprequesthandler.h"

/**
* JsHandler.
* Handles all requests to script pages.
* The requested pages are parsed and executed through the js script engine.
*/
class JsHandler : public HttpRequestHandler
{
public:
	/**
	 * Default constructor.
	 * @param urlPatternRegex the url pattern regular expression
	 * @return instance
	 */
	JsHandler(boost::regex urlPatternRegex) {
		m_urlPatternRegex = urlPatternRegex;
	}

	static const std::string DEFAULT_MIME_TYPE;

	/**
	 * @override
	 */
	virtual bool init();

	/**
	 * @override
	 */
	virtual void cleanup();

	/**
	 * @override
	 */
	virtual bool handleRequest(HttpWorker *worker,
		HttpServerRequest &httpRequest,HttpServerResponse &httpResponse);
private:
	Engine m_engine;
};

#endif
