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

#ifndef guard_contextprivate_h
#define guard_contextprivate_h

#include "../server/httpresponse.h"
#include "../server/httprequest.h"

#include "engine.h"

/**
* ContextPrivate.
* Used by each javascript context as the context private.
* Contains instances related to the origin of the script execution.
*/
class ContextPrivate
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param the http request
	* @param the http response
	* @return instance
	*/
	ContextPrivate(Engine *engine,HttpServerRequest &httpRequest,
		HttpServerResponse &httpResponse) : m_httpRequest(httpRequest),
			m_httpResponse(httpResponse)
	{
		m_engine = engine;
	}

	/**
	* Get the script engine in use.
	* @return the script engine in use
	*/
	Engine* getEngine() {
		return m_engine;
	}

	/**
	* Get the http response.
	* @return the http response
	*/
	HttpServerResponse& getHttpResponse() {
		return m_httpResponse;
	}

	/**
	* Get the http request.
	* @return the http request
	*/
	HttpServerRequest& getHttpRequest() {
		return m_httpRequest;
	}

private:
	Engine *m_engine;

	HttpServerResponse &m_httpResponse;
	HttpServerRequest &m_httpRequest;
};

#endif
