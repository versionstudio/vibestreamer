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
#include "group.h"

void Group::removeOption(std::string name)
{
	std::map<std::string,std::string>::iterator iter = m_options.find(name);
	if ( iter!=m_options.end() ) {
		m_options.erase(iter);
		m_transactions |= TransactionType::TRANSACTION_SETOPTIONS;
	}
}

void Group::removeUser(std::string guid) 
{
	std::list<std::string>::iterator iter;
	for ( iter=m_users.begin(); iter!=m_users.end(); ) {
		if ( *iter==guid ) {
			iter = m_users.erase(iter);
			m_transactions |= TransactionType::TRANSACTION_SETUSERS;
		}
		else {
			iter++;
		}
	}
}

bool Group::getOption(std::string name,std::string *value)
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

const bool Group::isMember(std::string guid) 
{
	std::list<std::string>::iterator iter;
	for ( iter=m_users.begin(); iter!=m_users.end(); iter++ ) {
		if ( *iter==guid ) {
			return true;
		}
	}

	return false;
}
