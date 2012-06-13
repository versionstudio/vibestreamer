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
#include "accesslogger.h"

#include "httpresponse.h"
#include "httprequest.h"

void AccessLogger::log(HttpServerRequest *httpRequest,HttpServerResponse *httpResponse)
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	tm localTime;
	Util::TimeUtil::getLocalTime(&localTime);
	std::string formattedPath = Util::TimeUtil::format(localTime,m_path.c_str());

	std::fstream file(formattedPath.c_str(),std::ios::out | std::ios::app);
	if ( file.is_open() )
	{
		std::string userId = "-";
		std::string queryString = "-";

		if ( httpRequest->getSession()!=NULL ) {
			userId = httpRequest->getSession()->getUserGuid();
		}

		if ( !httpRequest->getQueryString().empty() ) {
			queryString = httpRequest->getQueryString();
		}

		file << Util::TimeUtil::format(localTime,"%Y-%m-%d %H:%M:%S").c_str() << " ";
		file << httpRequest->getRemoteAddress().c_str() << " ";
		file << userId.c_str() << " ";
		file << httpRequest->getMethod().c_str() << " ";
		file << httpRequest->getUri().c_str() << " ";
		file << queryString.c_str() << " ";
		file << httpResponse->getStatusCode();
		file << "\r\n";

		file.close();
	}
}
