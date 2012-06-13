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

#ifndef guard_httpresponse_h
#define guard_httpresponse_h

class HttpServerClient; // forward declaration

/**
* HttpResponse.
* The base class for all HTTP response type classes.
*/
class HttpResponse
{
public:
	enum HttpStatus
	{
		HTTP_OK = 200,
		HTTP_FOUND = 302,
		HTTP_BAD_REQUEST = 400,
		HTTP_UNAUTHORIZED = 401,
		HTTP_FORBIDDEN = 403,
		HTTP_NOT_FOUND = 404,
		HTTP_METHOD_NOT_ALLOWED = 405,
		HTTP_REQUESTENTITYTOOLARGE=413,
		HTTP_INTERNAL_SERVER_ERROR=500
	};

	/**
	* Default constructor.
	* @return instance
	*/
	HttpResponse() : m_statusCode(0) {
		
	}

	/**
	* Get the content length.
	* This method returns the content length header.
	* @return the content length
	*/
	const uint64_t getContentLength() {
		std::string value;
		if ( getHeader("Content-Length",value) ) {
			return Util::ConvertUtil::toUnsignedInt64(value);
		}
		return -1;
	}

	/**
	* Get the content type.
	* This method returns the content type header.
	* @return the content type
	*/
	const std::string getContentType() { 
		std::string value;
		getHeader("Content-Type",value);
		return value;
	}

	/**
	* Get the cookie with the given name.
	* @param name the cookie to retrieve
	* @param value out parameter for the found cookie value
	* @return true if the cookie was found
	*/
	bool getCookie(const std::string &name,std::string &value);

	/**
	* Get all cookies.
	* @return a collection of all cookies
	*/
	const std::map<std::string,std::string> getCookies() { 
		return m_cookies; 
	}

	/**
	* Get the header with the given name.
	* @param name the header to retrieve
	* @param value out parameter for the found header value
	* @return true if the header was found
	*/
	bool getHeader(const std::string &name,std::string &value);

	/**
	* Get all headers.
	* @return a collection of all headers
	*/
	const std::map<std::string,std::string>& getHeaders() { 
		return m_headers; 
	}

	/**
	* Get the status code.
	* @return the status code
	*/
	const int getStatusCode() {
		return m_statusCode;
	}
	
	/**
	* Set the content length.
	* This method sets the content length header.
	* @param contentLength the content length
	*/
	void setContentLength(uint64_t contentLength) {
		setHeader("Content-Length",Util::ConvertUtil::toString(contentLength));
	}

	/**
	* Set the content type.
	* This method sets the content type header.
	* @param contentType the content type
	*/
	void setContentType(std::string contentType) {
		setHeader("Content-Type",contentType);
	}

	/**
	* Set the cookie with the given name.
	* @param name the cookie name
	* @param value the cookie value
	*/
	void setCookie(std::string name,std::string value) {
		m_cookies[name]=value;
	}

	/**
	* Set the header with the given name.
	* @param name the header name
	* @param value the header value
	*/
	void setHeader(std::string name,std::string value) {
		m_headers[name]=value;
	}

	/**
	* Set the status code.
	* @param statusCode the status code
	*/
	void setStatusCode(int statusCode) {
		m_statusCode = statusCode;
	}

protected:
	std::map<std::string,std::string> m_cookies;
	std::map<std::string,std::string> m_headers;

	int m_statusCode;
};

class HttpServerResponse : public HttpResponse
{
public:
	enum HttpSubStatus {
		SUB_FORBIDDEN_SERVER_FULL = 1,
		SUB_FORBIDDEN_ACCOUNT_DISABLED = 2,
		SUB_FORBIDDEN_SESSION_LIMIT_EXCEEDED = 3,
		SUB_FORBIDDEN_DOWNLOAD_LIMIT_EXCEEDED = 4
	};

	/**
	* Constructor used for creating a new instance.
	* @param client the client linked with the response
	* @param bufferSize the size of the buffer used for sending data
	* @return instance
	*/
	HttpServerResponse(HttpServerClient *client,int bufferSize) : m_buffer(NULL),
		m_bufferLength(0),
		m_commited(false),
		m_subStatusCode(0)
	{
		m_client = client;
		m_bufferSize = bufferSize;
	}

	/**
	* Destructor.
	*/
	~HttpServerResponse() {
		if ( m_buffer!=NULL ) {
			delete[] m_buffer;
		}
	}

	/**
	* Flush the buffer and send it to the client.
	* This method also commits the response header if not already done.
	*/
	void flush();

	/**
	* Sends a redirect response to the client using the specified redirect location.
	* @param uri the uri to redirect to
	*/
	void redirect(std::string uri) {
		setHeader("Location",uri);
		m_statusCode = HttpResponse::HTTP_FOUND;
	}

	/**
	* Require authentication from the client.
	* This sets the status code to 401 and the "www-authenticate" header.
	* @param realm the authentication realm
	*/
	void requireAuthentication(std::string realm) {
		setHeader("WWW-Authenticate","Basic Realm=\"" + realm + "\"");
		m_statusCode = HttpResponse::HTTP_UNAUTHORIZED;
	}

	/**
	* Write data to the client.
	* Data will be written to a buffer for buffered output to the client.
	* @param buffer the buffer to send
	* @param length the length of the buffer to send
	* @param autoFlush whether the data should automatically flush the buffer
	*/
	void write(const char *buffer,size_t length,bool autoFlush=false);

	/**
	* Get the sub status code of the response.
	* This is used for implementing more detailed status codes.
	* @return the sub status code of the response
	*/
	const int getSubStatusCode() {
		return m_subStatusCode;
	}

	/**
	* Get whether the response header has been commited.
	* @return true if the response header has been commited
	*/
	const bool isCommited() const {
		return m_commited;
	}

	/**
	* Set the sub status code of the response.
	* @param subStatusCode the sub status code of the response
	*/
	void setSubStatusCode(int subStatusCode) {
		m_subStatusCode = subStatusCode;
	}

private:
	/**
	* Send the response header.
	* This will mark the response as commited and changes
	* to any headers will no more have any effect to the response.
	* @return the number of bytes sent to the client
	*/
	ssize_t sendHeader();

	HttpServerClient *m_client;
	
	char *m_buffer;

	int m_bufferSize;
	int m_bufferLength;
	int m_subStatusCode;

	bool m_commited;
};

/**
* HttpClientResponse.
* This class contains the response of an executed HttpClientRequest.
*/
class HttpClientResponse : public HttpResponse
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	HttpClientResponse() : HttpResponse(),
		m_chunked(false),
		m_chunkSize(0),
		m_parsedHeader(false)
	{
		
	}

	static const int MAX_BODY_SIZE;
	static const int MAX_HEADER_SIZE;

	/**
	* Attach a buffer to the body.
	* This method handles the parsing of the response.
	* @param buffer the buffer to parse and attach to the body
	* @return true if the buffer was attached successfully
	*/
	bool attachBuffer(std::string buffer);

	/**
	* Get the response body.
	* @return the response body
	*/
	const std::string getBody() const {
		return m_body;
	}

	/**
	* Get the response header.
	* @return the response header
	*/
	const std::string getHeader() const {
		return m_header;
	}

private:
	/**
	* Parse the response header.
	*/
	void parseHeader();

	std::string m_body;
	std::string m_chunk;
	std::string m_header;

	int m_chunkSize;

	bool m_chunked;
	bool m_parsedHeader;
};

#endif
