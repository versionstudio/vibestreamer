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
#include "defaulthandler.h"

#define LOGGER_CLASSNAME "DefaultHandler"

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/path.hpp>

#include "configmanager.h"
#include "httpconnector.h"
#include "logmanager.h"

const std::string DefaultHandler::DEFAULT_MIME_TYPE = "text/html;charset=iso-8859-1";

const int DefaultHandler::IO_BUFFER_SIZE = 8192;

bool DefaultHandler::handleRequest(HttpWorker *worker,
	HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{
	if ( LogManager::getInstance()->isDebug() ) {
		LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Handling \"%s\"",httpRequest.getUri().c_str());
	}

	if ( httpResponse.getStatusCode()==HttpResponse::HttpStatus::HTTP_OK || 
		httpRequest.getAttribute(HttpConnector::ATTRIBUTE_FORWARD_URI,NULL) )
	{
		if ( handleFileResponse(worker,httpRequest,httpResponse) ) {
			return true;
		}
	}

	if ( httpResponse.getStatusCode()==HttpResponse::HttpStatus::HTTP_FOUND ) {
		return handleRedirectResponse(worker,httpRequest,httpResponse);
	}

	return handleErrorResponse(worker,httpRequest,httpResponse);
}

bool DefaultHandler::handleFileResponse(HttpWorker *worker,
	HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{
	std::string fileExtension;
	std::string filePath = httpRequest.getRealPath();
	boost::filesystem::path boostPath(filePath,boost::filesystem::native);

	try
	{
		// make sure file exists
		if ( !boost::filesystem::exists(boostPath) ) {
			httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_NOT_FOUND);
			return false;
		}

		// check if requested file is a directory
		if ( boost::filesystem::is_directory(boostPath) )
		{
			// check if client should be redirected to the same uri ending with a slash
			if ( *(httpRequest.getUri().end()-1)!='/' ) {
				httpResponse.redirect(httpRequest.getUri() + "/");
				return false;
			}

			// find the first default document that exists within the directory
			const std::list<std::string> &defaultDocuments = httpRequest.getSite()->getDefaultDocuments();
			std::list<std::string>::const_iterator iter;
			for ( iter=defaultDocuments.begin(); iter!=defaultDocuments.end(); iter++ )	
			{
				std::string documentPath = filePath + *iter;

				// check if default document exists
				boost::filesystem::path documentBoostPath(documentPath,boost::filesystem::native);
				if ( boost::filesystem::exists(documentBoostPath) )
				{
					// forward request to default document
					worker->forwardRequest(httpRequest,httpResponse,std::string(httpRequest.getUri() + *iter));
					return true;
				}
			}

			// directory listing is not allowed
			httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_FORBIDDEN);
			return false;
		}
		else
		{
			// get the file extension
			fileExtension = boost::filesystem::extension(boostPath);
			if ( boost::starts_with(fileExtension,".") ) {
				fileExtension.erase(fileExtension.begin());
			}
		}
	}
	catch(boost::filesystem::filesystem_error) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_INTERNAL_SERVER_ERROR);
		return false;
	}

	FILE *file = fopen(filePath.c_str(),"rb");
	if ( file!=NULL )
	{
		fpos_t fileLength = 0;

		fseek(file,0,SEEK_END);
		fgetpos(file,&fileLength);
		fseek(file,0,SEEK_SET);

		std::string mimeType = httpRequest.getSite()->getMimeMapping(fileExtension);
		if ( mimeType.empty() ) {
			mimeType = DEFAULT_MIME_TYPE;
		}

		httpResponse.setContentType(mimeType);
		httpResponse.setContentLength(fileLength);
		httpResponse.setHeader("Connection","close");

		char *buffer = new char[IO_BUFFER_SIZE];
		memset(buffer,0,IO_BUFFER_SIZE);

		size_t bytesRead = 0;
		while ( (bytesRead = fread(buffer,sizeof(char),IO_BUFFER_SIZE,file))>0 ) {
			httpResponse.write(buffer,bytesRead);
			memset(buffer,0,IO_BUFFER_SIZE);
		}

		httpResponse.flush();

		delete[] buffer;
		fclose(file);
	}
	else
	{
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_NOT_FOUND);
		return false;
	}

	return true;
}

bool DefaultHandler::handleRedirectResponse(HttpWorker *worker,
	HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{
	httpResponse.flush();
	return true;
}

bool DefaultHandler::handleErrorResponse(HttpWorker *worker,
	HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{
	std::string errorCode;
	std::string errorMessage;

	if ( httpResponse.getSubStatusCode()!=0 ) {
		errorCode = Util::ConvertUtil::toString(httpResponse.getStatusCode()) + "." +
			Util::ConvertUtil::toString(httpResponse.getSubStatusCode());
	}
	else {
		errorCode = Util::ConvertUtil::toString(httpResponse.getStatusCode());
	}

	Site *site = httpRequest.getSite();
	if ( site!=NULL )
	{
		std::string forwardUriAttr;
		std::string errorStatusCodeAttr;
		std::string errorSubStatusCodeAttr;
		
		httpRequest.getAttribute(HttpConnector::ATTRIBUTE_FORWARD_URI,&forwardUriAttr);
		httpRequest.getAttribute(HttpConnector::ATTRIBUTE_ERROR_STATUSCODE,&errorStatusCodeAttr);
		httpRequest.getAttribute(HttpConnector::ATTRIBUTE_ERROR_SUBSTATUSCODE,&errorSubStatusCodeAttr);

		bool forwardingAllowed = false;

		// prevent infinite forward loops
		if ( httpRequest.getUri()!=forwardUriAttr ) {
			forwardingAllowed = true;
		}
		else if ( !errorStatusCodeAttr.empty() )
		{
			if ( httpResponse.getStatusCode()!=Util::ConvertUtil::toInt(errorStatusCodeAttr)
				|| httpResponse.getSubStatusCode()!=Util::ConvertUtil::toInt(errorSubStatusCodeAttr) )
			{
				forwardingAllowed = true;
			}
		}

		// match error with existing error page
		const std::list<ErrorPage> &errorPages = site->getErrorPages();
		std::list<ErrorPage>::const_iterator iter;
		for ( iter=errorPages.begin(); iter!=errorPages.end(); iter++ )
		{
			if ( iter->getErrorCode()==errorCode )
			{
				if ( forwardingAllowed && !iter->getLocation().empty() )
				{
					// set original error status code
					httpRequest.setAttribute(HttpConnector::ATTRIBUTE_ERROR_STATUSCODE,
						Util::ConvertUtil::toString(httpResponse.getStatusCode()));

					// set original error sub status code
					httpRequest.setAttribute(HttpConnector::ATTRIBUTE_ERROR_SUBSTATUSCODE,
						Util::ConvertUtil::toString(httpResponse.getSubStatusCode()));

					// forward to error page
					worker->forwardRequest(httpRequest,httpResponse,site->getPath() + iter->getLocation());
					return true;
				}
				else
				{
					errorMessage = iter->getMessage();
					break;
				}
			}
		}
	}

	// send error message
	httpResponse.setContentLength(errorMessage.length());
	httpResponse.setContentType(DEFAULT_MIME_TYPE);
	httpResponse.write((char*)errorMessage.c_str(),errorMessage.length(),true);

	return true;
}
