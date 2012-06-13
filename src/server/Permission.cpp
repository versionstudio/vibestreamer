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
#include "permission.h"

#include "usermanager.h"

bool Permission::checkPermission(const std::list<Permission> &permissions,
	const User &user,const std::string remoteAddress)
{
	if ( user.isDisabled() ) {
		return false;
	}

	bool hasPermission = false;

	std::list<Permission>::const_iterator iter;
	for ( iter=permissions.begin(); iter!=permissions.end(); iter++ )
	{
		if ( !iter->getUserGuid().empty() && user.getGuid()!=iter->getUserGuid() ) {
			continue;
		}

		if ( !iter->getGroupGuid().empty() )
		{
			Group group;
			if ( UserManager::getInstance()->findGroupByGuid(iter->getGroupGuid(),&group) )	{
				if ( group.isDisabled() || !user.isMemberOf(group.getGuid()) ) {
					continue;
				}
			}
			else {
				continue;
			}
		}

		if ( !iter->getRemoteAddress().empty() &&
			!Util::StringUtil::isMatchingIpAddress(remoteAddress,iter->getRemoteAddress()) ) {
			continue;
		}

		if ( iter->isAllowed() ) {
			hasPermission = true;
		}
		else {
			hasPermission = false;
			break;
		}
	}

	return hasPermission;
}
