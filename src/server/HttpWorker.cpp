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
#include "httpworker.h"

#define LOGGER_CLASSNAME "HttpWorker"

#include "base64.h"
#include "logmanager.h"
#include "httpserver.h"
#include "sitemanager.h"
#include "usermanager.h"

bool HttpWorker::start()
{
	if ( m_started ) {
		return false;
	}

	if (!m_thread.start() ) {
		return false;
	}

	m_started = true;

	return true;
}

void HttpWorker::stop()
{
	if ( !m_started ) {
		return;
	}

	m_thread.cancel();
	m_thread.join();

	m_started = false;
}

void HttpWorker::run()
{
	while ( true )
	{
		if ( m_thread.isCancelled() ) {
			break;
		}
		
		if ( m_thread.wait()==Thread::State::SIGNALED ) 
		{
			while ( m_client!=NULL ) {
				process(m_client);
				delete m_client;
				m_client = m_connector->popClient();
			}

			m_connector->pushWorker(this);
		}
	}

	if ( m_client!=NULL ) {
		delete m_client;
	}
}

void HttpWorker::assign(HttpServerClient *client)
{
	m_client = client;
	m_thread.notify();
}

void HttpWorker::forwardRequest(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
	const std::string& uri)
{
	httpRequest.setAttribute(HttpConnector::ATTRIBUTE_FORWARD_URI,httpRequest.getUri());
	httpRequest.setAttribute(HttpConnector::ATTRIBUTE_FORWARD_QUERYSTRING,httpRequest.getQueryString());

	httpRequest.setUri(uri);
	httpRequest.clearParameters();
	httpRequest.setQueryString("");

	handleRequestedUri(httpRequest,httpResponse);
}

void HttpWorker::process(HttpServerClient *client)
{
	HttpServerRequest &httpRequest = client->getHttpRequest();
	HttpServerResponse &httpResponse = client->getHttpResponse();

	int bufferSize = m_connector->getBufferSize();
	int maxHeaderSize = m_connector->getMaxHeaderSize();
	int maxPostSize = m_connector->getMaxPostSize();

	std::string header;
	std::string postData;

	bool parsedHeader = false;

	while ( true )
	{
		char *buffer = new char[bufferSize];
		memset(buffer,0,bufferSize);
		
		// check if any bytes were received or if client disconnected
		ssize_t bytesReceived = client->recv(buffer,bufferSize-1);
		if ( bytesReceived>0 )
		{
			if ( !parsedHeader )
			{
				header += buffer;

				// check if header is ready to be parsed
				size_t pos = header.find("\r\n\r\n");
				if ( pos!=std::string::npos )
				{
					if ( pos<header.length() ) {
						postData += header.substr(pos+4);
						header = header.substr(0,pos+4);
					}

					// parse the header
					if ( parseHeader(httpRequest,httpResponse,header) ) {
						parsedHeader = true;
					}
				}
				else
				{
					// make sure header isn't too large
					if ( header.length()>maxHeaderSize ) {
						httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_REQUESTENTITYTOOLARGE);
					}
				}
			}
			else {
				postData += buffer;
			}

			delete[] buffer;

			// check if an error has occured
			if ( httpResponse.getStatusCode()!=0 && httpResponse.getStatusCode()!=HttpResponse::HttpStatus::HTTP_OK ) {
				handleRequest(httpRequest,httpResponse);
				break;
			}
			else if ( parsedHeader )
			{
				if ( httpRequest.getMethod()=="POST" )
				{
					// check if we have too much post data than expected
					if ( postData.length()>httpRequest.getContentLength() ) {
						httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_REQUESTENTITYTOOLARGE);
						handleRequest(httpRequest,httpResponse);
						break;
					}
					
					// handle the request if all post data has been received
					if ( postData.length()==httpRequest.getContentLength() ) {
						parsePostData(httpRequest,httpResponse,postData);
						handleRequest(httpRequest,httpResponse);
						break;
					}
				}
				else
				{
					// make sure this request does not contain any post data
					if ( !postData.empty() ) {
						httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_BAD_REQUEST);
					}

					// handle the request
					handleRequest(httpRequest,httpResponse);
					break;
				}
			}
		}
		else if ( bytesReceived==0 )
		{
			// client disconnected
			delete[] buffer;
			break;
		}
		else if ( bytesReceived==-1 )
		{
			// check if a socket error occured
			int lastError = ACE_OS::last_error();
			if ( lastError!=EWOULDBLOCK ) // sometimes occurs in ssl mode
			{
				if ( LogManager::getInstance()->isDebug() ) {
					LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Socket error during recv (%d)",lastError);
				}

				delete[] buffer;
				break;
			}
		}
	}
}

bool HttpWorker::parseAuthorization(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
	const std::string &header)
{
	if ( header.length()<7 ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_BAD_REQUEST);
		return false;
	}

	if ( boost::to_lower_copy(header).substr(0,5)!="basic" ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_BAD_REQUEST);
		return false;
	}

	std::string encoded = header.substr(6);
	std::string decoded = Base64::decode(encoded);

	size_t pos = decoded.find(":");
	if ( pos!=std::string::npos ) {
		httpRequest.setAuthUserName(decoded.substr(0,pos));
		httpRequest.setAuthPassword(decoded.substr(pos+1));
	}
	else {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_BAD_REQUEST);
		return false;
	}

	return true;
}

void HttpWorker::parseCookies(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
	const std::string &header)
{
	size_t start = 0;
	size_t pos = 0;

	while ( (pos=header.find(';',start))!=std::string::npos ) 
	{
		std::string cookie = header.substr(start,pos-start);
		if ( !cookie.empty() )
		{
			size_t delimiterPos = cookie.find('=');
			if ( delimiterPos!=std::string::npos ) {
				std::string name = boost::trim_copy(cookie.substr(0,delimiterPos));
				std::string value = boost::trim_copy(cookie.substr(delimiterPos+1));
				httpRequest.setCookie(name,value);
			}
		}

		start=pos+1;
	}

	std::string cookie = header.substr(start);
	if ( !cookie.empty() )
	{
		pos = cookie.find('=',0);
		if ( pos!=std::string::npos ) {
			std::string name = boost::trim_copy(cookie.substr(0,pos));
			std::string value = boost::trim_copy(cookie.substr(pos+1));
			httpRequest.setCookie(name,value);
		}
	}
}

void HttpWorker::parsePostData(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
	const std::string &postData)
{
	size_t start = 0;
	size_t pos = 0;

	while ( (pos=postData.find('&',start))!=std::string::npos ) 
	{
		std::string parameter = postData.substr(start,pos-start);
		if ( !parameter.empty() )
		{
			std::string name;
			std::string value;

			size_t delimiterPos = parameter.find('=');
			if ( delimiterPos!=std::string::npos ) 
			{
				name = boost::trim_copy(parameter.substr(0,delimiterPos));
				value = boost::trim_copy(parameter.substr(delimiterPos+1));

				name = Util::CryptoUtil::urlDecode(name);
				value = Util::CryptoUtil::urlDecode(value);
			}

			httpRequest.setParameter(name,value);
		}

		start=pos+1;
	}

	std::string parameter = postData.substr(start);
	if ( !parameter.empty() )
	{
		std::string name;
		std::string value;

		pos = parameter.find('=',0);
		if ( pos!=std::string::npos ) 
		{
			name = boost::trim_copy(parameter.substr(0,pos));
			value = boost::trim_copy(parameter.substr(pos+1));

			name = Util::CryptoUtil::urlDecode(name);
			value = Util::CryptoUtil::urlDecode(value);
		}

		httpRequest.setParameter(name,value);
	}
}

bool HttpWorker::parseContentLength(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
	const std::string &header)
{
	if ( !Util::StringUtil::isNumeric(header) ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_BAD_REQUEST);
		return false;
	}

	uint64_t contentLength = Util::ConvertUtil::toInt64(header);
	if ( contentLength<0 ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_BAD_REQUEST);
		return false;
	}

	httpRequest.setContentLength(contentLength);

	return true;
}

bool HttpWorker::parseHeader(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
	const std::string &header)
{
	size_t start = 0;
	size_t pos = 0;

	// find method
	pos = header.find(' ',0);
	if ( pos==std::string::npos ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_BAD_REQUEST);
		return false;
	}

	// validate method
	std::string method = boost::to_upper_copy(header.substr(0,pos));
	if ( method!="GET" && method!="POST" ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_METHOD_NOT_ALLOWED);
		return false;
	}
	else {
		httpRequest.setMethod(method);
	}

	// find uri
	start = pos+1;
	pos = header.find(' ',start);
	if ( pos==std::string::npos ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_BAD_REQUEST);
		return false;
	}

	std::string uri = header.substr(start,pos-start);
	std::string queryString;

	// find query string
	size_t queryStringPos = uri.find("?");
	if ( queryStringPos!=std::string::npos ) {
		queryString = uri.substr(queryStringPos+1);
		uri = uri.substr(0,queryStringPos);
	}
	
	// url decode and validate uri
	uri = Util::CryptoUtil::urlDecode(uri);
	if ( Util::UriUtil::isValid(uri) && Util::UriUtil::isAbsolute(uri) ) {
		httpRequest.setUri(uri);
		httpRequest.setQueryString(queryString);
	}
	else {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_BAD_REQUEST);
		return false;
	}

	// parse http version
	start = pos+1;
	pos = header.find("\r\n",start);
	if ( pos==std::string::npos ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_BAD_REQUEST);
		return false;
	}
	else {
		std::string version = header.substr(start,pos-start);
		httpRequest.setVersion(version);
	}

	// parse query string
	parseQueryString(httpRequest,httpResponse,queryString);

	// parse headers
	start = pos+2;
	while ( (pos=header.find("\r\n",start))!=std::string::npos )
	{
		std::string headerLine = header.substr(start,pos-start);
		size_t delimiterPos = headerLine.find(":");
		if ( delimiterPos!=std::string::npos )
		{
			std::string name = headerLine.substr(0,delimiterPos);
			std::string value = headerLine.substr(delimiterPos+2);
			
			httpRequest.setHeader(name,value);

			boost::to_lower(name);

			if ( name=="authorization" ) {
				if ( !parseAuthorization(httpRequest,httpResponse,value) ) {
					return false;
				}
			}
			else if ( name=="content-length" ) {
				if ( !parseContentLength(httpRequest,httpResponse,value) ) {
					return false;
				}
			}	
			else if ( name=="cookie" ) {
				parseCookies(httpRequest,httpResponse,value);
			}
			else if ( name=="host" ) {
				httpRequest.setHost(value);
			}
		}

		start = pos+2;
	}

	httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_OK);

	return true;
}

void parsePostData(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
	const std::string &postData)
{
	size_t start = 0;
	size_t pos = 0;

	while ( (pos=postData.find('&',start))!=std::string::npos ) 
	{
		std::string parameter = postData.substr(start,pos-start);
		if ( !parameter.empty() )
		{
			std::string name;
			std::string value;

			size_t delimiterPos = parameter.find('=');
			if ( delimiterPos!=std::string::npos ) 
			{
				name = boost::trim_copy(parameter.substr(0,delimiterPos));
				value = boost::trim_copy(parameter.substr(delimiterPos+1));

				name = Util::CryptoUtil::urlDecode(name);
				value = Util::CryptoUtil::urlDecode(value);
			}
			else {
				name = Util::CryptoUtil::urlDecode(parameter);
			}

			httpRequest.setParameter(name,value);
		}

		start=pos+1;
	}

	std::string parameter = postData.substr(start);
	if ( !parameter.empty() )
	{
		std::string name;
		std::string value;

		pos = parameter.find('=',0);
		if ( pos!=std::string::npos ) 
		{
			name = boost::trim_copy(parameter.substr(0,pos));
			value = boost::trim_copy(parameter.substr(pos+1));

			name = Util::CryptoUtil::urlDecode(name);
			value = Util::CryptoUtil::urlDecode(value);
		}

		httpRequest.setParameter(name,value);
	}
}

void HttpWorker::parseQueryString(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,
	const std::string &queryString)
{
	size_t start = 0;
	size_t pos = 0;

	while ( (pos=queryString.find('&',start))!=std::string::npos ) 
	{
		std::string parameter = queryString.substr(start,pos-start);
		if ( !parameter.empty() )
		{
			std::string name;
			std::string value;

			size_t delimiterPos = parameter.find('=');
			if ( delimiterPos!=std::string::npos ) 
			{
				name = boost::trim_copy(parameter.substr(0,delimiterPos));
				value = boost::trim_copy(parameter.substr(delimiterPos+1));

				name = Util::CryptoUtil::urlDecode(name);
				value = Util::CryptoUtil::urlDecode(value);
			}
			else {
				name = Util::CryptoUtil::urlDecode(parameter);
			}

			httpRequest.setParameter(name,value);
		}

		start=pos+1;
	}

	std::string parameter = queryString.substr(start);
	if ( !parameter.empty() )
	{
		std::string name;
		std::string value;

		pos = parameter.find('=',0);
		if ( pos!=std::string::npos )
		{
			name = boost::trim_copy(parameter.substr(0,pos));
			name = Util::CryptoUtil::urlDecode(name);

			value = boost::trim_copy(parameter.substr(pos+1));
			value = Util::CryptoUtil::urlDecode(value);
		}
		else {
			name = Util::CryptoUtil::urlDecode(parameter);
		}

		httpRequest.setParameter(name,value);
	}
}

void HttpWorker::handleRequest(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{	
	if ( httpResponse.getStatusCode()==HttpResponse::HttpStatus::HTTP_OK ) 
	{
		// match host
		checkHost(httpRequest,httpResponse);
		if ( httpResponse.getStatusCode()==HttpResponse::HttpStatus::HTTP_OK ) 
		{
			// match site
			checkSite(httpRequest,httpResponse);
			if ( httpResponse.getStatusCode()==HttpResponse::HttpStatus::HTTP_OK ) 
			{
				// check if request is for anything in the private directory
				if ( isPrivateRequest(httpRequest) ) {
					httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_FORBIDDEN);
				}
				else
				{
					// check if request requires authentication
					if ( !isAuthFormRequest(httpRequest) && !isPublicRequest(httpRequest) ) {
						checkCredentials(httpRequest,httpResponse);
					}
				}
			}
		}
	}

	// reference any attached session to prevent
	// it from expiring during the request handling
	if ( httpRequest.getSession()!=NULL ) {
		m_httpServer->getSessionManager().referenceSession(httpRequest.getSession());
	}

	// handle request through request handler
	if ( httpResponse.getStatusCode()==HttpResponse::HttpStatus::HTTP_OK ) {
		handleRequestedUri(httpRequest,httpResponse);
	}
	else {
		m_httpServer->getDefaultRequestHandler().handleRequest(this,httpRequest,httpResponse);
	}

	// remove reference to any attached session
	// allowing it to expire if inactive
	if ( httpRequest.getSession()!=NULL ) {
		m_httpServer->getSessionManager().dereferenceSession(httpRequest.getSession());
	}

	if ( httpRequest.getSite()!=NULL && httpRequest.getSite()->getAccessLogger()!=NULL ) {
		httpRequest.getSite()->getAccessLogger()->log(&httpRequest,&httpResponse);
	}
}

void HttpWorker::handleRequestedUri(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{
	HttpRequestHandler *requestHandler = NULL;

	// find request handler matching url pattern
	std::list<HttpRequestHandler*>& requestHandlers = m_httpServer->getRequestHandlers();
	for ( std::list<HttpRequestHandler*>::iterator iter=requestHandlers.begin(); 
		iter!=requestHandlers.end(); iter++ ) 
	{
		if ( boost::regex_search(httpRequest.getPath(),(*iter)->getUrlPatternRegex()) ) {
			requestHandler = *iter;
		}
	}

	if ( requestHandler!=NULL ) {
		if ( !requestHandler->handleRequest(this,httpRequest,httpResponse) ) {
			m_httpServer->getDefaultRequestHandler().handleRequest(this,httpRequest,httpResponse);
		}
	}
	else {
		m_httpServer->getDefaultRequestHandler().handleRequest(this,httpRequest,httpResponse);
	}
}

void HttpWorker::checkHost(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{
	std::string host = httpRequest.getHost();
	std::string serverHost = m_connector->getServerHost();

	size_t pos = host.find(":");
	if ( pos!=std::string::npos ) {
		host = host.substr(0,pos);
	}

	if ( !serverHost.empty() && !boost::iequals(serverHost,host) ) {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_NOT_FOUND);
	}
}

void HttpWorker::checkSite(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{
	Site *site = SiteManager::getInstance()->findSiteByPath(httpRequest.getUri());
	if ( site!=NULL ) {
		httpRequest.setSite(site);
		return;
	}

	httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_NOT_FOUND);
}

void HttpWorker::checkCredentials(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{
	std::string sessionGuid;
	if ( httpRequest.getParameter("auth_sessionguid",&sessionGuid)
		|| httpRequest.getCookie(HttpConnector::SESSION_COOKIE_NAME,&sessionGuid) )
	{
		// match clients session cookie with any existing session
		HttpSession::Ptr sessionPtr = m_httpServer->getSessionManager().matchSession(sessionGuid,httpRequest.getRemoteAddress());
		if ( sessionPtr!=NULL ) 
		{
			User user;
			if ( UserManager::getInstance()->findUserByGuid(sessionPtr->getUserGuid(),&user) ) {
				httpRequest.setSession(sessionPtr);
				httpRequest.setUser(user);
				return;
			}
			else {
				m_httpServer->getSessionManager().kickSession(sessionPtr);
			}
		}
	}

	const std::string& authType = httpRequest.getSite()->getAuthType();

	if ( authType=="basic" )
	{
		std::string userName = httpRequest.getAuthUserName();
		std::string password = httpRequest.getAuthPassword();

		if ( !userName.empty() )
		{
			User user;
			if ( UserManager::getInstance()->findUserByName(userName,&user) && user.checkPassword(password) ) {
				logonUser(httpRequest,httpResponse,user);
				return;
			}
		}
		else if ( httpRequest.getSite()->isAnonymousAccess() ) {
			logonAnonymousUser(httpRequest,httpResponse);
			return;
		}

		httpResponse.requireAuthentication(httpRequest.getSite()->getAuthRealm());
	}
	else if ( authType=="form" )
	{
		if ( httpRequest.getMethod()=="POST" &&
			httpRequest.getUri()==httpRequest.getSite()->getPath() + HttpConnector::AUTH_URI )
		{
			std::string userName;
			std::string password;
			std::string target;

			httpRequest.getParameter("auth_username",&userName);
			httpRequest.getParameter("auth_password",&password);
			httpRequest.getParameter("auth_target",&target);

			User user;
			if ( UserManager::getInstance()->findUserByName(userName,&user) && user.checkPassword(password) )
			{
				// attempt login
				if ( logonUser(httpRequest,httpResponse,user) )
				{
					if ( target.empty() ) {
						target = httpRequest.getSite()->getPath();
						if ( target.empty() ) {
							target = "/";
						}
					}

					httpResponse.redirect(target);
				}
			}
			else
			{
				// redirect to auth form with an error
				std::string authForm = httpRequest.getSite()->getPath() +
					httpRequest.getSite()->getAuthForm() + "?error";

				if ( !target.empty() ) {
					authForm += "&target=" + target;
				}

				httpResponse.redirect(authForm);
			}
		}
		else
		{
			// attempt anonymous access
			if ( httpRequest.getSite()->isAnonymousAccess() ) {
				logonAnonymousUser(httpRequest,httpResponse);
			}
			else
			{
				// redirect to auth form
				std::string authForm = httpRequest.getSite()->getPath() + 
					httpRequest.getSite()->getAuthForm();

				if ( httpRequest.getPath()!="/" ) {
					authForm += "?target=" + httpRequest.getUri();
				}

				httpResponse.redirect(authForm);
			}
		}
	}
	else {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_FORBIDDEN);
	}
}

bool HttpWorker::logonAnonymousUser(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{
	User user;
	if ( UserManager::getInstance()->findUserByGuid(httpRequest.getSite()->getAnonymousUserGuid(),&user) ) {
		return logonUser(httpRequest,httpResponse,user);	
	}
	
	httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_FORBIDDEN);

	return false;
}

bool HttpWorker::logonUser(HttpServerRequest &httpRequest,HttpServerResponse &httpResponse,User &user)
{
	if ( user.isDisabled() ) 
	{
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_FORBIDDEN);
		httpResponse.setSubStatusCode(HttpServerResponse::SUB_FORBIDDEN_ACCOUNT_DISABLED);

		return false;
	}
	
	// make sure user has permission to site
	if ( httpRequest.getSite()->checkPermission(user,httpRequest.getRemoteAddress()) )
	{
		// make sure user does not exceed session limits
		if ( isAllowedSession(user,httpRequest.getRemoteAddress()) ) 
		{
			std::string userAgent;
			httpRequest.getHeader("user-agent",&userAgent);

			// create session
			HttpSession::Ptr sessionPtr = HttpSession::Ptr(new HttpSession(user.getGuid(),
				userAgent,httpRequest.getRemoteAddress()));

			// add session to session manager
			if ( m_httpServer->getSessionManager().addSession(sessionPtr) )
			{
				httpRequest.setSession(sessionPtr);
				httpRequest.setUser(user);
				httpResponse.setCookie(HttpConnector::SESSION_COOKIE_NAME,sessionPtr->getGuid());

				user.setLogins(user.getLogins()+1);
				user.setLastKnownIp(httpRequest.getRemoteAddress());
				user.setLastLoginTime(Util::TimeUtil::getCalendarTime());

				UserManager::getInstance()->updateUser(user);

				return true;
			}
			else
			{
				httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_FORBIDDEN);
				httpResponse.setSubStatusCode(HttpServerResponse::SUB_FORBIDDEN_SERVER_FULL);
			}
		}
		else
		{
			httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_FORBIDDEN);
			httpResponse.setSubStatusCode(HttpServerResponse::SUB_FORBIDDEN_SESSION_LIMIT_EXCEEDED);
		}
	}
	else {
		httpResponse.setStatusCode(HttpResponse::HttpStatus::HTTP_FORBIDDEN);
	}

	return false;
}

bool HttpWorker::isAllowedSession(const User &user,const std::string &remoteAddress)
{
	if ( user.isRoleBypassLimits() ) {
		return true;
	}

	bool allowedSession = true;

	int maxUserSessions = user.getRoleMaxSessions();
	int maxUserSessionsPerIp = user.getRoleMaxSessionsPerIp();

	if ( maxUserSessions>-1 || maxUserSessionsPerIp>-1 )
	{
		if ( maxUserSessions==0 ) {
			allowedSession = false;
		}
		else if ( maxUserSessionsPerIp==0 ) {
			allowedSession = false;
		}
		else
		{
			int userSessionsCount = 0;
			int userSessionsPerIpCount = 0;

			std::vector<HttpSession::Ptr> sessions = m_httpServer->getSessionManager().getSessions();
			std::vector<HttpSession::Ptr>::iterator iter;
			for ( iter=sessions.begin(); iter!=sessions.end(); iter++ )
			{
				if ( user.getGuid()!=(*iter)->getUserGuid() ) {
					continue;
				}

				userSessionsCount++;

				if ( (*iter)->getRemoteAddress()==remoteAddress ) {
					userSessionsPerIpCount++;
				}

				if ( maxUserSessions>-1 && userSessionsCount>=maxUserSessions ) {
					allowedSession = false;
					break;
				}

				if ( maxUserSessionsPerIp>-1 && userSessionsPerIpCount>=maxUserSessionsPerIp ) {
					allowedSession = false;
					break;
				}
			}
		}
	}

	return allowedSession;
}

bool HttpWorker::isAuthFormRequest(HttpServerRequest &httpRequest)
{
	return (httpRequest.getSite()->getAuthType()=="form" 
		&& boost::iequals(httpRequest.getPath(),httpRequest.getSite()->getAuthForm()));
}

bool HttpWorker::isPublicRequest(HttpServerRequest &httpRequest)
{
	return boost::istarts_with(httpRequest.getPath(),HttpConnector::PUBLIC_URI);
}

bool HttpWorker::isPrivateRequest(HttpServerRequest &httpRequest)
{
	return boost::istarts_with(httpRequest.getPath(),HttpConnector::PRIVATE_URI);
}
