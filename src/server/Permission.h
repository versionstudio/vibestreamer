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

#ifndef guard_permission_h
#define guard_permission_h

#include "user.h"

/**
* Permission.
* Class representing a permission.
*/
class Permission
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	Permission() : m_allowed(true) {

	}

	/**
	* Constructor used for creating a new instance.
	* @param userGuid the guid of the user this permission applies to.
	* If left empty the permission applies to any user
	* @param groupGuid the guid of the group this permission applies to.
	* If left empty the permission applies to any group
	* @param remoteAddress the remote ip address this permission applies to.
	* If left empty the permission applies to any group. Wildcards are allowed
	* @param allowed true if this permission is an allowance permission
	*/
	Permission(std::string userGuid,std::string groupGuid,std::string remoteAddress,bool allowed) {
		m_userGuid = userGuid;
		m_groupGuid = groupGuid;
		m_remoteAddress = remoteAddress;
		m_allowed = allowed;
	}

	/**
	* Check whether the permission matches the given permission.
	* @param permission the permission to check against
	* @return true if the permissions matches
	*/
	bool equals(const Permission &permission) {
		return m_userGuid==permission.getUserGuid() &&
			   m_groupGuid==permission.getGroupGuid() &&
			   m_remoteAddress==permission.getRemoteAddress() &&
			   m_allowed==permission.isAllowed();
	}

	/**
	* Get the guid of the group this permission applies to.
	* @return the guid of the group this permission applies to
	*/
	const std::string getGroupGuid() const {
		return m_groupGuid;
	}

	/**
	* Get the remote address this permission applies to.
	* @return the remote ip address this permission applies to
	*/
	const std::string getRemoteAddress() const { 
		return m_remoteAddress; 
	}

	/**
	* Get the guid of the user this permission applies to.
	* @return the guid of the user this permission applies to
	*/
	const std::string getUserGuid() const {
		return m_userGuid;
	}

	/**
	* Get whether the permission is an allowance permission.
	* @return true if the permission is an allowance permission
	*/
	const bool isAllowed() const { 
		return m_allowed; 
	}

	/**
	* Check if the given user has allowance in the given permissions.
	* @param permission the permissions to check if the user has allowance permission on
	* @param user the user to check whether he has permission
	* @param remoteAddress the remote ip address the user is connected as
	* @return true if the given user has allowance in the given permissions
	*/
	static bool checkPermission(const std::list<Permission> &permissions,
		const User &user,const std::string remoteAddress);

private:
	std::string m_groupGuid;
	std::string m_remoteAddress;
	std::string m_userGuid;

	bool m_allowed;
};

#endif
