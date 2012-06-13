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

#ifndef guard_httpserverclient_h
#define guard_httpserverclient_h

#include <ace/os.h>
#include <ace/sock_stream.h>
#include <ace/ssl/ssl_sock_stream.h>

#include "httpresponse.h"
#include "httprequest.h"

/**
* HttpServerClient.
* Class representing a client connected to the HttpServer.
*/
class HttpServerClient
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param peer the connected clients socket
	* @param remoteAddres the remote address of the connected client
	* @param bufferSize the size of the buffer used for sending data
	* @param timeout the timeout for recv and send in milliseconds
	* @return instance
	*/
	HttpServerClient(ACE_SOCK_STREAM *peer,
		ACE_INET_Addr remoteAddress,int bufferSize,int timeout) : m_httpResponse(this,bufferSize),
		m_httpRequest(this)
	{
		m_peer = peer;
		m_sslPeer = NULL;
		m_remoteAddress = remoteAddress;
		m_timeout = ACE_Time_Value(0,timeout*1000);

		touch();
	}

	/**
	* Constructor used for creating a new instance for a secure connection.
	* @param sslPeer the connected clients secure socket
	* @param remoteAddress the remote address of the client
	* @param bufferSize the size of the buffer used for sending data
	* @param timeout the timeout for recv and send in milliseconds
	* @return instance
	*/
	HttpServerClient(ACE_SSL_SOCK_STREAM *sslPeer,
		ACE_INET_Addr remoteAddress,int bufferSize,int timeout) : m_httpResponse(this,bufferSize),
		m_httpRequest(this)
	{
		m_peer = NULL;
		m_sslPeer = sslPeer;
		m_remoteAddress = remoteAddress;
		m_timeout = ACE_Time_Value(0,timeout*1000);

		touch();
	}

	/**
	* Destructor.
	*/
	~HttpServerClient() 
	{
		if ( m_sslPeer!=NULL ) {
			m_sslPeer->close();
			delete m_sslPeer;
		}
				
		if ( m_peer!=NULL ) {
			m_peer->close();
			delete m_peer;
		}
	}

	/**
	* Receive data from client socket.
	* @param buffer out parameter where the data will be returned
	* @param buffer the size of the buffer
	* @param nonBlocking set to true if method should not wait for timeout
	* in case data could not be received immediately.
	* @return the number of bytes received
	*/
	ssize_t recv(char *buffer,size_t bufferSize,bool nonBlocking=false);

	/**
	* Send data to the client socket.
	* @param buffer the buffer containg the data to send to the socket
	* @param length the length of the data in the buffer
	* @param nonBlocking set to true if method should not wait for timeout
	* in case data could not be sent immediately.
	* @return the number of bytes sent to the socket
	*/
	ssize_t send(char *buffer,size_t length,bool nonBlocking=false);

	/**
	* Touch the client to update the clients last accessed time.
	* This prevents the client from timeout.
	*/
	void touch() {
		m_lastAccessedTime = Util::TimeUtil::getCalendarTime();
	}

	/**
	* Get the http response linked to this client.
	* @return the http response linked to this client
	*/
	HttpServerResponse& getHttpResponse() {
		return m_httpResponse;
	}

	/**
	* Get the http request linked to this client.
	* @return the http request linked to this client
	*/
	HttpServerRequest& getHttpRequest() {
		return m_httpRequest;
	}

	/**
	* Get the remote address of the client.
	* @return the remote address of the client
	*/
	ACE_INET_Addr& getRemoteAddress() { 
		return m_remoteAddress; 
	}

	/**
	* Get the clients socket.
	* @return the clients socket
	*/
	ACE_SOCK_Stream* getPeer() { 
		return m_peer; 
	}

	/**
	* Get the clients secure socket.
	* @return the clients secure socket
	*/
	ACE_SSL_SOCK_Stream* getSslPeer() { 
		return m_sslPeer; 
	}

	/**
	* Get the last time the client was accessed.
	* @return the last time the client was accessed
	*/
	const time_t& getLastAccessedTime() { return m_lastAccessedTime; }

private:
	ACE_INET_Addr m_remoteAddress;
	ACE_SOCK_Stream *m_peer;
	ACE_SSL_SOCK_Stream *m_sslPeer;
	ACE_Time_Value m_timeout;

	HttpServerResponse m_httpResponse;
	HttpServerRequest m_httpRequest;

	time_t m_lastAccessedTime;
};

#endif
