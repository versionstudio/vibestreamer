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
#include "jshandler.h"

#define LOGGER_CLASSNAME "JsHandler"

#include "logmanager.h"

const std::string JsHandler::DEFAULT_MIME_TYPE = "text/html;charset=utf-8";

bool JsHandler::init() 
{
	return m_engine.init();
}

void JsHandler::cleanup() 
{
	m_engine.cleanup();
}

bool JsHandler::handleRequest(HttpWorker *worker,
	HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{
	if ( LogManager::getInstance()->isDebug() ) {
		LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Handling \"%s\"",httpRequest.getUri().c_str());
	}

	std::string path = httpRequest.getRealPath();
	FILE *file = fopen(path.c_str(),"rb");
	if ( file!=NULL )
	{
		// disable client cache
		if ( !httpRequest.getParameter("allowcaching",NULL) )
		{
			tm localTime;
			Util::TimeUtil::getLocalTime(&localTime);
			std::string modifiedTime = Util::TimeUtil::format(localTime,"%a, %d %b %Y %H:%M:%S GMT");

			httpResponse.setHeader("Cache-Control","no-store,no-cache,must-revalidate,max-age=0");
			httpResponse.setHeader("Pragma","no-cache");
			httpResponse.setHeader("Last-Modified",modifiedTime);
			httpResponse.setHeader("Expires","-1000");
		}

		httpResponse.setContentType(DEFAULT_MIME_TYPE);
		httpResponse.setHeader("Connection","close");

		// run script file through engine
		if ( !m_engine.executeFile(file,httpRequest,httpResponse) ) {
			httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_INTERNAL_SERVER_ERROR);
		}

		httpResponse.flush();
		
		fclose(file);

		return true;
	}
	else  {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_NOT_FOUND);
	}

	return false;
}
