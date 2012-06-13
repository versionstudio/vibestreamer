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
#include "httpsession.h"

#include "usermanager.h"
#include "httpsessionmanager.h"

void HttpSession::removeAttribute(std::string name)
{
	if ( m_manager!=NULL ) {
		m_manager->lockSession(this);
	}

	std::map<std::string,std::string>::iterator iter = m_attributes.find(name);
	if ( iter!=m_attributes.end() ) {
		m_attributes.erase(iter);
	}

	if ( m_manager!=NULL ) {
		m_manager->releaseSession(this);
	}
}

bool HttpSession::getAttribute(std::string name,std::string *value)
{
	bool foundAttribute = false;

	if ( m_manager!=NULL ) {
		m_manager->lockSession(this);
	}

	std::map<std::string,std::string>::iterator iter = m_attributes.find(name);
	if ( iter!=m_attributes.end() ) 
	{
		if ( value!=NULL ) {
			*value = iter->second;
		}

		foundAttribute = true;
	}

	if ( m_manager!=NULL ) {
		m_manager->releaseSession(this);
	}

	return foundAttribute;
}

const std::string HttpSession::getPresentationName() const
{
	User user;
	if ( UserManager::getInstance()->findUserByGuid(m_userGuid,&user) ) {
		return user.getName();
	}

	return m_userGuid;
}

void HttpSession::setAttribute(std::string name,std::string value)
{
	if ( m_manager!=NULL ) {
		m_manager->lockSession(this);
	}

	m_attributes[name] = value;

	if ( m_manager!=NULL ) {
		m_manager->releaseSession(this);
	}
}
