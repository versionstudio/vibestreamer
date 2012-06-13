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

#ifndef guard_httpworker_h
#define guard_httpworker_h

#include "httpserverclient.h"
#include "thread.h"

class HttpConnector; // forward declaration
class HttpServer; // forward declaration

/**
* HttpWorker.
* Worker thread used by HttpConnector in order to handle
* multiple requests at once.
*/
class HttpWorker : public Runnable
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param connector the connector
	* @return instance
	*/
	HttpWorker(HttpServer *httpServer,HttpConnector *connector) : m_client(NULL),
		m_started(false),
		m_thread(this) 
	{
		m_httpServer = httpServer;
		m_connector = connector;
	}

	/**
	* Start the worker.
	*/
	bool start();

	/**
	* Stop the worker.
	*/
	void stop();

	/**
	* @override
	*/
	virtual void run();

	/**
	* Assign a client this worker should process.
	* The client will be disconnected and destroyed when the
	* worker has completed.
	*/
	void assign(HttpServerClient *client);

	/**
	* Forward the request to an internal uri.
	* @param httpRequest the request
	* @param httpResponse the response
	* @param uri the uri
	*/
	void forwardRequest(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
		const std::string& uri);

private:
	/**
	* Process the client.
	* @param client the client to process
	*/
	void process(HttpServerClient *client);

	/**
	* Parse the authorization header.
	* @param httpRequest the request
	* @param httpResponse the response
	* @param header the authorization header
	* @return false if header is invalid
	*/
	bool parseAuthorization(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
		const std::string &header);

	/**
	* Parse the content length header.
	* @param httpRequest the request
	* @param httpResponse the response
	* @param header the content length header
	* @return false if header is invalid
	*/
	bool parseContentLength(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
		const std::string &header);

	/**
	* Parse the entire HTTP header.
	* @param httpRequest the request
	* @param httpResponse the response
	* @param header the entire HTTP header
	* @return false if HTTP header is invalid
	*/
	bool parseHeader(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
		const std::string &header);

	/**
	* Parse all cookies.
	* @param httpRequest the request
	* @param httpResponse the response
	* @param header the cookie header
	*/
	void parseCookies(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
		const std::string &header);

	/**
	* Parse post data into parameters.
	* @param httpRequest the request
	* @param httpResponse the response
	* @param postData the post data
	*/
	void parsePostData(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
		const std::string &postData);

	/**
	* Parse query string into parameters.
	* @param httpRequest the request
	* @param httpResponse the response
	* @param queryString the query string
	*/
	void parseQueryString(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
		const std::string &queryString);

	/**
	* Matches the request with a host, site context and checks credentials
	* and prepares the request to be handled.
	* @param httpRequest the request
	* @param httpResponse the response
	*/
	void handleRequest(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse);

	/**
	* Handle the requested uri with the appropriate request handler.
	* Any invalid/error requests as well as requests that cannot be match
	* with a unique handler will be handled by the default request handler.
	* @param httpRequest the request
	* @param httpResponse the response
	*/
	void handleRequestedUri(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse);

	/**
	* Check the request and match the requested host.
	* @param httpRequest the request
	* @param httpResponse the response
	*/
	void checkHost(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse);

	/**
	* Check the request and match the requested site.
	* @param httpRequest the request
	* @param httpResponse the response
	*/
	void checkSite(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse);

	/**
	* Check the request and match credentials.
	* If the client has a valid session cookie this will be matched against an existing session.
	* If the client has no valid session cookie the client will be forwarded to the login form.
	* Login form submits is also handled by this method.
	* @param httpRequest the request
	* @param httpResponse the response
	*/
	void checkCredentials(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse);

	/**
	* Logs on the client as the configured anonymous user.
	* @param httpRequest the request
	* @param httpResponse the response
	* @return true if the client was successfully logged on
	*/
	bool logonAnonymousUser(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse);

	/**
	* Logs on the client as the given user.
	* @param httpRequest the request
	* @param httpResponse the response
	* @param user the user the client should be logged on as
	* @return true if the client was successfully logged on
	*/
	bool logonUser(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,User &user);

	/**
	* Check if the given user is allowed to create a new session.
	* @return true if the given user is allowed to create a new session
	*/
	bool isAllowedSession(const User &user,const std::string &remoteAddress);

	/**
	* Get whether the request is a request to auth form, for logging on.
	* @param httpRequest the request
	* @return true if the request is a request to the autm for
	*/
	bool isAuthFormRequest(HttpServerRequest &httpRequest);

	/**
	* Get whether the request is for anything in the public folder.
	* @param httpRequest the request
	* @return true if the request is for anything in the public folder
	*/
	bool isPublicRequest(HttpServerRequest &httpRequest);

	/**
	* Get whether the request is for anything in the private folder.
	* @param httpRequest the request
	* @return true if the request is for anything in the private folder
	*/
	bool isPrivateRequest(HttpServerRequest &httpRequest);

	HttpConnector *m_connector;

	HttpServer *m_httpServer;

	HttpServerClient *m_client;

	Thread m_thread;

	bool m_started;
};

#endif
