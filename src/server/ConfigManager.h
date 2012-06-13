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

#ifndef guard_configmanager_h
#define guard_configmanager_h

#include "../tinyxml/tinyxml.h"

#include <ace/synch.h>

#include "eventbroadcaster.h"
#include "persistentmanager.h"
#include "singleton.h"

/**
* ConfigManagerListener.
* Abstract class containing event definitions for the ConfigManager class.
*/
class ConfigManagerListener
{
public:
	template<int I>	struct X { enum { TYPE = I };  };

	typedef X<0> Load;
	typedef X<1> Save;

	/**
	* Fired when the configuration is loaded.
	*/
	virtual void on(Load) = 0;

	/**
	* Fired when the configuration is saved.
	*/
	virtual void on(Save) = 0;
};

/**
* ConfigManager.
* Singleton class that manages the configuration for the server.
*/
class ConfigManager  : public Singleton<ConfigManager>,
					   public EventBroadcaster<ConfigManagerListener>,
					   public PersistentManager
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	ConfigManager() : m_serverNode(NULL) {

	}

	static const std::string DATABASEMANAGER_DATABASES;

	static const std::string HTTPSERVER_CONNECTOR_BUFFERSIZE;
	static const std::string HTTPSERVER_CONNECTOR_CLIENTTIMEOUT;
	static const std::string HTTPSERVER_CONNECTOR_MAXCLIENTS;
	static const std::string HTTPSERVER_CONNECTOR_MAXHEADERSIZE;
	static const std::string HTTPSERVER_CONNECTOR_MAXPOSTSIZE;
	static const std::string HTTPSERVER_CONNECTOR_MAXWORKERS;
	static const std::string HTTPSERVER_CONNECTOR_SERVERADDRESS;
	static const std::string HTTPSERVER_CONNECTOR_SERVERHOST;
	static const std::string HTTPSERVER_CONNECTOR_SERVERPORT;
	static const std::string HTTPSERVER_CONNECTOR_SSLCERTIFICATEFILE;
	static const std::string HTTPSERVER_CONNECTOR_SSLCERTIFICATEKEYFILE;
	static const std::string HTTPSERVER_CONNECTOR_SSLCERTIFICATEKEYPASSWORD;
	static const std::string HTTPSERVER_CONNECTOR_SSLENABLED;

	static const std::string HTTPSERVER_REQUESTHANDLERS;

	static const std::string HTTPSERVER_SESSIONMANAGER_MAXSESSIONS;
	static const std::string HTTPSERVER_SESSIONMANAGER_SESSIONTIMEOUT;

	static const std::string INDEXER_FILEPATTERN;
	static const std::string INDEXER_INCLUDEHIDDEN;
	static const std::string INDEXER_MAPPINGS;

	static const std::string LOGMANAGER_DEBUG;
	static const std::string LOGMANAGER_PATH;

	/**
	* @override
	*/
	virtual int load();

	/**
	* @override
	*/
	virtual int save();

	/**
	* Restore the default configurations.
	*/
	void restoreDefaults();

	/**
	* Get a configuration as a boolean.
	* @param name the configuration name
	* @return the configuration value
	*/
	const bool getBool(const std::string name) {
		return Util::ConvertUtil::toBool(getString(name));
	}

	/**
	* Get a configuration as an integer.
	* @param name the configuration name
	* @return the configuration value
	*/
	const int getInt(const std::string name) {
		return Util::ConvertUtil::toInt(getString(name));
	}

	/**
	* Get configuration as a 64 bit integer.
	* @param name the configuration name
	* @return the configuration value
	*/
	const int64_t getInt64(const std::string name) {
		return Util::ConvertUtil::toInt64(getString(name));
	}

	/**
	* Get a configuration as an xml element.
	* @param name the configuration name
	* @return the configuration xml element
	*/
	TiXmlElement getElement(const std::string name) {
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

		return *findNode(name,true)->ToElement();
	}

	/**
	* Get configuration as a string.
	* @param name the configuration name
	* @return the configuration value
	*/
	const std::string getString(const std::string name) {
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex);
		TiXmlNode *node = findNode(name);
		if ( node!=NULL && node->FirstChild() ) {
			return node->FirstChild()->Value();
		}

		return "";
	}

	/**
	* Get whether a configuration element exists.
	* @param name the configuration name
	* @return true if the configuration element exists
	*/
	const bool hasElement(const std::string name) {
		return findNode(name)!=NULL;
	}

	/**
	* Set a boolean configuration.
	* @param name the configuration name
	* @param value the configuration value
	*/
	void setBool(const std::string name,const bool value) {
		setString(name,Util::ConvertUtil::toString(value));
	}

	/**
	* Set an integer configuration.
	* @param name the configuration name
	* @param value the configuration value
	*/
	void setInt(const std::string name,const int value) {
		setString(name,Util::ConvertUtil::toString(value));
	}

	/**
	* Set a 64 bit integer configuration.
	* @param name the configuration name
	* @param value the configuration value
	*/
	void setInt64(const std::string name,const int64_t value) {
		setString(name,Util::ConvertUtil::toString(value));
	}

	/**
	* Set a string configuration.
	* @param name the configuration name
	* @param value the configuration value
	*/
	void setString(const std::string name,const std::string value) {
		ACE_Write_Guard<ACE_Mutex> guard(m_mutex);
		TiXmlNode *node = findNode(name,true);
		node->Clear();
		node->InsertEndChild(TiXmlText(value.c_str()));
	}

	/**
	* Set an xml element configuration.
	* @param name the configuration name
	* @param element the configuration element
	*/
	void setElement(const std::string name,const TiXmlElement &element) {
		ACE_Write_Guard<ACE_Mutex> guard(m_mutex);
		TiXmlNode *node = findNode(name,true);
		node->Parent()->ReplaceChild(node,element);
	}

private:
	/**
	* Find a an xml node by the given path.
	* @param path the path, separated with dots to indicate the element nodes.
	* For example: server.httpServer.connector.serverHost
	* @param create true if a non-existing path should be created
	* @return the found xml node or NULL if no path could be found
	*/
	TiXmlNode* findNode(std::string path,bool create=false);

	/**
	* Load all default configurations.
	*/
	void loadDefaults();

	void setDefaultBool(const std::string name,const bool value) {
		setDefaultString(name,Util::ConvertUtil::toString(value));
	}

	void setDefaultInt(const std::string name,const int value) {
		setDefaultString(name,Util::ConvertUtil::toString(value));
	}

	void setDefaultInt64(const std::string name,const int64_t value) {
		setDefaultString(name,Util::ConvertUtil::toString(value));
	}

	void setDefaultString(const std::string name,const std::string value) {
		if ( hasElement(name)==NULL ) {
			setString(name,value);
		}
	}

	ACE_Mutex m_mutex;

	TiXmlDocument m_document;

	TiXmlNode *m_serverNode;
};

#endif
