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
#include "logmanager.h"

void LogManager::debug(const std::string &source,const char *fmt,...)
{
	if ( !m_debug ) {
		return;
	}

	std::string message;

	va_list args;
	va_start(args,fmt);
	message = Util::StringUtil::formatArgs(fmt,args);
	va_end(args);

	log(LogEntry(LogManager::LogLevel::DEBUG,source,message));
}

void LogManager::info(const std::string &source,const char *fmt,...)
{
	std::string message;

	va_list args;
	va_start(args,fmt);
	message = Util::StringUtil::formatArgs(fmt,args);
	va_end(args);

	log(LogEntry(LogManager::LogLevel::INFO,source,message));
}

void LogManager::warning(const std::string &source,const char *fmt,...)
{
	std::string message;

	va_list args;
	va_start(args,fmt);
	message = Util::StringUtil::formatArgs(fmt,args);
	va_end(args);

	log(LogEntry(LogManager::LogLevel::WARNING,source,message));
}

void LogManager::log(const LogEntry &logEntry)
{
	m_mutex.acquire();

	tm localTime;
	Util::TimeUtil::getLocalTime(logEntry.getCreationTime(),&localTime);

	std::string formattedLocalTime = Util::TimeUtil::format(localTime,"%Y-%m-%d %H:%M:%S");
	std::string formattedPath = Util::TimeUtil::format(localTime,m_path.c_str());

	std::fstream fs(formattedPath.c_str(),std::ios::out | std::ios::app);
	if ( fs.is_open() )
	{
		switch ( logEntry.getLevel() )
		{
			case LogManager::LogLevel::DEBUG: 
				fs << "DEBUG\t";
			break;

			case LogManager::LogLevel::INFO: 
				fs << "INFO\t";
			break;

			case LogManager::LogLevel::WARNING: 
				fs << "WARNING\t";
			break;
		}

		fs << "[" << formattedLocalTime << "]";
		fs << " " << logEntry.getSource() << " - " << logEntry.getMessage();
		fs << "\r\n";

		fs.close();
	}

	m_mutex.release();

	fireEvent(LogManagerListener::Log(),logEntry);
}

void LogManager::on(ConfigManagerListener::Load)
{
	m_debug = ConfigManager::getInstance()->getBool(ConfigManager::LOGMANAGER_DEBUG);
	m_path = ConfigManager::getInstance()->getString(ConfigManager::LOGMANAGER_PATH);
}
