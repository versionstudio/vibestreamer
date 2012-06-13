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

#ifndef guard_usermanager_h
#define guard_usermanager_h

#include <ace/synch.h>

#include "eventbroadcaster.h"
#include "group.h"
#include "persistentmanager.h"
#include "singleton.h"
#include "user.h"

/**
* UserManagerListener.
* Abstract class containing event definitions for the UserManager class.
*/
class UserManagerListener
{
public:
	template<int I>	struct X { enum { TYPE = I };  };

	typedef X<0> GroupAdded;
	typedef X<1> GroupRemoved;
	typedef X<2> GroupUpdated;

	typedef X<3> UserAdded;
	typedef X<4> UserRemoved;
	typedef X<5> UserUpdated;

	/**
	* Fired when a group is added.
	* @param group the group that was added
	*/
	virtual void on(GroupAdded,const Group &group) = 0;

	/**
	* Fired when a group is removed.
	* @param group the group that was removed
	*/
	virtual void on(GroupRemoved,const Group &group) = 0;

	/**
	* Fired when a group is updated.
	* @param group the group that was updated
	*/
	virtual void on(GroupUpdated,const Group &group) = 0;

	/**
	* Fired when a user is added.
	* @param user the user that was added
	*/
	virtual void on(UserAdded,const User &user) = 0;

	/**
	* Fired when a user is removed.
	* @param user the user that was removed
	*/
	virtual void on(UserRemoved,const User &user) = 0;

	/**
	* Fired when a user is updated.
	* @param user the user that was updated
	*/
	virtual void on(UserUpdated,const User &user) = 0;
};

/**
* UserManager.
* Singleton class that manages all users and groups.
*/
class UserManager : public Singleton<UserManager>, 
					public EventBroadcaster<UserManagerListener>,
					public PersistentManager
{
public:
	/**
	* @override
	*/
	virtual int load();

	/**
	* @override
	*/
	virtual int save();

	/**
	* Add a group to the manager.
	* The added group will be given a guid and a database id.
	* @param group the group to add to the manager
	*/
	bool addGroup(Group &group);

	/**
	* Remove a group from the manager.
	* @param group the group to remove
	* @return true if the group was successfully removed
	*/
	bool removeGroup(const Group &group);

	/**
	* Update the managed group with any 
	* transactions that's been done in the given group instance.
	* @param group the group to update the managed group with
	* @return true if the group was successfully updated
	*/
	bool updateGroup(Group &group);

	/**
	* Add a user to the manager.
	* The added user will be given a guid and a database id.
	* @param user the user to add to the manager
	*/
	bool addUser(User &user);

	/**
	* Remove a user from the manager.
	* @param user the user to remove
	* @return true if the user was successfully removed
	*/
	bool removeUser(const User &user);

	/**
	* Update the managed user with any 
	* transactions that's been done in the given user instance.
	* @param user the user to update the managed user with
	* @return true if the user was successfully updated
	*/
	bool updateUser(User &user);

	/**
	* Get a group by database id.
	* @param dbId the database id of the group to look for
	* @param group the out parameter where the found group is returned
	* @return true if a group was found
	*/
	bool findGroupByDbId(uint64_t dbId,Group *group);

	/**
	* Get a group by guid.
	* @param guid the guid of the group to look for
	* @param group the out parameter where the found group is returned
	* @return true if a group was found
	*/
	bool findGroupByGuid(const std::string &guid,Group *group);

	/**
	* Get a group by name.
	* @param name the name of the group to look for
	* @param group the out parameter where the found group is returned
	* @return true if a group was found
	*/
	bool findGroupByName(const std::string &name,Group *group);

	/**
	* Get a user by database id.
	* @param dbId the database id of the user to look for
	* @param user the out parameter where the found user is returned
	* @return true if a user was found
	*/
	bool findUserByDbId(uint64_t dbId,User *user);

	/**
	* Get a user by guid.
	* @param guid the guid of the user to look for
	* @param user the out parameter where the found user is returned
	* @return true if a user was found
	*/
	bool findUserByGuid(const std::string &guid,User *user);

	/**
	* Get a user by name.
	* @param name the name of the user to look for
	* @param user the out parameter where the found user is returned
	* @return true if a user was found
	*/
	bool findUserByName(const std::string &name,User *user);

	/**
	* Get the number of groups in the manager.
	* @return the number of groups in the manager
	*/
	const size_t getGroupCount() {
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex);
		return m_groups.size();
	}

	/**
	* Get all groups.
	* @return a collection of all groups
	*/

	std::list<Group> getGroups();

	/**
	* Get the number of users in the manager.
	* @return the number of users in the manager
	*/
	const size_t getUserCount() {
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex);
		return m_users.size();
	}

	/**
	* Get all users.
	* @return a collection of all users
	*/
	std::list<User> getUsers();

private:
	/**
	* Prepare a database entry for the given group.
	* If no database entry exists for this group, a new one will be created.
	* @param group the group to prepare a database entry for
	* @return true if the database entry was successfully prepared
	*/
	bool prepareDbEntry(Group &group);

	/**
	* Prepare a database entry for the given user.
	* If no database entry exists for this user, a new one will be created.
	* @param user the user to prepare a database entry for
	* @return true if the database entry was successfully prepared
	*/
	bool prepareDbEntry(User &user);

	/**
	* Delete the database entry for the given group
	* @param group the group to delete the database entry for
	*/
	void deleteDbEntry(const Group &group);

	/**
	* Delete the database entry for the given user
	* @param user the user to delete the database entry for
	*/
	void deleteDbEntry(const User &user);

	/**
	* Update all user memberships for the given group.
	* This validates and handles the new memberships both on the
	* managed users and groups.
	* @param group the group to update memberships for
	* @param users the list of the new members
	*/
	void updateMemberships(Group &group,std::list<std::string> users);

	/**
	* Update all group memberships for the given user.
	* This validates and handles the new memberships both on the
	* managed users and groups.
	* @param user the user to update memberships for
	* @param groups the list of new groups that the user should be a member of
	*/
	void updateMemberships(User &user,std::list<std::string> groups);

	/**
	* Remove all memberships of this group from the managed users.
	* @param group the group to remove all memberships from
	*/
	void removeMemberships(const Group &group);

	/**
	* Remove all memberships for this user from the managed groups.
	* @param user the user to remove all memberships for
	*/
	void removeMemberships(const User &user);

	ACE_Mutex m_mutex;

	std::list<Group> m_groups;
	std::list<User> m_users;
};

#endif
