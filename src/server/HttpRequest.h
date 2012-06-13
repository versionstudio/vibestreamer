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

#ifndef guard_httprequest_h
#define guard_httprequest_h

#include "httpresponse.h"
#include "httpsession.h"
#include "site.h"
#include "user.h"

class HttpServerClient; // forward declaration

/**
* HttpRequest.
* The base class for all HTTP request type classes.
*/
class HttpRequest
{
public:
	/**
	* Clear all cookies.
	*/
	void clearCookies() {
		m_cookies.clear();
	}

	/**
	* Clear all headers.
	*/
	void clearHeader() {
		m_headers.clear();
	}

	/**
	* Clear all parameters.
	*/
	void clearParameters() {
		m_parameters.clear();
	}

	/**
	* Get the cookie with the given name.
	* @param name the cookie to retrieve
	* @param value out parameter for the found cookie value
	* @return true if the cookie was found
	*/
	bool getCookie(const std::string &name,std::string *value);

	/**
	* Get all cookies.
	* @return a collection of all cookies
	*/
	const std::map<std::string,std::string>& getCookies() { 
		return m_cookies; 
	}

	/**
	* Get the header with the given name.
	* @param name the header to retrieve
	* @param value out parameter for the found header value
	* @return true if the header was found
	*/
	bool getHeader(const std::string &name,std::string *value);

	/**
	* Get all headers.
	* @return a collection of all headers
	*/
	const std::map<std::string,std::string>& getHeaders() { 
		return m_headers; 
	}

	/**
	* Get the requested host.
	* @return the requested host
	*/
	const std::string& getHost() const { 
		return m_host; 
	}

	/**
	* Get the request method.
	* @return the request method
	*/
	const std::string& getMethod() const { 
		return m_method; 
	}

	/**
	* Get the parameter with the given name.
	* @param name the parameter to retrieve
	* @param value out parameter for the found parameter value
	* @return true if the parameter was found
	*/
	bool getParameter(const std::string &name,std::string *value);

	/**
	* Get all parameters.
	* @return a collection of all parameters
	*/
	const std::map<std::string,std::string>& getParameters() { 
		return m_parameters; 
	}

	/**
	* Get the request query string.
	* @return the request query string.
	*/
	const std::string& getQueryString() const {
		return m_queryString;
	}

	/**
	* Get the requested uri.
	* Does not contain the query string.
	* @return the requested uri
	*/
	const std::string& getUri() const { 
		return m_uri; 
	}

	/**
	* Get the request HTTP version.
	* @return the request HTTP version
	*/
	const std::string& getVersion() const { 
		return m_version; 
	}

	/**
	* Set the cookie with the given name.
	* @param name the cookie name
	* @param value the cookie value
	*/
	void setCookie(std::string name,std::string value) {
		m_cookies[name] = value;
	}

	/**
	* Set the header with the given name.
	* @param the header name
	* @param the header value
	*/
	void setHeader(std::string name,std::string value) {
		m_headers[name] = value;
	}

	/**
	* Set the request host.
	* @param host the request host
	*/
	void setHost(std::string host) {
		m_host = host;
	}

	/**
	* Set the request method.
	* @param method the request method
	*/
	void setMethod(std::string method) {
		m_method = method;
	}

	/**
	* Set the parameter with the given name.
	* @param name the parameter name
	* @param value the parameter value
	*/
	void setParameter(std::string name,std::string value) {
		m_parameters[name] = value;
	}

	/**
	* Set the request query string.
	* @param queryString the request query string
	*/
	void setQueryString(std::string queryString) {
		m_queryString = queryString;
	}

	/**
	* Set the requested uri.
	* @param uri the requested uri
	*/
	void setUri(std::string uri) {
		m_uri = uri;
	}

	/**
	* Set the request HTTP version.
	* @param version the request HTTP version
	*/
	void setVersion(std::string version) {
		m_version = version;
	}

protected:
	std::map<std::string,std::string> m_parameters;
	std::map<std::string,std::string> m_headers;
	std::map<std::string,std::string> m_cookies;

	std::string m_host;
	std::string m_method;
	std::string m_uri;
	std::string m_version;
	std::string m_queryString;
};

/**
* HttpServerRequest.
* All incoming requests to the server are an instance of this class.
*/
class HttpServerRequest : public HttpRequest
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param client the client linked with the request
	* @return instance
	*/
	HttpServerRequest(HttpServerClient *client) : m_contentLength(0),
		m_site(NULL),
		m_user(NULL)
	{
		m_client = client;
	}

	/**
	* Destructor.
	*/
	~HttpServerRequest()
	{
		if ( m_user!=NULL ) {
			delete m_user;
		}
	}

	/**
	* Remove the attribute with the given name.
	* @param name the attribute to remove
	*/
	void removeAttribute(std::string name);

	/**
	* Get the attribute with the given name.
	* @param name the attribute to retrieve. The name is case sensitive
	* @param value out parameter for the found attribute value
	* @return true if the attribute was found
	*/
	bool getAttribute(std::string name,std::string *value);

	/**
	* Get all attributes.
	* @return a collection of all attributes
	*/
	const std::map<std::string,std::string>& getAttributes() { 
		return m_attributes; 
	}

	/**
	* Get the basic authentication password.
	* @return the basic authentication password
	*/
	std::string getAuthPassword() const { 
		return m_authPassword; 
	}

	/**
	* Get the basic authentication user name.
	* @return the basic authentication user name
	*/
	std::string getAuthUserName() const { 
		return m_authUserName; 
	}

	/**
	* Get the content length of a post.
	* @return the content length of a post
	*/
	uint64_t getContentLength() {
		return m_contentLength;
	}

	/**
	* Get any extra path information associated with the url.
	* The extra path follows the site path but precedes the query string.
	* @return the extra path information.
	*/
	std::string getPath();

	/**
	* Get any extra path information associated with the url
	* but translated into a real path.
	* @return the extra path information translated into a real path
	*/
	std::string getRealPath();

	/**
	* Get the ip address of the client that made the request.
	* @return the ip address of the client that made the request
	*/
	std::string getRemoteAddress();

	/**
	* Get the fully qualified name of the client that sent the request.
	* @return the fully qualified name of the client
	*/
	std::string getRemoteHost();

	/**
	* Get the current session associated with this request.
	* @return the session associated with this request
	*/
	HttpSession::Ptr getSession() {
		return m_sessionPtr;
	}

	/**
	* Get the site associated with this request.
	* @return the site associated with this request
	*/
	Site* getSite() {
		return m_site;
	}

	/**
	* Get the authenticated user associated with this request.
	* @return the authenticated user associated with this request
	*/
	User* getUser() {
		return m_user;
	}

	/**
	* Set the attribute with the given name.
	* @param the header name
	* @param the header value
	*/
	void setAttribute(std::string name,std::string value) {
		m_attributes[name] = value;
	}

	/**
	* Set the basic authentication password.
	* @param authUserName the basic authentication password
	*/
	void setAuthPassword(std::string authPassword) {
		m_authPassword = authPassword;
	}

	/**
	* Set the basic authentication user name.
	* @param authUserName the basic authentication user name
	*/
	void setAuthUserName(std::string authUserName) {
		m_authUserName = authUserName;
	}

	/**
	* Set the content length of a post.
	* @param contentLength the content lengt of a post
	*/
	void setContentLength(uint64_t contentLength) {
		m_contentLength = contentLength;
	}

	/**
	* Set the current session associated with this request.
	* @param sessionPtr the session associated with this request
	*/
	void setSession(HttpSession::Ptr sessionPtr) {
		m_sessionPtr = sessionPtr;
	}

	/**
	* Set the site associated with this request.
	* @param site the site associated with this request
	*/
	void setSite(Site *site) {
		m_site = site;
	}

	/**
	* Set the authenticated user associated with this request.
	* @param user the authenticated user associated with this request
	*/
	void setUser(const User &user)
	{
		if ( m_user!=NULL ) {
			delete m_user;
		}

		m_user = new User(user);
	}

private:
	HttpSession::Ptr m_sessionPtr;

	HttpServerClient *m_client;

	Site *m_site;

	User *m_user;

	std::map<std::string,std::string> m_attributes;

	std::string m_authUserName;
	std::string m_authPassword;

	uint64_t m_contentLength;
};

/**
* HttpClientRequest.
* This class is used to make HTTP requests.
*/
class HttpClientRequest : public HttpRequest
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param host the remote host for the request
	* @param uri the requested uri for the request
	* @param port the remote port for the request
	* @return instance
	*/
	HttpClientRequest(std::string host,std::string uri,int port) : m_bufferSize(2048),
		m_timeout(5000)
	{
		m_host = host;
		m_uri = uri;
		m_port = port;
		m_method = "GET";
		m_version = "1.1";
	}

	/**
	* Execute the request.
	* @param clientResponse out parameter where the response will be returned
	* @return true if request was executed successfully.
	*/
	bool execute(HttpClientResponse &response);

	/**
	* Get the remote port for this request.
	* @return the remote port for this request
	*/
	const int getPort() const {
		return m_port;
	}

	/**
	* Get the timeout value for this request.
	* @return the timeout value for this request
	*/
	const int getTimeout() const {
		return m_timeout;
	}

	/**
	* Set the timeout value for this request.
	* @param timeout the timeout value for this value
	*/
	void setTimeout(int timeout) {
		m_timeout = timeout;
	}

private:
	const int m_bufferSize;

	int m_port;
	int m_timeout;
};

#endif
