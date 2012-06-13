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
#include "core.h"

#define LOGGER_CLASSNAME "Core"

#include "configmanager.h"
#include "databasemanager.h"
#include "httpserver.h"
#include "indexer.h"
#include "logmanager.h"
#include "savetask.h"
#include "sharemanager.h"
#include "sitemanager.h"
#include "statisticsmanager.h"
#include "taskrunner.h"
#include "usermanager.h"
#include "version.h"

void Core::init()
{
	ACE::init(); // initialize ace framework

	ConfigManager::newInstance();
	DatabaseManager::newInstance();
	LogManager::newInstance();
	ShareManager::newInstance();
	SiteManager::newInstance();
	UserManager::newInstance();
	StatisticsManager::newInstance();
	HttpServer::newInstance();
	Indexer::newInstance();
	TaskRunner::newInstance();
}

int Core::startup(void (*callback)(void*,const std::string&),void *arg)
{
	if ( callback!=NULL ) callback(arg,"Loading Config Manager");
	if ( ConfigManager::getInstance()->load()!=0 ) {
		return 1;
	}

	LogManager::getInstance()->info(LOGGER_CLASSNAME,"Initializing server");

	if ( callback!=NULL ) { callback(arg,"Initializing Database Manager"); }
	if ( !DatabaseManager::getInstance()->init() ) {
		return 1;
	}

	if ( callback!=NULL ) { callback(arg,"Loading User Manager"); }
	if ( UserManager::getInstance()->load()!=0 ) {
		return 1;
	}

	if ( callback!=NULL ) { callback(arg,"Loading Site Manager"); }
	if ( SiteManager::getInstance()->load()!=0 ) {
		return 1;
	}

	if ( callback!=NULL ) { callback(arg,"Loading Share Manager"); }
	if ( ShareManager::getInstance()->load()!=0 ) {
		return 1;
	}

	if ( callback!=NULL ) callback(arg,"Loading Statistics Manager");
	if ( StatisticsManager::getInstance()->load()!=0 ) {
		return 1;
	}

	if ( callback!=NULL ) { callback(arg,"Starting Task Runner"); }
	if ( !TaskRunner::getInstance()->start() ) {
		return 1;
	}

	if ( callback!=NULL ) { callback(arg,"Initializing Indexer"); }
	if ( !Indexer::getInstance()->init() ) {
		return 1;
	}

	if ( callback!=NULL ) { callback(arg,"Starting Indexer"); }
	if ( !Indexer::getInstance()->start() ) {
		return 1;
	}

	// schedule tasks to save all persistent managers at a regular interval
	TaskRunner::getInstance()->schedule(new SaveTask(ConfigManager::getInstance()),3600,3600);
	TaskRunner::getInstance()->schedule(new SaveTask(ShareManager::getInstance()),3600,3600);
	TaskRunner::getInstance()->schedule(new SaveTask(SiteManager::getInstance()),3600,3600);
	TaskRunner::getInstance()->schedule(new SaveTask(StatisticsManager::getInstance()),30,30);
	TaskRunner::getInstance()->schedule(new SaveTask(UserManager::getInstance()),3600,3600);

	return 0;
}

int Core::cleanup(void (*callback)(void*,const std::string&),void *arg)
{
	LogManager::getInstance()->info(LOGGER_CLASSNAME,"Cleaning up server");

	if ( callback!=NULL ) { callback(arg,"Stopping Http server"); }
	HttpServer::getInstance()->stop();

	if ( callback!=NULL ) { callback(arg,"Stopping Indexer"); }
	Indexer::getInstance()->stop();

	if ( callback!=NULL ) { callback(arg,"Stopping Task Runner"); }
	TaskRunner::getInstance()->stop();

	if ( callback!=NULL ) { callback(arg,"Saving Statistics Manager"); }
	StatisticsManager::getInstance()->save();

	if ( callback!=NULL ) { callback(arg,"Saving Share Manager"); }
	ShareManager::getInstance()->save();

	if ( callback!=NULL ) { callback(arg,"Saving Site Manager"); }
	SiteManager::getInstance()->save();

	if ( callback!=NULL ) { callback(arg,"Saving User Manager"); }
	UserManager::getInstance()->save();

	if ( callback!=NULL ) { callback(arg,"Saving Config Manager"); }
	ConfigManager::getInstance()->save();

	TaskRunner::deleteInstance();
	Indexer::deleteInstance();
	HttpServer::deleteInstance();
	StatisticsManager::deleteInstance();
	UserManager::deleteInstance();
	SiteManager::deleteInstance();
	ShareManager::deleteInstance();
	LogManager::deleteInstance();
	DatabaseManager::deleteInstance();
	ConfigManager::deleteInstance();

	ACE::fini(); // finalize ace framework

	return 0;
}
