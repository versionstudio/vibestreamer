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

#ifndef guard_sharehandler_h
#define guard_sharehandler_h

#include "httprequesthandler.h"

/**
* ShareHandler.
* Handles all requests for retrieving files from the virtual share/index.
*/
class ShareHandler : public HttpRequestHandler
{
public:
	/**
	 * Constructor.
	 * @param urlPatternRegex the url pattern regular expression
	 * @return 
	 */
	ShareHandler(boost::regex urlPatternRegex) {
		m_urlPatternRegex = urlPatternRegex;
	}

	static const std::string ATTRIBUTE_BANDWIDTH;
	static const std::string ATTRIBUTE_LAST_DOWNLOAD;
	static const std::string ATTRIBUTE_LAST_PLAYED;
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
	* Send the stream to the connected client.
	* @param httpRequest the http request
	* @param httpResponse the http response
	* @param file the file stream to transfer
	* @param mimeType the mime type of the file being transferred
	* @return true if the stream was transferred without an error occuring
	*/
	bool sendStream(HttpServerRequest &httpRequest,
		HttpServerResponse &httpResponse,FILE *file,std::string mimeType);
};

#endif
