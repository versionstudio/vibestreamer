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

#ifndef guard_httpserver_h
#define guard_httpserver_h

#include "configmanager.h"
#include "defaulthandler.h"
#include "eventbroadcaster.h"
#include "httpconnector.h"
#include "httprequesthandler.h"
#include "httpsessionmanager.h"
#include "singleton.h"

/**
* HttpServerListener.
* Contains event definitions for the HttpServer.
*/
class HttpServerListener
{
public:
	template<int I>	struct X { enum { TYPE = I };  };

	typedef X<0> Start;
	typedef X<1> Stop;

	/**
	* Fired when the HttpServer instance is started.
	*/
	virtual void on(Start) = 0;

	/**
	* Fired when the HttpServer instance is stopped.
	*/
	virtual void on(Stop) = 0;
};

/**
* HttpServer.
* Singleton web server class.
*/
class HttpServer : public Singleton<HttpServer>,
				   public EventBroadcaster<HttpServerListener>,
				   public ConfigManagerListener
{
public:
	/**
	* Default constructor.
	*/
	HttpServer() : m_connector(this),
		m_started(false) 
	{
		ConfigManager::getInstance()->addListener(this);
	}

	/**
	* Destructor.
	*/
	~HttpServer() 
	{
		std::list<HttpRequestHandler*>::iterator iterHandler;
		for ( iterHandler=m_requestHandlers.begin(); iterHandler!=m_requestHandlers.end(); iterHandler++ ) {
			delete *iterHandler;
		}

		ConfigManager::getInstance()->removeListener(this);
	}

	/**
	* Restart the http server.
	* @return true if http server was successfully restarted
	*/
	bool restart();

	/**
	* Start the http server.
	* @return true if http server was successfully started
	*/
	bool start();

	/**
	* Stop the http server.
	* @return true if http server was successfully stopped
	*/
	bool stop();

	/**
	* Get the default request handler.
	* @return the default request handler instance
	*/
	DefaultHandler& getDefaultRequestHandler() {
		return m_defaultHandler;
	}

	/**
	* Get all request handlers.
	* @return a collection of all request handlers
	*/
	std::list<HttpRequestHandler*>& getRequestHandlers() {
		return m_requestHandlers;
	}

	/**
	* Get the session manager.
	* @return the session manager instance
	*/
	HttpSessionManager& getSessionManager() {
		return m_sessionManager;
	}

	/**
	* Get whether the http server is started.
	* @return true if the http server is started
	*/
	bool isStarted() {
		return m_started;
	}

	/**
	* @override
	*/
	virtual void on(ConfigManagerListener::Load);

	/**
	* @override
	*/
	virtual void on(ConfigManagerListener::Save) {
		// not implemented
	}

private:
	HttpConnector m_connector;

	HttpSessionManager m_sessionManager;

	DefaultHandler m_defaultHandler;

	std::list<HttpRequestHandler*> m_requestHandlers;

	bool m_started;
};

#endif
