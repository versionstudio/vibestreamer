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

#ifndef guard_logmanager_h
#define guard_logmanager_h

#include <ace/synch.h>

#include "configmanager.h"
#include "eventbroadcaster.h"
#include "singleton.h"

class LogEntry; // forward declaration
class LogManagerListener; // forward declaration

/**
* LogManager.
* Singleton class that manages logging.
* This class should probably replaced by log4cxx or a similar library.
*/
class LogManager : public Singleton<LogManager>,
				   public EventBroadcaster<LogManagerListener>,
				   public ConfigManagerListener
{
public:
	enum LogLevel 
	{ 
		DEBUG	= 0x00000001, 
		INFO	= 0x00000002,
		WARNING = 0x00000004 
	};

	/**
	* Default constructor.
	* @return instance
	*/
	LogManager() : m_debug(false) {
		ConfigManager::getInstance()->addListener(this);
	}

	/**
	* Destructor.
	*/
	~LogManager() {
		ConfigManager::getInstance()->removeListener(this);
	}
	
	/**
	* Add a debug entry to the log.
	* @param source the source of the log entry. Usually the class name
	* @param fmt the formatted log message together with any additional arguments
	*/
	void debug(const std::string &source,const char *fmt,...);

	/**
	* Add an info entry to the log.
	* @param source the source of the log entry. Usually the class name
	* @param fmt the formatted log message together with any additional arguments
	*/
	void info(const std::string &source,const char *fmt,...);

	/**
	* Add a warning entry to the log.
	* @param source the source of the log entry. Usually the class name
	* @param fmt the formatted log message together with any additional arguments
	*/
	void warning(const std::string &source,const char *fmt,...);

	/**
	* Get whether the log manager is in debug mode.
	* @return true of the log manager is in debug mode
	*/
	const bool isDebug() const {
		return m_debug;
	}

	/**
	* @override
	*/
	virtual void on(ConfigManagerListener::Load);

	/**
	* @override
	*/
	virtual void on(ConfigManagerListener::Save) {
		// not implemented
	}

private:
	/**
	* Add a log entry.
	* @param logEntry the created log entry
	*/
	void log(const LogEntry &logEntry);

	ACE_Mutex m_mutex;

	std::string m_path;

	bool m_debug;
};

/**
* LogEntry.
* Class representing a log entry in the LogManager.
*/
class LogEntry
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param level the log level for the entry
	* @param source the source of the log entry
	* @param message the message
	* @return instance
	*/
	LogEntry(LogManager::LogLevel level,std::string source,std::string message)
	{
		m_source = source;
		m_message = message;
		m_level = level;
		m_creationTime = Util::TimeUtil::getCalendarTime();
	}

	time_t getCreationTime() const {
		return m_creationTime;
	}

	LogManager::LogLevel getLevel() const {
		return m_level;
	}

	const std::string& getMessage() const {
		return m_message;
	}

	const std::string& getSource() const {
		return m_source;
	}

private:
	std::string m_message;
	std::string m_source;

	LogManager::LogLevel m_level;

	time_t m_creationTime;
};

/**
* LogManagerListener.
* Abstract class containing event definitions for the LogManager class.
*/
class LogManagerListener
{
public:
	template<int I>	struct X { enum { TYPE = I };  };

	typedef X<0> Log;

	/**
	* Fired when a log entry is logged
	* @param logEntry the log entry that was logged
	*/
	virtual void on(LogManagerListener::Log,const LogEntry &logEntry) = 0;
};

#endif
