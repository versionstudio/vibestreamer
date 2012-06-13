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

#ifndef guard_statisticsmanager_h
#define guard_statisticsmanager_h

#include <ace/synch.h>

#include "persistentmanager.h"
#include "singleton.h"
#include "usermanager.h"

/**
* DownloadEntry.
* Class representing a download entry to be stored in the database.
*/
class DownloadEntry
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param userId the database id of the user
	* @param size the size of the download
	* @param timeStamp the time stamp for the download
	* @return instance
	*/
	DownloadEntry(uint64_t userId,uint64_t size,time_t timeStamp) {
		m_userId = userId;
		m_size = size;
		m_timeStamp = timeStamp;
	}

	/**
	* Get the size of the download, in bytes.
	* @Return the size of the download, in bytes.
	*/
	const uint64_t getSize() const {
		return m_size;
	}

	/**
	* Get the time stamp for the download
	* @return the time stamp for the download
	*/
	const time_t getTimeStamp() const {
		return m_timeStamp;
	}

	/**
	* Get the user id for the download.
	* @Return the user id for the download
	*/
	const uint64_t getUserId() const {
		return m_userId;
	}

private:
	time_t m_timeStamp;

	uint64_t m_size;
	uint64_t m_userId;
};

/**
* DownloadStatistics.
* Class representing download statistics for a user.
*/
class DownloadStatistics
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param user the user owning the download statistics.
	* @return instance
	*/
	DownloadStatistics(uint64_t userId) : m_dayDownloadedBytes(0),
		m_dayDownloadedFiles(0),
		m_lastUpdateTime(0),
		m_monthDownloadedBytes(0),
		m_monthDownloadedFiles(0),
		m_totalDownloadedBytes(0),
		m_totalDownloadedFiles(0),
		m_userId(0),
		m_weekDownloadedBytes(0),
		m_weekDownloadedFiles(0)
	{
		m_userId = userId;
	}

	/**
	* Add to the download statistics.
	* @param files the number of files for the download
	* @param size the size of the download in bytes
	* @param timeStamp the time stamp for the download
	*/
	void add(uint64_t files,uint64_t size,time_t timeStamp);

	/**
	* Update the download statistics.
	* This method verifies the last download time and
	* resets the statistics if the current time does not
	* match the last download time.
	*/
	void update();

	/**
	* Get the number of downloaded bytes today.
	* @return the number of downloaded bytes today
	*/
	const uint64_t getDayDownloadedBytes() const {
		return m_dayDownloadedBytes;
	}

	/**
	* Get the number of downloaded files today.
	* @return the number of downloaded files today
	*/
	const uint64_t getDayDownloadedFiles() const {
		return m_dayDownloadedFiles;
	}

	/**
	* Get the number of downloaded bytes this month.
	* @return the number of downloaded bytes this month
	*/
	const uint64_t getMonthDownloadedBytes() const {
		return m_monthDownloadedBytes;
	}

	/**
	* Get the number of downloaded files this month.
	* @return the number of downloaded files this month
	*/
	const uint64_t getMonthDownloadedFiles() const {
		return m_monthDownloadedFiles;
	}

	/**
	* Get the total number of downloaded bytes.
	* @return the total number of downloaded bytes
	*/
	const uint64_t getTotalDownloadedBytes() const {
		return m_totalDownloadedBytes;
	}

	/**
	* Get the total number of downloaded files.
	* @return the total number of downloaded files
	*/
	const uint64_t getTotalDownloadedFiles() const {
		return m_totalDownloadedFiles;
	}

	/**
	* Get the database id of the user owning the download statistics.
	* @return the database id of the user owning the download statistics
	*/
	const uint64_t getUserId() const {
		return m_userId;
	}

	/**
	* Get the number of downloaded bytes this week.
	* @return the number of downloaded bytes this week
	*/
	const uint64_t getWeekDownloadedBytes() const {
		return m_weekDownloadedBytes;
	}

	/**
	* Get the number of downloaded files this week.
	* @return the number of downloaded files this week
	*/
	const uint64_t getWeekDownloadedFiles() const {
		return m_weekDownloadedFiles;
	}

private:
	time_t m_lastUpdateTime;

	uint64_t m_dayDownloadedBytes;
	uint64_t m_dayDownloadedFiles;

	uint64_t m_monthDownloadedBytes;
	uint64_t m_monthDownloadedFiles;

	uint64_t m_totalDownloadedBytes;
	uint64_t m_totalDownloadedFiles;

	uint64_t m_weekDownloadedBytes;
	uint64_t m_weekDownloadedFiles;

	uint64_t m_userId;
};

/**
* StatisticsManager.
* Singleton class that manages all statistics.
*/
class StatisticsManager : public Singleton<StatisticsManager>,
						  public PersistentManager,
						  public UserManagerListener
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	StatisticsManager() {
		UserManager::getInstance()->addListener(this);
	}

	/**
	* Destructor.
	*/
	~StatisticsManager() {
		UserManager::getInstance()->removeListener(this);
	}

	/**
	* @override
	*/
	virtual int load();

	/**
	* @override
	*/
	virtual int save();

	/**
	* Add a download to the download history and statistics.
	* @param downloadEntry the download entry representing the download
	*/
	void addDownload(const DownloadEntry &downloadEntry);

	/**
	* Clear the downloads for the given user.
	* @param user the user to clear the downloads for
	*/
	void clearDownloads(const User &user);

	/**
	* Get the download statistics for the given user.
	* @param user the user to find the download statistics for
	* @return the download statistics for the given user
	*/
	DownloadStatistics getDownloadStatistics(const User &user);

	/**
	* @override
	*/
	virtual void on(UserManagerListener::GroupAdded,const Group &group) {
		// not implemented
	}

	/**
	* @override
	*/
	virtual void on(UserManagerListener::GroupRemoved,const Group &group) {
		// not implemented
	}

	/**
	* @override
	*/
	virtual void on(UserManagerListener::GroupUpdated,const Group &group) {
		// not implemented
	}

	/**
	* @override
	*/
	virtual void on(UserManagerListener::UserAdded,const User &user) {
		// not implemented
	}

	/**
	* @override
	*/
	virtual void on(UserManagerListener::UserRemoved,const User &user);

	/**
	* @override
	*/
	virtual void on(UserManagerListener::UserUpdated,const User &user) {
		// not implemented
	}

private:
	/**
	* Delete the database entry for the given user.
	* @param userId the database id of the user to remove statistics entries for
	*/
	void deleteDbEntry(uint64_t userId);

	/**
	* Load the download statistics for the given user.
	* If no download statistics already exists, a new one 
	* will be created and any statistics will be loaded from the database.
	* @param user the user to find the download statistics for
	* @return the download statistics for the given user
	*/
	DownloadStatistics* loadDownloadStatistics(const User &user);

	ACE_Mutex m_mutex;

	std::list<DownloadEntry> m_downloadEntries;

	std::list<DownloadStatistics> m_downloadStatistics;
};

#endif
