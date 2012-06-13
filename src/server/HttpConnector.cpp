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
#include "httpconnector.h"

#define LOGGER_CLASSNAME "HttpConnector"

#include <openssl/ssl.h>

#include "configmanager.h"
#include "logmanager.h"

const std::string HttpConnector::ATTRIBUTE_ERROR_STATUSCODE = "vibe.httpconnector.error.statuscode";
const std::string HttpConnector::ATTRIBUTE_ERROR_SUBSTATUSCODE = "vibe.httpconnector.error.substatuscode";
const std::string HttpConnector::ATTRIBUTE_FORWARD_QUERYSTRING = "vibe.httpconnector.forward.querystring";
const std::string HttpConnector::ATTRIBUTE_FORWARD_URI = "vibe.httpconnector.forward.uri";
const std::string HttpConnector::AUTH_URI = "/auth/";
const std::string HttpConnector::PUBLIC_URI = "/_public/";
const std::string HttpConnector::PRIVATE_URI = "/_private/";
const std::string HttpConnector::SESSION_COOKIE_NAME = "sessionguid";

bool HttpConnector::start()
{
	if ( m_started ) {
		return false;
	}

	m_bufferSize = ConfigManager::getInstance()->getInt(ConfigManager::HTTPSERVER_CONNECTOR_BUFFERSIZE);
	m_clientTimeout = ConfigManager::getInstance()->getInt(ConfigManager::HTTPSERVER_CONNECTOR_CLIENTTIMEOUT);
	m_maxClients = ConfigManager::getInstance()->getInt(ConfigManager::HTTPSERVER_CONNECTOR_MAXCLIENTS);
	m_maxHeaderSize = ConfigManager::getInstance()->getInt(ConfigManager::HTTPSERVER_CONNECTOR_MAXHEADERSIZE);
	m_maxPostSize = ConfigManager::getInstance()->getInt(ConfigManager::HTTPSERVER_CONNECTOR_MAXPOSTSIZE);
	m_maxWorkers = ConfigManager::getInstance()->getInt(ConfigManager::HTTPSERVER_CONNECTOR_MAXWORKERS);
	m_serverAddress = ConfigManager::getInstance()->getString(ConfigManager::HTTPSERVER_CONNECTOR_SERVERADDRESS);
	m_serverHost = ConfigManager::getInstance()->getString(ConfigManager::HTTPSERVER_CONNECTOR_SERVERHOST);
	m_serverPort = ConfigManager::getInstance()->getInt(ConfigManager::HTTPSERVER_CONNECTOR_SERVERPORT);
	m_sslEnabled = ConfigManager::getInstance()->getBool(ConfigManager::HTTPSERVER_CONNECTOR_SSLENABLED);

	std::string sslCertificateFile = ConfigManager::getInstance()->getString(ConfigManager::HTTPSERVER_CONNECTOR_SSLCERTIFICATEFILE);
	std::string sslCertificateKeyFile = ConfigManager::getInstance()->getString(ConfigManager::HTTPSERVER_CONNECTOR_SSLCERTIFICATEKEYFILE);
	std::string sslCertificateKeyPassword = ConfigManager::getInstance()->getString(ConfigManager::HTTPSERVER_CONNECTOR_SSLCERTIFICATEKEYPASSWORD);

	// initialize ssl support
	if ( m_sslEnabled && !initSsl(sslCertificateFile,sslCertificateKeyFile,sslCertificateKeyPassword) ) {
		return false;
	}

	// open acceptor socket
	if ( !openAcceptor(m_serverAddress,m_serverPort) ) {
		return false;
	}

	for ( int i=0; i<m_maxWorkers; i++ ) 
	{
		HttpWorker *worker = new HttpWorker(m_httpServer,this);
		if ( worker->start() ) {
			m_workers.push_back(worker);
			m_idleWorkers.push(worker);
		}
		else {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could not start worker");
			delete worker;
		}
	}

	if ( !m_thread.start() ) {
		return false;
	}

	m_started = true;

	return true;
}

void HttpConnector::stop()
{
	if ( !m_started ) {
		return;
	}

	m_thread.cancel();
	closeAcceptor();
	m_thread.join(); // wait for thread to exit
	removeClients(); // remove all queued clients

	while ( !m_workers.empty() ) {
		m_workers.back()->stop();
		delete m_workers.back();
		m_workers.pop_back();
	}

	while ( !m_idleWorkers.empty() ) {
		m_idleWorkers.pop();
	}

	m_started = false;
}

void HttpConnector::run()
{
	while ( true )
	{
		if ( m_thread.isCancelled() ) {
			break;
		}

		checkConnections();
	}
}

int HttpConnector::defaultSslPasswordCallback(char *buf,int size,int rwflag,void *password)
{
    strncpy(buf,(char*)(password),size);
    buf[size - 1] = '\0';

    return strlen(buf);
}

bool HttpConnector::initSsl(std::string sslCertificateFile,std::string sslCertificateKeyFile,
	std::string sslCertificateKeyPassword)
{
	// initialize context
	ACE_SSL_Context *sslContext = ACE_SSL_Context::instance();
	SSL_CTX_set_default_passwd_cb_userdata(sslContext->context(),(char*)sslCertificateKeyPassword.c_str());
	SSL_CTX_set_default_passwd_cb(sslContext->context(),defaultSslPasswordCallback);

	// set certificate file
	if ( sslContext->certificate(sslCertificateFile.c_str(),SSL_FILETYPE_PEM)==-1 ) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Invalid SSL certificate file");
		return false;
	}

	// set certificate key file (private key)
	if ( sslContext->private_key(sslCertificateKeyFile.c_str(),SSL_FILETYPE_PEM)==-1 ) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Invalid SSL certificate key");
		return false;
	}

	return true;
}

bool HttpConnector::openAcceptor(std::string serverAddress,int serverPort)
{
	ACE::set_handle_limit();
	ACE_INET_Addr addr;

	if ( serverAddress.empty() ) 
	{
		if ( addr.set(serverPort)==-1 ) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Invalid server port");
			return false;
		}
	}
	else
	{
		if ( addr.set(serverPort,serverAddress.c_str())==-1 ) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Invalid server address");
			return false;
		}
	}

	if ( m_sslEnabled )
	{
		if ( m_sslAcceptor.open(addr,1)==-1 ) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could not bind at server address");
			return false;
		}

		m_acceptorHandleSet.set_bit(m_sslAcceptor.get_handle());
	}
	else
	{
		if ( m_acceptor.open(addr,1)==-1 ) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could not bind at server address");
			return false;
		}

		m_acceptorHandleSet.set_bit(m_acceptor.get_handle());
	}

	return true;
}

void HttpConnector::closeAcceptor()
{
	if ( m_sslEnabled ) {
		m_sslAcceptor.close();
	}
	else {
		m_acceptor.close();
	}

	m_acceptorHandleSet.reset();
}

void HttpConnector::checkConnections()
{
	ACE_Handle_Set handleSet = m_acceptorHandleSet;
	if ( ACE::select((int)handleSet.max_set()+1,handleSet,NULL)==-1 ) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Socket error");		
		return;
	}

	while ( true )
	{
		ACE_SOCK_Stream *peer = NULL;
		ACE_SSL_SOCK_Stream *sslPeer = NULL;
		ACE_INET_Addr remoteAddress;

		HttpServerClient *client = NULL;

		if ( m_sslEnabled )
		{
			// accept ssl connection - return when no more connections are available
			sslPeer = new ACE_SSL_SOCK_Stream();
			if ( m_sslAcceptor.accept(*sslPeer,&remoteAddress)!=-1 ) {
				sslPeer->enable(TCP_NODELAY); // disable nagle's algorithm
				client = new HttpServerClient(sslPeer,remoteAddress,m_bufferSize,m_clientTimeout);
			}
			else {
				delete sslPeer;
				return;
			}
		}
		else
		{
			// accept connection - return when no more connections are available
			peer = new ACE_SOCK_Stream();
			if ( m_acceptor.accept(*peer,&remoteAddress)!=-1 ) {
				peer->enable(TCP_NODELAY); // disable nagle's algorithm
				client = new HttpServerClient(peer,remoteAddress,m_bufferSize,m_clientTimeout);
			}
			else {
				delete peer;
				return;
			}
		}

		// assign client to an idle worker
		HttpWorker *worker = popWorker();
		if ( worker!=NULL ) {
			worker->assign(client);
		}
		else
		{
			// queue client for later processing
			if ( !pushClient(client) ) {
				delete client;
			}
		}
	}
}

HttpWorker* HttpConnector::popWorker()
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);
	HttpWorker *worker = NULL;

	if ( !m_idleWorkers.empty() ) {
		worker = m_idleWorkers.top();
		m_idleWorkers.pop();
	}
	
	return worker;
}

void HttpConnector::pushWorker(HttpWorker *worker)
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);
	m_idleWorkers.push(worker);
}

HttpServerClient* HttpConnector::popClient()
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);
	HttpServerClient *client = NULL;

	if ( !m_clientQueue.empty() ) {
		client = m_clientQueue.top();
		m_clientQueue.pop();
	}
	
	return client;
}

bool HttpConnector::pushClient(HttpServerClient *client)
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	if ( m_maxClients!=-1 && m_clientQueue.size()>=m_maxClients ) {
		return false;
	}

	m_clientQueue.push(client);

	return true;
}

void HttpConnector::removeClients()
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	while ( !m_clientQueue.empty() ) {
		HttpServerClient *client = m_clientQueue.top();
		m_clientQueue.pop();
		delete client;
	}
}
