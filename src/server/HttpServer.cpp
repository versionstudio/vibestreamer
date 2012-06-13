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

#include "common.h"
#include "httpserver.h"

#define LOGGER_CLASSNAME "HttpServer"

#include "jshandler.h"
#include "logmanager.h"
#include "sharehandler.h"

bool HttpServer::restart()
{
	return stop() && start();
}

bool HttpServer::start()
{
	if ( m_started ) {
		return false;
	}

	m_started = true;

	std::string errorReason;
	if ( !m_sessionManager.start() ) {
		errorReason = "Invalid session manager";
	}
	else
	{
		// init default request handler
		if ( !m_defaultHandler.init() ) {
			errorReason = "Invalid handler";
		}
		else
		{
			// init request handlers
			std::list<HttpRequestHandler*>::iterator iter;
			for ( iter=m_requestHandlers.begin(); iter!=m_requestHandlers.end(); iter++ ) {
				if ( !(*iter)->init() ) {
					errorReason = "Invalid handler";
					break;
				}
			}
			
			if ( errorReason.empty() )
			{
				if ( !m_connector.start() ) {
					errorReason = "Invalid connector";
				}
			}
		}
	}

	if ( errorReason.empty() )
	{
		std::string serverAddress = m_connector.getServerAddress();
		if ( serverAddress.empty() ) {
			serverAddress = "(All unassigned)";
		}

		int serverPort = m_connector.getServerPort();

		if ( m_connector.isSslEnabled() ) {
			LogManager::getInstance()->info(LOGGER_CLASSNAME,"Server started. IP address: %s, Port: %d, SSL enabled",
				serverAddress.c_str(),serverPort);
		}
		else
		{
			LogManager::getInstance()->info(LOGGER_CLASSNAME,"Server started. IP address: %s, Port: %d",
				serverAddress.c_str(),serverPort);
		}

		fireEvent(HttpServerListener::Start());
		return true;
	}
	else
	{
		LogManager::getInstance()->info(LOGGER_CLASSNAME,"Server could not be started - %s",errorReason.c_str());
		stop();
		return false;
	}
}

bool HttpServer::stop()
{
	if ( !m_started ) {
		return false;
	}

	m_connector.stop();
	m_sessionManager.stop();

	// cleanup request handlers
	std::list<HttpRequestHandler*>::iterator iter;
	for ( iter=m_requestHandlers.begin(); iter!=m_requestHandlers.end(); iter++ ) {
		(*iter)->cleanup();
	}

	// cleanup default request handler
	m_defaultHandler.cleanup();

	m_started = false;

	fireEvent(HttpServerListener::Stop());
	LogManager::getInstance()->info(LOGGER_CLASSNAME,"Server stopped");

	return true;
}

void HttpServer::on(ConfigManagerListener::Load)
{
	TiXmlElement requestHandlersElement = ConfigManager::getInstance()->getElement(ConfigManager::HTTPSERVER_REQUESTHANDLERS);
	TiXmlNode *requestHandlerNode = requestHandlersElement.FirstChildElement("requestHandler");
	while ( requestHandlerNode!=NULL )
	{
		std::string handler;
		std::string urlPattern;

		TiXmlNode *node = NULL;

		node = requestHandlerNode->FirstChild("handler");
		if ( node!=NULL && node->FirstChild()!=NULL ) {
			handler = node->FirstChild()->Value();
		}

		node = requestHandlerNode->FirstChild("urlPattern");
		if ( node!=NULL && node->FirstChild()!=NULL ) {
			urlPattern = node->FirstChild()->Value();
		}

		boost::regex urlPatternRegex(urlPattern);

		HttpRequestHandler* requestHandler = NULL;

		if ( handler=="ShareHandler" ) {
			requestHandler = new ShareHandler(urlPatternRegex);
		}
		else if ( handler=="JsHandler" ) {
			requestHandler = new JsHandler(urlPatternRegex);
		}

		if ( requestHandler!=NULL ) {
			m_requestHandlers.push_back(requestHandler);
		}

		requestHandlerNode = requestHandlersElement.IterateChildren("requestHandler",requestHandlerNode);
	}
}
