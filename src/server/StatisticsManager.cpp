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
#include "statisticsmanager.h"

#define LOGGER_CLASSNAME "StatisticsManager"

#include "databasemanager.h"
#include "databasetask.h"
#include "logmanager.h"
#include "taskrunner.h"

void DownloadStatistics::add(uint64_t files,uint64_t size,time_t timeStamp)
{
	tm currentTimeLocal;
	tm timeStampLocal;

	Util::TimeUtil::getLocalTime(&currentTimeLocal);
	Util::TimeUtil::getLocalTime(timeStamp,&timeStampLocal);

	if ( timeStampLocal.tm_year==currentTimeLocal.tm_year
		&& timeStampLocal.tm_mon==currentTimeLocal.tm_mon
		&& timeStampLocal.tm_mday==currentTimeLocal.tm_mday )
	{
		m_dayDownloadedFiles += files;
		m_dayDownloadedBytes += size;
	}

	if ( timeStampLocal.tm_year==currentTimeLocal.tm_year
		&& timeStampLocal.tm_mon==currentTimeLocal.tm_mon )
	{
		m_monthDownloadedFiles += files;
		m_monthDownloadedBytes += size;
	}

	if ( timeStampLocal.tm_year==currentTimeLocal.tm_year
		&& Util::TimeUtil::format(timeStampLocal,"%U")==Util::TimeUtil::format(currentTimeLocal,"%U") )
	{
		m_weekDownloadedFiles += files;
		m_weekDownloadedBytes += size;
	}

	m_totalDownloadedFiles += files;
	m_totalDownloadedBytes += size;
}

void DownloadStatistics::update()
{
	time_t currentTime = Util::TimeUtil::getCalendarTime();

	if ( m_lastUpdateTime>0 )
	{
		tm currentTimeLocal;
		tm lastUpdateTimeLocal;

		Util::TimeUtil::getLocalTime(currentTime,&currentTimeLocal);
		Util::TimeUtil::getLocalTime(m_lastUpdateTime,&lastUpdateTimeLocal);

		// check if daily download statistics should be reset
		if ( !(lastUpdateTimeLocal.tm_year==currentTimeLocal.tm_year
			&& lastUpdateTimeLocal.tm_mon==currentTimeLocal.tm_mon
			&& lastUpdateTimeLocal.tm_mday==currentTimeLocal.tm_mday) )
		{
			m_dayDownloadedFiles = 0;
			m_dayDownloadedBytes = 0;
		}

		// check if monthly download statistics should be reset
		if ( !(lastUpdateTimeLocal.tm_year==currentTimeLocal.tm_year
			&& lastUpdateTimeLocal.tm_mon==currentTimeLocal.tm_mon) )
		{
			m_monthDownloadedFiles = 0;
			m_monthDownloadedBytes = 0;
		}

		// check if weekly download statistics should be reset
		if ( !(lastUpdateTimeLocal.tm_year==currentTimeLocal.tm_year
			&& Util::TimeUtil::format(lastUpdateTimeLocal,"%U")==Util::TimeUtil::format(currentTimeLocal,"%U")) )
		{
			m_weekDownloadedFiles = 0;
			m_weekDownloadedBytes = 0;
		}
	}

	m_lastUpdateTime = currentTime;
}

int StatisticsManager::load()
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Loading");

	return 0;
}

int StatisticsManager::save()
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Saving");

	DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_SERVER,true);
	if ( conn!=NULL )
	{
		try
		{
			sqlite3x::sqlite3_transaction transaction(conn->getSqliteConn(),true);

			std::list<DownloadEntry>::iterator iter;
			for ( iter=m_downloadEntries.begin(); iter!=m_downloadEntries.end(); iter++ )
			{				
				try
				{
					uint64_t downloadId = 0;

					std::stringstream query;
					query << "SELECT downloadId FROM [downloads] "
						<< "WHERE DATE(timeStamp,'unixepoch')=DATE(" << iter->getTimeStamp() << ",'unixepoch') "
						<< "AND userId=" << iter->getUserId() << " LIMIT 1";

					sqlite3x::sqlite3_command cmd(conn->getSqliteConn(),query.str());
					sqlite3x::sqlite3_reader reader = cmd.executereader();

					if ( reader.read() ) {
						downloadId = reader.getint64(0);
					}

					std::stringstream appendQuery;

					if ( downloadId>0 )
					{
						appendQuery << "UPDATE [downloads] "
							<< "SET files=files+1,"
							<< "size=size+" << iter->getSize() << ","
							<< "timeStamp=" << iter->getTimeStamp() << " "
							<< "WHERE downloadId=" << downloadId;
					}
					else 
					{
						appendQuery << "INSERT INTO [downloads] ("
								<< "userId,"
								<< "files,"
								<< "size,"
								<< "timeStamp) "
							<< "VALUES ("
								<< iter->getUserId() << ","
								<< "1" << ","
								<< iter->getSize() << ","
								<< iter->getTimeStamp() << ")";
					}

					conn->getSqliteConn().executenonquery(appendQuery.str());
				}
				catch(exception &ex) {
					LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could not save download statistics [%s]",ex.what());
				}
			}

			transaction.commit();

			m_downloadEntries.clear();
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could not save download statistics [%s]",ex.what());
		}

		DatabaseManager::getInstance()->releaseConnection(conn);
	}

	return 0;
}

void StatisticsManager::deleteDbEntry(uint64_t userId)
{
	std::stringstream taskQuery;
	taskQuery << "DELETE FROM [downloads] WHERE userId=" << userId;
	TaskRunner::getInstance()->schedule(new DatabaseTask(DatabaseManager::DATABASE_SERVER,taskQuery.str(),true));
}

void StatisticsManager::addDownload(const DownloadEntry &downloadEntry)
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	time_t currentTime = Util::TimeUtil::getCalendarTime();

	User user;
	if ( UserManager::getInstance()->findUserByDbId(downloadEntry.getUserId(),&user) )
	{
		DownloadStatistics *downloadStatistics = loadDownloadStatistics(user);
		downloadStatistics->add(1,downloadEntry.getSize(),downloadEntry.getTimeStamp());

		m_downloadEntries.push_back(downloadEntry);
	}
}

void StatisticsManager::clearDownloads(const User &user)
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	// remove all download entries for the user
	std::list<DownloadEntry>::iterator entryIter;
	for ( entryIter=m_downloadEntries.begin(); entryIter!=m_downloadEntries.end(); ) {
		if ( entryIter->getUserId()==user.getDbId() ) {
			entryIter = m_downloadEntries.erase(entryIter);
		}
		else {
			entryIter++;
		}
	}

	// remove download statistics for the user
	std::list<DownloadStatistics>::iterator statsIter;
	for ( statsIter=m_downloadStatistics.begin(); statsIter!=m_downloadStatistics.end(); statsIter++ ) {
		if ( statsIter->getUserId()==user.getDbId() ) {
			m_downloadStatistics.erase(statsIter);
			break;
		}
	}

	deleteDbEntry(user.getDbId());
}

DownloadStatistics StatisticsManager::getDownloadStatistics(const User &user)
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	return *loadDownloadStatistics(user);
}

DownloadStatistics* StatisticsManager::loadDownloadStatistics(const User &user)
{
	DownloadStatistics *downloadStatistics = NULL;

	// find existing download statistics
	std::list<DownloadStatistics>::iterator iter;
	for ( iter=m_downloadStatistics.begin(); iter!=m_downloadStatistics.end(); iter++ ) {
		if ( iter->getUserId()==user.getDbId() ) {
			downloadStatistics = &*iter;
			break;
		}
	}

	if ( downloadStatistics==NULL )
	{
		// create download statistics for the user
		m_downloadStatistics.push_back(DownloadStatistics(user.getDbId()));
		downloadStatistics = &m_downloadStatistics.back();

		DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_SERVER);
		if ( conn!=NULL )
		{
			try
			{
				std::stringstream query;
				query << "SELECT files,size,timeStamp FROM [downloads] WHERE userId=" << user.getDbId();

				sqlite3x::sqlite3_command cmd(conn->getSqliteConn(),query.str());
				sqlite3x::sqlite3_reader reader = cmd.executereader();

				while ( reader.read() ) {
					downloadStatistics->add(reader.getint64(0),reader.getint64(1),reader.getint64(2));
				}
			}
			catch(exception &ex) {
				LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could not get download statistics for user '%s' [%s]",user.getName().c_str(),ex.what());
			}

			DatabaseManager::getInstance()->releaseConnection(conn);
		}
	}

	downloadStatistics->update();

	return downloadStatistics;
}

void StatisticsManager::on(UserManagerListener::UserRemoved,const User &user)
{
	clearDownloads(user);
}
