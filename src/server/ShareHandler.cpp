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
#include "sharehandler.h"

#define LOGGER_CLASSNAME "ShareHandler"

#include <ace/high_res_timer.h>
#include <ace/os.h>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/path.hpp>

#include "bandwidthtracker.h"
#include "databasemanager.h"
#include "logmanager.h"
#include "sharemanager.h"
#include "statisticsmanager.h"
#include "usermanager.h"

const std::string ShareHandler::ATTRIBUTE_BANDWIDTH = "vibe.sharehandler.bandwidth";
const std::string ShareHandler::ATTRIBUTE_LAST_DOWNLOAD = "vibe.sharehandler.lastdownload";
const std::string ShareHandler::ATTRIBUTE_LAST_PLAYED = "vibe.sharehandler.lastplayed";
const std::string ShareHandler::DEFAULT_MIME_TYPE = "text/plain;charset=iso-8859-1";

const int ShareHandler::IO_BUFFER_SIZE = 2048;

bool ShareHandler::handleRequest(HttpWorker *worker,
	HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{
	if ( LogManager::getInstance()->isDebug() ) {
		LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Handling \"%s\"",httpRequest.getUri().c_str());
	}

	HttpSession::Ptr sessionPtr = httpRequest.getSession();

	// make sure client is authenticated
	if ( sessionPtr==NULL ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_UNAUTHORIZED);
		return false;
	}

	// check if user only has browse access
	if ( httpRequest.getUser()->isRoleBrowser() ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_UNAUTHORIZED);
		return false;
	}

	// everything from the last slash and forward is the hash
	size_t pos = httpRequest.getPath().find_last_of("/");
	if ( pos==std::string::npos ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_INTERNAL_SERVER_ERROR);
		return false;
	}

	// make sure hash is not empty
	std::string hash = httpRequest.getPath().substr(pos+1);
	if ( hash.empty() ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_INTERNAL_SERVER_ERROR);
		return false;
	}

	if ( LogManager::getInstance()->isDebug() ) {
		LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Hash \"%s\" was requested",hash.c_str());
	}

	std::string shareIds;

	// create a list of share id's that the user has access to
	std::list<Share> shares = ShareManager::getInstance()->getShares();
	for ( std::list<Share>::iterator iter=shares.begin(); iter!=shares.end(); iter++ ) {
		if ( iter->checkPermission(*httpRequest.getUser(),httpRequest.getRemoteAddress()) ) {
			std::string shareId = Util::ConvertUtil::toString(iter->getDbId());
			shareIds.empty() ? shareIds += shareId : shareIds += "," + shareId;
		}
	}

	// make sure user has access to at least one share
	if ( shareIds.empty() ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_FORBIDDEN);
		return false;
	}

	std::wstring filePath;
	std::wstring fileExtension;

	// query for an item matching the given hash
	DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_INDEX);
	if ( conn!=NULL )
	{
		try
		{
			std::stringstream query;
			query << "SELECT path FROM [items] WHERE shareId IN (" << shareIds << ") AND hash='" 
				<< conn->quote(hash) << "' LIMIT 1";

			filePath = conn->getSqliteConn().executestring16(query.str());
		}
		catch(exception &ex) 
		{
			if ( LogManager::getInstance()->isDebug() ) {
				LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Query for item failed [%s]",ex.what());
			}
		}

		DatabaseManager::getInstance()->releaseConnection(conn);
	}

	boost::filesystem::wpath boostPath(filePath,boost::filesystem::native);

	try
	{
		// make sure file exists
		if ( !boost::filesystem::exists(boostPath) ) {
			httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_NOT_FOUND);
			return false;
		}

		// get the file extension
		fileExtension = boost::filesystem::extension(boostPath);
		if ( boost::starts_with(fileExtension,".") ) {
			fileExtension.erase(fileExtension.begin());
		}
	}
	catch(boost::filesystem::filesystem_error) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_INTERNAL_SERVER_ERROR);
		return false;
	}

	if ( httpRequest.getParameter("play",NULL) )
	{
		sessionPtr->setAttribute(ATTRIBUTE_LAST_PLAYED,hash);
		LogManager::getInstance()->info(LOGGER_CLASSNAME,"User \"%s\" playing \"%ls\"",
			httpRequest.getUser()->getName().c_str(),filePath.c_str());
	}
	else {
		sessionPtr->setAttribute(ATTRIBUTE_LAST_DOWNLOAD,hash);
	}

	bool success = false;

	FILE *file = NULL;

	#ifdef WIN32
		file = _wfopen(filePath.c_str(),L"rb");
	#else
		// TODO: fix non win32 compability
	#endif

	if ( file!=NULL )
	{
		std::string mimeType = httpRequest.getSite()->getMimeMapping(Util::ConvertUtil::toString(fileExtension));
		if ( mimeType.empty() ) {
			mimeType = DEFAULT_MIME_TYPE;
		}

		success = sendStream(httpRequest,httpResponse,file,mimeType);

		fclose(file);
	}
	else {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_INTERNAL_SERVER_ERROR);
	}

	return success;
}

bool ShareHandler::sendStream(HttpServerRequest &httpRequest,
	HttpServerResponse &httpResponse,FILE *file,std::string mimeType)
{
	fpos_t fileSize = 0;

	fseek(file,0,SEEK_END);
	fgetpos(file,&fileSize);
	fseek(file,0,SEEK_SET);

	// make sure user can download the file
	if ( httpRequest.getUser()->checkRoleAllotment(fileSize) )
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

		httpResponse.setHeader("Connection","close");
		httpResponse.setContentType(mimeType);

		if ( fileSize>0 ) {
			httpResponse.setContentLength(fileSize);
		}

		BandwidthTracker tracker;

		time_t startTime = Util::TimeUtil::getCalendarTime();
		time_t lastUpdateTime = startTime;

		char *buffer = new char[IO_BUFFER_SIZE];
		memset(buffer,0,IO_BUFFER_SIZE);

		size_t bytesRead = 0;
		while ( (bytesRead = fread(buffer,sizeof(char),IO_BUFFER_SIZE,file))>0 )
		{
			if ( Thread::current()->isInterrupted() ) {
				break;
			}

			httpResponse.write(buffer,bytesRead);

			tracker.measureBandwidth(bytesRead);

			// limit bandwidth
			int maxBandwidth = httpRequest.getUser()->getRoleMaxBandwidth();
			if ( maxBandwidth>-1 )
			{
				while ( tracker.getAverageBandwidth()>maxBandwidth ) {
					tracker.measureBandwidth();
					Thread::current()->yield();
				}
			}

			// update session once per second
			time_t currentTime = Util::TimeUtil::getCalendarTime();
			if ( currentTime!=lastUpdateTime )
			{
				httpRequest.getSession()->setAttribute(ATTRIBUTE_BANDWIDTH,
					Util::ConvertUtil::toString(tracker.getAverageBandwidth()));

				lastUpdateTime = currentTime;
			}

			memset(buffer,0,IO_BUFFER_SIZE);
		}

		httpResponse.flush();

		delete[] buffer;

		httpRequest.getSession()->setAttribute(ATTRIBUTE_BANDWIDTH,"0");

		if ( tracker.getTotalBytes()>0 ) {
			StatisticsManager::getInstance()->addDownload(
				DownloadEntry(httpRequest.getUser()->getDbId(),tracker.getTotalBytes(),startTime));
		}

		return true;
	}
	else
	{
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_FORBIDDEN);
		httpResponse.setSubStatusCode(HttpServerResponse::SUB_FORBIDDEN_DOWNLOAD_LIMIT_EXCEEDED);
	}

	return false;
}
