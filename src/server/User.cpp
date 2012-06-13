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
#include "user.h"

#include "statisticsmanager.h"
#include "usermanager.h"

const bool User::checkRoleAllotment(uint64_t fileSize) const
{
	if ( fileSize==0 ) {
		return true;
	}

	int maxDownloadPeriod = 0;

	uint64_t maxDownloadBytes = 0;

	if ( m_maxDownloadEnabled ) {
		maxDownloadBytes = m_maxDownloadBytes;
		maxDownloadPeriod = m_maxDownloadPeriod;
	}
	else if ( m_manager!=NULL )
	{
		std::list<std::string>::const_iterator iter;
		for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) 
		{
			Group group;
			if ( !m_manager->findGroupByGuid(*iter,&group) ) {
				continue;
			}

			if ( !group.isDisabled() && group.isMaxDownloadEnabled() ) {
				if ( group.getMaxDownloadPeriod()>maxDownloadPeriod ) {
					maxDownloadBytes = group.getMaxDownloadBytes();
					maxDownloadPeriod = group.getMaxDownloadPeriod();
				}
			}
		}
	}

	if ( maxDownloadPeriod>0 )
	{
		DownloadStatistics downloadStatistics = StatisticsManager::getInstance()->getDownloadStatistics(*this);

		if ( maxDownloadPeriod==User::DownloadPeriod::DAY ) {
			return (downloadStatistics.getDayDownloadedBytes()+fileSize)<maxDownloadBytes;
		}
		else if ( maxDownloadPeriod==User::DownloadPeriod::MONTH ) {
			return (downloadStatistics.getMonthDownloadedBytes()+fileSize)<maxDownloadBytes;
		}
		else if ( maxDownloadPeriod==User::DownloadPeriod::WEEK ) {
			return (downloadStatistics.getWeekDownloadedBytes()+fileSize)<maxDownloadBytes;
		}
	}

	return true;
}

void User::removeGroup(std::string guid) 
{
	std::list<std::string>::iterator iter;
	for ( iter=m_groups.begin(); iter!=m_groups.end(); ) 
	{
		if ( *iter==guid ) {
			iter = m_groups.erase(iter);
			m_transactions |= TransactionType::TRANSACTION_SETGROUPS;
		}
		else {
			iter++;
		}
	}
}

void User::removeOption(std::string name)
{
	std::map<std::string,std::string>::iterator iter = m_options.find(name);
	if ( iter!=m_options.end() ) {
		m_options.erase(iter);
		m_transactions |= TransactionType::TRANSACTION_SETOPTIONS;
	}
}

bool User::getOption(std::string name,std::string *value)
{
	std::map<std::string,std::string>::iterator iter;
	iter = m_options.find(name);
	if ( iter!=m_options.end() ) 
	{
		if ( value!=NULL ) {
			*value = iter->second;
		}

		return true;
	}

	return false;
}

const int User::getRoleMaxBandwidth() const
{
	int maxBandwidth = -1;

	if ( m_maxBandwidthEnabled ) {
		maxBandwidth = m_maxBandwidth;
	}
	else if ( m_manager!=NULL )
	{
		std::list<std::string>::const_iterator iter;
		for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) 
		{
			Group group;
			if ( !m_manager->findGroupByGuid(*iter,&group) ) {
				continue;
			}

			if ( !group.isDisabled() && group.isMaxBandwidthEnabled() ) {
				if ( group.getMaxBandwidth()>maxBandwidth ) {
					maxBandwidth = group.getMaxBandwidth();
				}
			}
		}
	}

	return maxBandwidth;
}

const int User::getRoleMaxSessions() const
{
	int maxSessions = -1;

	if ( m_maxSessionsEnabled ) {
		maxSessions = m_maxSessions;
	}
	else if ( m_manager!=NULL )
	{
		std::list<std::string>::const_iterator iter;
		for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) 
		{
			Group group;
			if ( !m_manager->findGroupByGuid(*iter,&group) ) {
				continue;
			}

			if ( !group.isDisabled() && group.isMaxSessionsEnabled() ) {
				if ( group.getMaxSessions()>maxSessions ) {
					maxSessions = group.getMaxSessions();
				}
			}
		}
	}

	return maxSessions;
}

const int User::getRoleMaxSessionsPerIp() const
{
	int maxSessionsPerIp = -1;

	if ( m_maxSessionsPerIpEnabled ) {
		maxSessionsPerIp = m_maxSessionsPerIp;
	}
	else if ( m_manager!=NULL )
	{
		std::list<std::string>::const_iterator iter;
		for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) 
		{
			Group group;
			if ( !m_manager->findGroupByGuid(*iter,&group) ) {
				continue;
			}

			if ( !group.isDisabled() && group.isMaxSessionsPerIpEnabled() ) {
				if ( group.getMaxSessionsPerIp()>maxSessionsPerIp ) {
					maxSessionsPerIp = group.getMaxSessionsPerIp();
				}
			}
		}	
	}

	return maxSessionsPerIp;
}

bool User::getRoleOption(std::string name,std::string *value)
{
	if ( getOption(name,value) ) {
		return true;
	}
	else if ( m_manager!=NULL )
	{
		std::list<std::string>::const_iterator iter;
		for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) 
		{
			Group group;
			if ( !m_manager->findGroupByGuid(*iter,&group) ) {
				continue;
			}

			if ( !group.isDisabled() && group.getOption(name,value) ) {
				return true;
			}
		}
	}

	return false;
}

std::map<std::string,std::string> User::getRoleOptions()
{
	std::map<std::string,std::string> options;

	// get all group options
	for ( std::list<std::string>::const_iterator iter=m_groups.begin(); 
		iter!=m_groups.end(); iter++ )
	{
		Group group;
		if ( !m_manager->findGroupByGuid(*iter,&group) || group.isDisabled() ) {
			continue;
		}

		const std::map<std::string,std::string> &groupOptions = group.getOptions();
		for ( std::map<std::string,std::string>::const_iterator optionIter=groupOptions.begin();
			optionIter!=groupOptions.end(); optionIter++ )
		{
			options[optionIter->first] = optionIter->second;
		}
	}

	// get all user options
	for ( std::map<std::string,std::string>::const_iterator iter=m_options.begin();
		iter!=m_options.end(); iter++ )
	{
		options[iter->first] = iter->second;
	}

	return options;
}

const bool User::isMemberOf(std::string groupGuid) const
{
	std::list<std::string>::const_iterator iter;
	for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) {
		if ( *iter==groupGuid ) {
			return true;
		}
	}

	return false;
}

const bool User::isRoleAdmin() const
{
	if ( m_admin ) {
		return true;
	}
	else if ( m_manager!=NULL )
	{
		std::list<std::string>::const_iterator iter;
		for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) 
		{
			Group group;
			if ( !m_manager->findGroupByGuid(*iter,&group) ) {
				continue;
			}

			if ( !group.isDisabled() && group.isAdmin() ) {
				return true;
			}
		}	
	}

	return false;
}

const bool User::isRoleBrowser() const
{
	if ( m_browser ) {
		return true;
	}
	else if ( m_manager!=NULL )
	{
		std::list<std::string>::const_iterator iter;
		for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) 
		{
			Group group;
			if ( !m_manager->findGroupByGuid(*iter,&group) ) {
				continue;
			}

			if ( !group.isDisabled() && group.isBrowser() ) {
				return true;
			}
		}	
	}

	return false;
}

const bool User::isRoleBypassLimits() const
{
	if ( m_bypassLimits ) {
		return true;
	}
	else if ( m_manager!=NULL )
	{
		std::list<std::string>::const_iterator iter;
		for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) 
		{
			Group group;
			if ( !m_manager->findGroupByGuid(*iter,&group) ) {
				continue;
			}

			if ( !group.isDisabled() && group.isBypassLimits() ) {
				return true;
			}
		}	
	}

	return false;
}
