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
#include "configmanager.h"

#define LOGGER_CLASSNAME "ConfigManager"

#include "databasemanager.h"
#include "logmanager.h"

const std::string ConfigManager::DATABASEMANAGER_DATABASES = "databaseManager.databases";

const std::string ConfigManager::HTTPSERVER_CONNECTOR_BUFFERSIZE = "httpServer.connector.bufferSize";
const std::string ConfigManager::HTTPSERVER_CONNECTOR_CLIENTTIMEOUT = "httpServer.connector.clientTimeout";
const std::string ConfigManager::HTTPSERVER_CONNECTOR_MAXCLIENTS = "httpServer.connector.maxClients";
const std::string ConfigManager::HTTPSERVER_CONNECTOR_MAXHEADERSIZE = "httpServer.connector.maxHeaderSize";
const std::string ConfigManager::HTTPSERVER_CONNECTOR_MAXPOSTSIZE = "httpServer.connector.maxPostSize";
const std::string ConfigManager::HTTPSERVER_CONNECTOR_MAXWORKERS = "httpServer.connector.maxWorkers";
const std::string ConfigManager::HTTPSERVER_CONNECTOR_SERVERADDRESS = "httpServer.connector.serverAddress";
const std::string ConfigManager::HTTPSERVER_CONNECTOR_SERVERHOST = "httpServer.connector.serverHost";
const std::string ConfigManager::HTTPSERVER_CONNECTOR_SERVERPORT = "httpServer.connector.serverPort";
const std::string ConfigManager::HTTPSERVER_CONNECTOR_SSLCERTIFICATEFILE = "httpServer.connector.sslCertificateFile";
const std::string ConfigManager::HTTPSERVER_CONNECTOR_SSLCERTIFICATEKEYFILE = "httpServer.connector.sslCertificateKeyFile";
const std::string ConfigManager::HTTPSERVER_CONNECTOR_SSLCERTIFICATEKEYPASSWORD = "httpServer.connector.sslCertificateKeyPassword";
const std::string ConfigManager::HTTPSERVER_CONNECTOR_SSLENABLED = "httpServer.connector.sslEnabled";

const std::string ConfigManager::HTTPSERVER_REQUESTHANDLERS = "httpServer.requestHandlers";

const std::string ConfigManager::HTTPSERVER_SESSIONMANAGER_MAXSESSIONS = "httpServer.sessionManager.maxSessions";
const std::string ConfigManager::HTTPSERVER_SESSIONMANAGER_SESSIONTIMEOUT = "httpServer.sessionManager.sessionTimeout";

const std::string ConfigManager::INDEXER_FILEPATTERN = "indexer.filePattern";
const std::string ConfigManager::INDEXER_INCLUDEHIDDEN = "indexer.includeHidden";
const std::string ConfigManager::INDEXER_MAPPINGS = "indexer.mappings";

const std::string ConfigManager::LOGMANAGER_DEBUG = "logManager.debug";
const std::string ConfigManager::LOGMANAGER_PATH = "logManager.path";

int ConfigManager::load()
{
	if ( LogManager::getInstance()->isDebug() ) {
		LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Loading");
	}

	m_mutex.acquire();
	m_document.LoadFile("conf\\server.xml");

	m_serverNode = m_document.FirstChildElement("server");
	if ( m_serverNode==NULL ) {
		m_serverNode = m_document.InsertEndChild(TiXmlElement("server"));
	}

	loadDefaults();
	m_mutex.release();
	fireEvent(ConfigManagerListener::Load());

	return 0;
}

int ConfigManager::save()
{
	if ( LogManager::getInstance()->isDebug() ) {
		LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Saving");
	}

	m_mutex.acquire_read();

	if ( m_document.FirstChild()!=NULL && m_document.FirstChild()->ToDeclaration()==NULL ) {
		m_document.InsertBeforeChild(m_document.FirstChild(),TiXmlDeclaration("1.0","iso-8859-1","yes"));
	}

	m_document.SaveFile();
	m_mutex.release();
	fireEvent(ConfigManagerListener::Save());

	return 0;
}

void ConfigManager::restoreDefaults()
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);
	m_serverNode->Clear();
	loadDefaults();
}

TiXmlNode* ConfigManager::findNode(std::string name,bool create)
{
	TiXmlNode *node = m_serverNode;

	std::vector<std::string> tokens;
	boost::split(tokens,name,boost::is_any_of("."));
	std::vector<std::string>::iterator iter;
	for ( iter=tokens.begin(); iter!=tokens.end(); iter++ ) 
	{
		TiXmlNode *foundNode = node->FirstChildElement(iter->c_str());
		if ( foundNode==NULL )
		{
			if ( create ) {
				foundNode = node->InsertEndChild(TiXmlElement(iter->c_str()));
			}
			else {
				node = NULL;
				break;
			}
		}

		node = foundNode;
	}

	return node;
}

void ConfigManager::loadDefaults()
{
	if ( !hasElement(DATABASEMANAGER_DATABASES) )
	{
		TiXmlElement element("databases");
		TiXmlNode *node = NULL;

		node = element.InsertEndChild(TiXmlElement("database"));
		node->InsertEndChild(TiXmlElement("name"))->InsertEndChild(TiXmlText(DatabaseManager::DATABASE_SERVER.c_str()));
		node->InsertEndChild(TiXmlElement("path"))->InsertEndChild(TiXmlText("db/server.db"));
		node->InsertEndChild(TiXmlElement("synchronous"))->InsertEndChild(TiXmlText("false"));

		node = element.InsertEndChild(TiXmlElement("database"));
		node->InsertEndChild(TiXmlElement("name"))->InsertEndChild(TiXmlText(DatabaseManager::DATABASE_INDEX.c_str()));
		node->InsertEndChild(TiXmlElement("path"))->InsertEndChild(TiXmlText("db/index.db"));
		node->InsertEndChild(TiXmlElement("synchronous"))->InsertEndChild(TiXmlText("false"));

		setElement(DATABASEMANAGER_DATABASES,element);
	}

	setDefaultInt(HTTPSERVER_CONNECTOR_BUFFERSIZE,2048);
	setDefaultInt(HTTPSERVER_CONNECTOR_CLIENTTIMEOUT,45000);
	setDefaultInt(HTTPSERVER_CONNECTOR_MAXCLIENTS,150);
	setDefaultInt(HTTPSERVER_CONNECTOR_MAXHEADERSIZE,4096);
	setDefaultInt(HTTPSERVER_CONNECTOR_MAXPOSTSIZE,2097152);
	setDefaultInt(HTTPSERVER_CONNECTOR_MAXWORKERS,20);
	setDefaultString(HTTPSERVER_CONNECTOR_SERVERADDRESS,"0.0.0.0");
	setDefaultString(HTTPSERVER_CONNECTOR_SERVERHOST,"");
	setDefaultInt(HTTPSERVER_CONNECTOR_SERVERPORT,8081);
	setDefaultString(HTTPSERVER_CONNECTOR_SSLCERTIFICATEFILE,"");
	setDefaultString(HTTPSERVER_CONNECTOR_SSLCERTIFICATEKEYFILE,"");
	setDefaultString(HTTPSERVER_CONNECTOR_SSLCERTIFICATEKEYPASSWORD,"");
	setDefaultBool(HTTPSERVER_CONNECTOR_SSLENABLED,false);

	setDefaultInt(HTTPSERVER_SESSIONMANAGER_MAXSESSIONS,10);
	setDefaultInt(HTTPSERVER_SESSIONMANAGER_SESSIONTIMEOUT,45000);

	if ( !hasElement(HTTPSERVER_REQUESTHANDLERS) )
	{
		TiXmlElement element("requestHandlers");
		TiXmlNode *node = NULL;

		node = element.InsertEndChild(TiXmlElement("requestHandler"));
		node->InsertEndChild(TiXmlElement("handler"))->InsertEndChild(TiXmlText("JsHandler"));
		node->InsertEndChild(TiXmlElement("urlPattern"))->InsertEndChild(TiXmlText(".vibe$"));

		node = element.InsertEndChild(TiXmlElement("requestHandler"));
		node->InsertEndChild(TiXmlElement("handler"))->InsertEndChild(TiXmlText("ShareHandler"));
		node->InsertEndChild(TiXmlElement("urlPattern"))->InsertEndChild(TiXmlText("^/share/"));

		setElement(HTTPSERVER_REQUESTHANDLERS,element);
	}

	setDefaultString(INDEXER_FILEPATTERN,".gif$|.jpeg$|.jpg$|.mp3$|.nfo$|.txt$");
	setDefaultBool(INDEXER_INCLUDEHIDDEN,false);

	if ( !hasElement(INDEXER_MAPPINGS) ) 
	{
		TiXmlElement element("mappings");
		TiXmlNode *node = NULL;

		node = element.InsertEndChild(TiXmlElement("mapping"));
		node->InsertEndChild(TiXmlElement("filePattern"))->InsertEndChild(TiXmlText(".mp3$"));
		node->InsertEndChild(TiXmlElement("metadataReader"))->InsertEndChild(TiXmlText("TagLibReader"));

		setElement(INDEXER_MAPPINGS,element);
	}

	setDefaultString(LOGMANAGER_PATH,"logs/server-%y%m%d.log");
}
