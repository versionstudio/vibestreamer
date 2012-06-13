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

#ifndef guard_httpconnector_h
#define guard_httpconnector_h

#include <ace/handle_set.h>
#include <ace/sock_acceptor.h>
#include <ace/ssl/ssl_sock_acceptor.h>
#include <ace/synch.h>

#include "httpserverclient.h"
#include "httpworker.h"
#include "thread.h"

class HttpServer; // forward declaration

/**
* HttpConnector.
* Handles all incoming connections and requests.
* All requests are parsed and given to the connector workers
* for handling.
*/
class HttpConnector : public Runnable
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	HttpConnector(HttpServer *httpServer) :
		m_bufferSize(0),
		m_clientTimeout(0),
		m_maxClients(0),
		m_maxHeaderSize(0),
		m_maxPostSize(0),
		m_maxWorkers(0),
		m_serverPort(0),
		m_sslEnabled(false),
		m_started(false),
		m_thread(this)
	{
		m_httpServer = httpServer;
	}

	static const std::string ATTRIBUTE_ERROR_STATUSCODE;
	static const std::string ATTRIBUTE_ERROR_SUBSTATUSCODE;
	static const std::string ATTRIBUTE_FORWARD_QUERYSTRING;
	static const std::string ATTRIBUTE_FORWARD_URI;
	static const std::string AUTH_URI;
	static const std::string PUBLIC_URI;
	static const std::string PRIVATE_URI;
	static const std::string SESSION_COOKIE_NAME;

	/**
	* Start the connector.
	* @return true if the connector was successfully started
	*/
	bool start();
	
	/**
	* Stop the connector.
	*/
	void stop();

	/**
	* @override
	*/
	virtual void run();

	/**
	* Pop an client from the client queue.
	* @return a client from the client queue, NULL if no client was queued.
	*/
	HttpServerClient* popClient();

	/**
	* Push a worker into the idle queue.
	* @param worker the worker to push into the idle queue
	*/
	void pushWorker(HttpWorker *worker);

	/**
	* Get the buffer size for client data.
	* @return the buffer size for client data
	*/
	int getBufferSize() {
		return m_bufferSize;
	}

	/**
	* Get the max header size allowed for an incoming request.
	* @return the max header size allowed for an incoming request
	*/
	int getMaxHeaderSize() {
		return m_maxHeaderSize;
	}

	/**
	* Get the max post data size allowed for an incoming request.
	* @return the max post data size allowed for an incoming request
	*/
	int getMaxPostSize() {
		return m_maxPostSize;
	}

	/**
	* Get the server address.
	* @return the server address
	*/
	const std::string getServerAddress() const { 
		return m_serverAddress; 
	}

	/**
	* Get the server host.
	* @return the server host
	*/
	const std::string getServerHost() const { 
		return m_serverHost; 
	}

	/**
	* Get the server port.
	* @return the server port
	*/
	const int getServerPort() const {
		return m_serverPort;
	}

	/**
	* Get whether ssl is enabled.
	* @return true if ssl is enabled
	*/
	const bool isSslEnabled() const {
		return m_sslEnabled;
	}
	
private:
	/**
	* Password callback for ssl certificate.
	* @return
	*/
	static int defaultSslPasswordCallback(char *buf,int size,int rwflag,void *password);

	/**
	* Initialize ssl support.
	* @param sslCertificateFile the path to the ssl certificate file
	* @param sslCertificateKeyFile the path to the ssl key file
	* @param sslCertificateKeyPassword the password to the key file 
	* @return true if ssl was initialized successfully
	*/
	bool initSsl(std::string sslCertificateFile,std::string sslCertificateKeyFile,
		std::string sslCertificateKeyPassword);

	/**
	* Open the acceptor socket.
	* @param serverAddress the server address to open the acceptor on
	* @param serverPort the server port to open the acceptor on
	* @return true if the acceptor socket could be opened
	*/
	bool openAcceptor(std::string serverAddress,int serverPort);

	/**
	* Close the acceptor socket.
	*/
	void closeAcceptor();

	/**
	* Check and accept any incoming client connections.
	*/
	void checkConnections();

	/**
	* Pop an idle worker.
	* @return an idle worker, NULL if no worker was idle
	*/
	HttpWorker* popWorker();

	/**
	* Push a client into the client queue.
	* @param client the client to push into the client queue
	* @return false if the client queue is full
	*/
	bool pushClient(HttpServerClient *client);

	/**
	* Remove and disconnect all clients in the client queue.
	*/
	void removeClients();

	ACE_Mutex m_mutex;

	ACE_Handle_Set m_acceptorHandleSet;

	ACE_SOCK_Acceptor m_acceptor;
	ACE_SSL_SOCK_Acceptor m_sslAcceptor;

	HttpServer *m_httpServer;

	Thread m_thread;

	std::list<HttpWorker*> m_workers;

	std::stack<HttpWorker*> m_idleWorkers;
	std::stack<HttpServerClient*> m_clientQueue;

	std::string m_serverAddress;
	std::string m_serverHost;

	int m_bufferSize;
	int m_clientTimeout;
	int m_maxClients;
	int m_maxHeaderSize;
	int m_maxPostSize;
	int m_maxWorkers;
	int m_serverPort;

	bool m_sslEnabled;
	bool m_started;
};

#endif
