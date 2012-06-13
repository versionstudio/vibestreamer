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
#include "databasemanager.h"

#define LOGGER_CLASSNAME "DatabaseManager"

#include "logmanager.h"

const std::string DatabaseManager::DATABASE_INDEX = "index";
const std::string DatabaseManager::DATABASE_SERVER = "server";

bool DatabaseManager::init()
{
	std::list<Database>::iterator iter;
	for ( iter=m_databases.begin(); iter!=m_databases.end(); iter++ )
	{
		DatabaseConnection *conn = getConnection(&*iter,true);
		if ( conn==NULL ) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,
				"Could not retrieve an initial connection to database '%s'",iter->getName().c_str());
			return false;
		}

		bool success = false;

		try
		{
			if ( iter->isSynchronous() )
			{
				conn->getSqliteConn().executenonquery(
					"PRAGMA auto_vacuum=FULL;"
					"PRAGMA count_changes=OFF;"
					"PRAGMA synchronous=NORMAL;"
					"PRAGMA temp_store=MEMORY");
			}
			else
			{
				conn->getSqliteConn().executenonquery(
					"PRAGMA auto_vacuum=FULL;"
					"PRAGMA count_changes=OFF;"
					"PRAGMA synchronous=OFF;"
					"PRAGMA temp_store=MEMORY");
			}

			std::string scriptPath = iter->getPath() + ".sql";
			std::fstream file(scriptPath.c_str(),std::ios::in);
			if ( file.is_open() )
			{
				std::stringstream script;
				script << file.rdbuf();

				try 
				{
					std::vector<std::string> queries = tokenizeScript(script.str());
					std::vector<std::string>::iterator queryIter;
					for ( queryIter=queries.begin(); queryIter!=queries.end(); queryIter++ ) 
					{
						if ( LogManager::getInstance()->isDebug() ) {
							LogManager::getInstance()->info(LOGGER_CLASSNAME,
								"Executing init query: %s",queryIter->c_str());
						}

						conn->getSqliteConn().executenonquery(*queryIter);
					}

					success = true;
				}
				catch(exception &ex) {
					LogManager::getInstance()->warning(LOGGER_CLASSNAME,
						"Could not execute script for database '%s' [%s]",iter->getName().c_str(),ex.what());
				}

				file.close();
			}
			else {
				success = true;
			}
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could not optimize database '%s' [%s]",iter->getName().c_str(),ex.what());
		}

		releaseConnection(conn);

		if ( !success ) {
			return false;
		}
	}

	return true;
}

DatabaseConnection* DatabaseManager::getConnection(const std::string name,const bool writeLock)
{
	Database *database = findDatabaseByName(name);
	if ( database!=NULL ) {
		return getConnection(database,writeLock);
	}

	return NULL;
}

DatabaseConnection* DatabaseManager::getConnection(Database *database,const bool writeLock)
{
	DatabaseConnection *conn = NULL;

	m_mutex.acquire();
	conn = database->popConnection();
	m_mutex.release();

	if ( conn==NULL ) {
		conn = database->newConnection();
	}

	if ( conn!=NULL ) 
	{
		if ( writeLock ) {
			acquireWriteLock(database);
		}
	}

	return conn;
}

void DatabaseManager::releaseConnection(DatabaseConnection *conn)
{
	releaseWriteLock(conn->getDatabase());

	m_mutex.acquire();
	conn->getDatabase()->pushConnection(conn);
	m_mutex.release();
}

void DatabaseManager::acquireWriteLock(Database *database)
{
	m_mutexPool.lock(database);

	m_mutex.acquire();
	database->setWriteLocked(true);
	m_mutex.release();
}

void DatabaseManager::releaseWriteLock(Database *database) 
{
	m_mutex.acquire();

	if ( database->isWriteLocked() ) {
		m_mutexPool.release(database);
		database->setWriteLocked(false);
	}

	m_mutex.release();
}

Database* DatabaseManager::findDatabaseByName(const std::string name)
{
	std::list<Database>::iterator iter;
	for ( iter=m_databases.begin(); iter!=m_databases.end(); iter++ ) {
		if ( iter->getName()==name ) {
			return &(*iter);
		}
	}

	return NULL;
}

std::vector<std::string> DatabaseManager::tokenizeScript(const std::string &script)
{
	std::vector<std::string> tokens;
	boost::iter_split(tokens,script,boost::first_finder("\n\n"));
	for ( std::vector<std::string>::iterator iter=tokens.begin(); iter!=tokens.end(); )
	{
		if ( boost::starts_with(boost::trim_copy(*iter),"--") ) {
			iter = tokens.erase(iter);
		}
		else {
			 iter++;
		}
	}

	return tokens;
}

void DatabaseManager::on(ConfigManagerListener::Load)
{
	TiXmlElement databasesElement = ConfigManager::getInstance()->getElement(ConfigManager::DATABASEMANAGER_DATABASES);
	TiXmlNode *databaseNode = databasesElement.FirstChildElement("database");
	while ( databaseNode!=NULL )
	{
		Database db;
		TiXmlNode *node = NULL;

		node = databaseNode->FirstChild("name");
		if ( node!=NULL && node->FirstChild()!=NULL ) {
			db.setName(node->FirstChild()->Value());
		}

		node = databaseNode->FirstChild("path");
		if ( node!=NULL && node->FirstChild()!=NULL ) {
			db.setPath(node->FirstChild()->Value());
		}

		node = databaseNode->FirstChild("synchronous");
		if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
			db.setSynchronous(true);
		}

		m_databases.push_back(db);
		databaseNode = databasesElement.IterateChildren("database",databaseNode);
	}
}
