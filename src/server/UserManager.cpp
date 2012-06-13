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
#include "usermanager.h"

#define LOGGER_CLASSNAME "UserManager"

#include "../tinyxml/tinyxml.h"

#include "databasemanager.h"
#include "logmanager.h"

int UserManager::load()
{
	LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Loading");

	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	m_users.clear();
	m_groups.clear();

	TiXmlDocument document;
	document.LoadFile("conf\\users.xml");
	TiXmlNode *usersNode = document.FirstChildElement("users");
	if ( usersNode!=NULL ) 
	{
		// create all user instances
		TiXmlNode *userNode = usersNode->FirstChildElement("user");
		while ( userNode!=NULL )
		{
			User user;

			TiXmlNode *node = NULL;

			node = userNode->FirstChild("guid");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				user.setGuid(node->FirstChild()->Value());
			}

			node = userNode->FirstChild("dbId");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				user.setDbId(Util::ConvertUtil::toUnsignedInt64(node->FirstChild()->Value()));
			}

			node = userNode->FirstChild("name");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				user.setName(node->FirstChild()->Value());
			}

			node = userNode->FirstChild("disabled");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				user.setDisabled(true);
			}

			node = userNode->FirstChild("password");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				user.setEncryptedPassword(node->FirstChild()->Value());
			}

			node = userNode->FirstChild("logins");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				user.setLogins(Util::ConvertUtil::toInt(node->FirstChild()->Value()));
			}

			node = userNode->FirstChild("lastKnownIp");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				user.setLastKnownIp(node->FirstChild()->Value());
			}

			node = userNode->FirstChild("lastLoginTime");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				user.setLastLoginTime(Util::ConvertUtil::toUnsignedInt64(node->FirstChild()->Value()));
			}

			node = userNode->FirstChild("admin");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				user.setAdmin(true);
			}

			node = userNode->FirstChild("browser");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				user.setBrowser(true);
			}

			node = userNode->FirstChild("bypassLimits");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				user.setBypassLimits(true);
			}

			node = userNode->FirstChild("maxBandwidth");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				user.setMaxBandwidth(Util::ConvertUtil::toInt(node->FirstChild()->Value()));
			}

			node = userNode->FirstChild("maxDownloadBytes");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				user.setMaxDownloadBytes(Util::ConvertUtil::toUnsignedInt64(node->FirstChild()->Value()));
			}

			node = userNode->FirstChild("maxBandwidthEnabled");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				user.setMaxBandwidthEnabled(true);
			}

			node = userNode->FirstChild("maxDownloadEnabled");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				user.setMaxDownloadEnabled(true);
			}

			node = userNode->FirstChild("maxDownloadPeriod");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				user.setMaxDownloadPeriod(Util::ConvertUtil::toUnsignedInt64(node->FirstChild()->Value()));
			}

			node = userNode->FirstChild("maxSessions");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				user.setMaxSessions(Util::ConvertUtil::toInt(node->FirstChild()->Value()));
			}

			node = userNode->FirstChild("maxSessionsEnabled");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				user.setMaxSessionsEnabled(true);
			}

			node = userNode->FirstChild("maxSessionsPerIp");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				user.setMaxSessionsPerIp(Util::ConvertUtil::toInt(node->FirstChild()->Value()));
			}

			node = userNode->FirstChild("maxSessionsPerIpEnabled");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				user.setMaxSessionsPerIpEnabled(true);
			}

			TiXmlNode *optionsNode = userNode->FirstChild("options");
			if ( optionsNode!=NULL )
			{
				std::map<std::string,std::string> options;
				node = optionsNode->FirstChildElement("option");
				while ( node!=NULL )
				{
					const char *name = node->ToElement()->Attribute("name");
					if ( name!=NULL )
					{
						std::string value;
						if ( node->FirstChild()!=NULL ) {
							value = node->FirstChild()->Value();
						}

						options[name] = value;
					}

					node = optionsNode->IterateChildren("option",node);
				}

				user.setOptions(options);
			}

			user.setTransactions(User::TRANSACTION_NONE);

			prepareDbEntry(user);

			user.setManager(this);
			m_users.push_back(user);

			userNode = usersNode->IterateChildren("user",userNode);
		}
	}

	TiXmlNode *groupsNode = document.FirstChildElement("groups");
	if ( groupsNode!=NULL ) 
	{
		// create all group instances
		TiXmlNode *groupNode = groupsNode->FirstChildElement("group");
		while ( groupNode!=NULL )
		{
			Group group;

			TiXmlNode *node = NULL;

			node = groupNode->FirstChild("guid");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				group.setGuid(node->FirstChild()->Value());
			}

			node = groupNode->FirstChild("dbId");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				group.setDbId(Util::ConvertUtil::toUnsignedInt64(node->FirstChild()->Value()));
			}

			node = groupNode->FirstChild("name");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				group.setName(node->FirstChild()->Value());
			}

			node = groupNode->FirstChild("disabled");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				group.setDisabled(true);
			}

			node = groupNode->FirstChild("admin");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				group.setAdmin(true);
			}

			node = groupNode->FirstChild("browser");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				group.setBrowser(true);
			}

			node = groupNode->FirstChild("bypassLimits");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				group.setBypassLimits(true);
			}

			node = groupNode->FirstChild("maxBandwidth");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				group.setMaxBandwidth(Util::ConvertUtil::toInt(node->FirstChild()->Value()));
			}

			node = groupNode->FirstChild("maxDownloadBytes");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				group.setMaxDownloadBytes(Util::ConvertUtil::toUnsignedInt64(node->FirstChild()->Value()));
			}

			node = groupNode->FirstChild("maxBandwidthEnabled");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				group.setMaxBandwidthEnabled(true);
			}

			node = groupNode->FirstChild("maxDownloadEnabled");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				group.setMaxDownloadEnabled(true);
			}

			node = groupNode->FirstChild("maxDownloadPeriod");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				group.setMaxDownloadPeriod(Util::ConvertUtil::toUnsignedInt64(node->FirstChild()->Value()));
			}

			node = groupNode->FirstChild("maxSessions");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				group.setMaxSessions(Util::ConvertUtil::toInt(node->FirstChild()->Value()));
			}

			node = groupNode->FirstChild("maxSessionsEnabled");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				group.setMaxSessionsEnabled(true);
			}

			node = groupNode->FirstChild("maxSessionsPerIp");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				group.setMaxSessionsPerIp(Util::ConvertUtil::toInt(node->FirstChild()->Value()));
			}

			node = groupNode->FirstChild("maxSessionsPerIpEnabled");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				group.setMaxSessionsPerIpEnabled(true);
			}

			TiXmlNode *optionsNode = groupNode->FirstChild("options");
			if ( optionsNode!=NULL )
			{
				std::map<std::string,std::string> options;
				node = optionsNode->FirstChildElement("option");
				while ( node!=NULL )
				{
					const char *name = node->ToElement()->Attribute("name");
					if ( name!=NULL )
					{
						std::string value;
						if ( node->FirstChild()!=NULL ) {
							value = node->FirstChild()->Value();
						}

						options[name] = value;
					}

					node = optionsNode->IterateChildren("option",node);
				}

				group.setOptions(options);
			}

			TiXmlNode *membersNode = groupNode->FirstChild("members");
			if ( membersNode!=NULL )
			{
				std::list<std::string> users;
				node = membersNode->FirstChildElement("userGuid");
				while ( node!=NULL )
				{
					if ( node->FirstChild()!=NULL ) {
						users.push_back(node->FirstChild()->Value());
					}

					node = membersNode->IterateChildren("userGuid",node);
				}

				updateMemberships(group,users);
			}

			group.setTransactions(Group::TRANSACTION_NONE);

			prepareDbEntry(group);
		
			group.setManager(this);
			m_groups.push_back(group);
			
			groupNode = groupsNode->IterateChildren("group",groupNode);
		}
	}

	return 0;
}

int UserManager::save()
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Saving");

	TiXmlDocument document;
	document.LoadFile("conf\\users.xml");
	TiXmlNode *usersNode = document.FirstChildElement("users");
	if ( usersNode==NULL ) {
		usersNode = document.InsertEndChild(TiXmlElement("users"));
	}

	if ( usersNode!=NULL )
	{
		usersNode->Clear();

		// save all user instances
		std::list<User>::iterator iter;
		for ( iter=m_users.begin(); iter!=m_users.end(); iter++ ) 
		{
			TiXmlNode *userNode = usersNode->InsertEndChild(TiXmlElement("user"));
			if ( userNode!=NULL )
			{
				TiXmlNode *node = NULL;

				node = userNode->InsertEndChild(TiXmlElement("guid"));
				node->InsertEndChild(TiXmlText(iter->getGuid().c_str()));

				node = userNode->InsertEndChild(TiXmlElement("dbId"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getDbId()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("name"));
				node->InsertEndChild(TiXmlText(iter->getName().c_str()));

				node = userNode->InsertEndChild(TiXmlElement("disabled"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isDisabled()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("password"));
				node->InsertEndChild(TiXmlText(iter->getPassword().c_str()));

				node = userNode->InsertEndChild(TiXmlElement("logins"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getLogins()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("lastKnownIp"));
				node->InsertEndChild(TiXmlText(iter->getLastKnownIp().c_str()));

				node = userNode->InsertEndChild(TiXmlElement("lastLoginTime"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getLastLoginTime()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("admin"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isAdmin()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("browser"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isBrowser()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("bypassLimits"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isBypassLimits()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("maxBandwidth"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getMaxBandwidth()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("maxBandwidthEnabled"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isMaxBandwidthEnabled()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("maxDownloadBytes"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getMaxDownloadBytes()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("maxDownloadEnabled"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isMaxDownloadEnabled()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("maxDownloadPeriod"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getMaxDownloadPeriod()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("maxSessions"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getMaxSessions()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("maxSessionsEnabled"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isMaxSessionsEnabled()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("maxSessionsPerIp"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getMaxSessionsPerIp()).c_str()));

				node = userNode->InsertEndChild(TiXmlElement("maxSessionsPerIpEnabled"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isMaxSessionsPerIpEnabled()).c_str()));

				TiXmlNode *optionsNode = userNode->InsertEndChild(TiXmlElement("options"));
				if ( optionsNode!=NULL )
				{
					const std::map<std::string,std::string> &options = iter->getOptions();
					std::map<std::string,std::string>::const_iterator optionIter;
					for ( optionIter=options.begin(); optionIter!=options.end(); optionIter++ ) {
						node = optionsNode->InsertEndChild(TiXmlElement("option"));
						node->ToElement()->SetAttribute("name",optionIter->first.c_str());
						node->InsertEndChild(TiXmlText(optionIter->second.c_str()));
					}
				}
			}
		}
	}

	TiXmlNode *groupsNode = document.FirstChildElement("groups");
	if ( groupsNode==NULL ) {
		groupsNode = document.InsertEndChild(TiXmlElement("groups"));
	}

	if ( groupsNode!=NULL )
	{
		groupsNode->Clear();

		// save all group instances
		std::list<Group>::iterator iter;
		for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) 
		{
			TiXmlNode *groupNode = groupsNode->InsertEndChild(TiXmlElement("group"));
			if ( groupNode!=NULL )
			{
				TiXmlNode *node = NULL;

				node = groupNode->InsertEndChild(TiXmlElement("guid"));
				node->InsertEndChild(TiXmlText(iter->getGuid().c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("dbId"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getDbId()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("name"));
				node->InsertEndChild(TiXmlText(iter->getName().c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("disabled"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isDisabled()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("admin"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isAdmin()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("browser"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isBrowser()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("bypassLimits"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isBypassLimits()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("maxBandwidth"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getMaxBandwidth()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("maxBandwidthEnabled"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isMaxBandwidthEnabled()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("maxDownloadBytes"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getMaxDownloadBytes()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("maxDownloadEnabled"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isMaxDownloadEnabled()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("maxDownloadPeriod"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getMaxDownloadPeriod()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("maxSessions"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getMaxSessions()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("maxSessionsEnabled"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isMaxSessionsEnabled()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("maxSessionsPerIp"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getMaxSessionsPerIp()).c_str()));

				node = groupNode->InsertEndChild(TiXmlElement("maxSessionsPerIpEnabled"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isMaxSessionsPerIpEnabled()).c_str()));

				TiXmlNode *optionsNode = groupNode->InsertEndChild(TiXmlElement("options"));
				if ( optionsNode!=NULL )
				{
					const std::map<std::string,std::string> &options = iter->getOptions();
					std::map<std::string,std::string>::const_iterator optionIter;
					for ( optionIter=options.begin(); optionIter!=options.end(); optionIter++ ) {
						node = optionsNode->InsertEndChild(TiXmlElement("option"));
						node->ToElement()->SetAttribute("name",optionIter->first.c_str());
						node->InsertEndChild(TiXmlText(optionIter->second.c_str()));
					}
				}

				TiXmlNode *membersNode = groupNode->InsertEndChild(TiXmlElement("members"));
				if ( membersNode!=NULL )
				{
					const std::list<std::string> &userGuids = iter->getUsers();
					std::list<std::string>::const_iterator userGuidIter;
					for ( userGuidIter=userGuids.begin(); userGuidIter!=userGuids.end(); userGuidIter++ ) {
						node = membersNode->InsertEndChild(TiXmlElement("userGuid"));
						node->InsertEndChild(TiXmlText(userGuidIter->c_str()));
					}
				}
			}
		}
	}

	if ( document.FirstChild()!=NULL && document.FirstChild()->ToDeclaration()==NULL ) {
		document.InsertBeforeChild(document.FirstChild(),TiXmlDeclaration("1.0","iso-8859-1","yes"));
	}

	if ( !document.SaveFile() ) {
		return 1;
	}

	return 0;
}

bool UserManager::prepareDbEntry(User &user)
{
	bool success = false;

	DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_SERVER,true);
	if ( conn!=NULL )
	{
		if ( user.getDbId()>0 )
		{
			try	{
				conn->getSqliteConn().executeint64(
					"SELECT userId FROM [users] WHERE userId=" + Util::ConvertUtil::toString(user.getDbId()));

				success = true;
			}
			catch(exception &ex) {
				LogManager::getInstance()->warning(LOGGER_CLASSNAME,
					"Could not find database entry for user '%s' [%s]",user.getName().c_str(),ex.what());
			}
		}

		if ( !success )
		{
			try	{
				conn->getSqliteConn().executenonquery("INSERT INTO [users] (guid) "
					"VALUES ('" + conn->quote(user.getGuid()) + "')");

				user.setDbId(conn->getSqliteConn().insertid());
				success = true;
			}
			catch(exception &ex) {
				LogManager::getInstance()->warning(LOGGER_CLASSNAME,
					"Could not create database entry for user '%s' [%s]",user.getName().c_str(),ex.what());
			}
		}

		DatabaseManager::getInstance()->releaseConnection(conn);
	}

	return success;
}

bool UserManager::prepareDbEntry(Group &group)
{
	bool success = false;

	DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_SERVER,true);
	if ( conn!=NULL )
	{
		if ( group.getDbId()>0 )
		{
			try	{
				conn->getSqliteConn().executeint64(
					"SELECT groupId FROM [groups] WHERE groupId=" + Util::ConvertUtil::toString(group.getDbId()));

				success = true;
			}
			catch(exception &ex) {
				LogManager::getInstance()->warning(LOGGER_CLASSNAME,
					"Could not find database entry for group '%s' [%s]",group.getName().c_str(),ex.what());
			}
		}

		if ( !success )
		{
			try	{
				conn->getSqliteConn().executenonquery("INSERT INTO [groups] (guid) "
					"VALUES ('" + conn->quote(group.getGuid()) + "')");

				group.setDbId(conn->getSqliteConn().insertid());
				success = true;
			}
			catch(exception &ex) {
				LogManager::getInstance()->warning(LOGGER_CLASSNAME,
					"Could not create database entry for group '%s' [%s]",group.getName().c_str(),ex.what());
			}
		}

		DatabaseManager::getInstance()->releaseConnection(conn);
	}

	return success;
}

void UserManager::deleteDbEntry(const Group &group)
{
	DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_SERVER,true);
	if ( conn!=NULL ) 
	{
		try {
			conn->getSqliteConn().executenonquery("DELETE FROM [groups] WHERE groupId=" 
				+ Util::ConvertUtil::toString(group.getDbId()));
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,
				"Could not delete database entry for group '%s' [%s]",group.getName().c_str(),ex.what());
		}

		DatabaseManager::getInstance()->releaseConnection(conn);
	}
}

void UserManager::deleteDbEntry(const User &user)
{
	DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_SERVER,true);
	if ( conn!=NULL ) 
	{
		try {
			conn->getSqliteConn().executenonquery("DELETE FROM [users] WHERE userId=" 
				+ Util::ConvertUtil::toString(user.getDbId()));
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,
				"Could not delete database entry for user '%s' [%s]",user.getName().c_str(),ex.what());
		}

		DatabaseManager::getInstance()->releaseConnection(conn);
	}
}

bool UserManager::addGroup(Group &group)
{
	group.setGuid(Util::CryptoUtil::generateGuid());

	if ( prepareDbEntry(group) )
	{
		group.setManager(this);

		m_mutex.acquire();
		updateMemberships(group,group.getUsers());
		m_groups.push_back(group);
		m_mutex.release();

		LogManager::getInstance()->info(LOGGER_CLASSNAME,"Added group '%s'",group.getName().c_str());
		fireEvent(UserManagerListener::GroupAdded(),group);

		return false;
	}

	return true;
}

bool UserManager::removeGroup(const Group &group)
{
	Group *removedGroup = NULL;

	m_mutex.acquire();

	// remove group from manager
	std::list<Group>::iterator iter;
	for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) 
	{
		if ( iter->getGuid()==group.getGuid() ) {
			removedGroup = new Group(*iter);
			m_groups.erase(iter);
			break;
		}
	}

	m_mutex.release();

	if ( removedGroup==NULL ) {
		return false;
	}

	removeMemberships(*removedGroup);

	deleteDbEntry(*removedGroup);

	LogManager::getInstance()->info(LOGGER_CLASSNAME,"Removed group '%s'",removedGroup->getName().c_str());
	fireEvent(UserManagerListener::GroupRemoved(),*removedGroup);

	delete removedGroup;

	return true;
}

bool UserManager::updateGroup(Group &group)
{
	Group *managedGroup = NULL;

	// make sure there are transactions ready to be made
	const unsigned int transactions = group.getTransactions();
	if ( transactions==Group::TRANSACTION_NONE ) {
		return false;
	}

	m_mutex.acquire();

	// find the managed group
	std::list<Group>::iterator iter;
	for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) {
		if ( iter->getGuid()==group.getGuid() ) {
			managedGroup = &(*iter);
			break;
		}
	}

	if ( managedGroup==NULL ) {
		m_mutex.release();
		return false;
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETADMIN ) {
		managedGroup->setAdmin(group.isAdmin());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETBROWSER ) {
		managedGroup->setBrowser(group.isBrowser());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETBYPASSLIMITS ) {
		managedGroup->setBypassLimits(group.isBypassLimits());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETDISABLED ) {
		managedGroup->setDisabled(group.isDisabled());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETMAXBANDWIDTH ) {
		managedGroup->setMaxBandwidth(group.getMaxBandwidth());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETMAXBANDWIDTHENABLED ) {
		managedGroup->setMaxBandwidthEnabled(group.isMaxBandwidthEnabled());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETMAXDOWNLOADBYTES ) {
		managedGroup->setMaxDownloadBytes(group.getMaxDownloadBytes());	
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETMAXDOWNLOADENABLED ) {
		managedGroup->setMaxDownloadEnabled(group.isMaxDownloadEnabled());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETMAXDOWNLOADPERIOD ) {
		managedGroup->setMaxDownloadPeriod(group.getMaxDownloadPeriod());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETMAXSESSIONS ) {
		managedGroup->setMaxSessions(group.getMaxSessions());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETMAXSESSIONSENABLED ) {
		managedGroup->setMaxSessionsEnabled(group.isMaxSessionsEnabled());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETMAXSESSIONSPERIP ) {
		managedGroup->setMaxSessionsPerIp(group.getMaxSessionsPerIp());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETMAXSESSIONSPERIPENABLED ) {
		managedGroup->setMaxSessionsPerIpEnabled(group.isMaxSessionsPerIpEnabled());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETNAME ) {
		managedGroup->setName(group.getName());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETOPTIONS ) {
		managedGroup->setOptions(group.getOptions());
	}

	if ( group.getTransactions() & Group::TransactionType::TRANSACTION_SETUSERS ) {
		updateMemberships(*managedGroup,group.getUsers());
	}

	group.setTransactions(Group::TRANSACTION_NONE);
	managedGroup->setTransactions(Group::TRANSACTION_NONE);

	Group updatedGroup = *managedGroup;

	m_mutex.release();

	fireEvent(UserManagerListener::GroupUpdated(),updatedGroup);

	return true;
}

bool UserManager::addUser(User &user)
{
	user.setGuid(Util::CryptoUtil::generateGuid());

	if ( prepareDbEntry(user) )
	{
		user.setManager(this);

		m_mutex.acquire();
		updateMemberships(user,user.getGroups());
		m_users.push_back(user);
		m_mutex.release();

		LogManager::getInstance()->info(LOGGER_CLASSNAME,"Added user '%s'",user.getName().c_str());
		fireEvent(UserManagerListener::UserAdded(),user);

		return true;
	}
	
	return false;
}

bool UserManager::removeUser(const User &user)
{
	User *removedUser = NULL;

	m_mutex.acquire();

	// remove user from manager
	std::list<User>::iterator iter;
	for ( iter=m_users.begin(); iter!=m_users.end(); iter++ ) 
	{
		if ( iter->getGuid()==user.getGuid() ) {
			removedUser = new User(*iter);
			m_users.erase(iter);
			break;
		}
	}

	m_mutex.release();

	if ( removedUser==NULL ) {
		return false;
	}

	removeMemberships(*removedUser);

	deleteDbEntry(*removedUser);

	LogManager::getInstance()->info(LOGGER_CLASSNAME,"Removed user '%s'",removedUser->getName().c_str());
	fireEvent(UserManagerListener::UserRemoved(),*removedUser);

	delete removedUser;

	return true;
}

bool UserManager::updateUser(User &user)
{
	User *managedUser = NULL;

	// make sure there are transactions ready to be made
	const unsigned int transactions = user.getTransactions();
	if ( transactions==User::TRANSACTION_NONE ) {
		return false;
	}

	m_mutex.acquire();

	// find the managed user
	std::list<User>::iterator iter;
	for ( iter=m_users.begin(); iter!=m_users.end(); iter++ ) {
		if ( iter->getGuid()==user.getGuid() ) {
			managedUser = &(*iter);
			break;
		}
	}

	if ( managedUser==NULL ) {
		m_mutex.release();
		return false;
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETADMIN ) {
		managedUser->setAdmin(user.isAdmin());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETBROWSER ) {
		managedUser->setBrowser(user.isBrowser());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETBYPASSLIMITS ) {
		managedUser->setBypassLimits(user.isBypassLimits());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETDISABLED ) {
		managedUser->setDisabled(user.isDisabled());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETGROUPS ) {
		updateMemberships(*managedUser,user.getGroups());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETLASTKNOWNIP ) {
		managedUser->setLastKnownIp(user.getLastKnownIp());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETLASTLOGINTIME ) {
		managedUser->setLastLoginTime(user.getLastLoginTime());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETLOGINS ) {
		managedUser->setLogins(user.getLogins());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETMAXBANDWIDTH ) {
		managedUser->setMaxBandwidth(user.getMaxBandwidth());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETMAXBANDWIDTHENABLED ) {
		managedUser->setMaxBandwidthEnabled(user.isMaxBandwidthEnabled());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETMAXDOWNLOADBYTES ) {
		managedUser->setMaxDownloadBytes(user.getMaxDownloadBytes());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETMAXDOWNLOADENABLED ) {
		managedUser->setMaxDownloadEnabled(user.isMaxDownloadEnabled());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETMAXDOWNLOADPERIOD ) {
		managedUser->setMaxDownloadPeriod(user.getMaxDownloadPeriod());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETMAXSESSIONS ) {
		managedUser->setMaxSessions(user.getMaxSessions());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETMAXSESSIONSENABLED ) {
		managedUser->setMaxSessionsEnabled(user.isMaxSessionsEnabled());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETMAXSESSIONSPERIP ) {
		managedUser->setMaxSessionsPerIp(user.getMaxSessionsPerIp());		
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETMAXSESSIONSPERIPENABLED ) {
		managedUser->setMaxSessionsPerIpEnabled(user.isMaxSessionsPerIpEnabled());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETNAME ) {
		managedUser->setName(user.getName());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETOPTIONS ) {
		managedUser->setOptions(user.getOptions());
	}

	if ( user.getTransactions() & User::TransactionType::TRANSACTION_SETPASSWORD ) {
		managedUser->setEncryptedPassword(user.getPassword());
	}

	user.setTransactions(User::TRANSACTION_NONE);
	managedUser->setTransactions(User::TRANSACTION_NONE);

	User updatedUser = *managedUser;

	m_mutex.release();

	fireEvent(UserManagerListener::UserUpdated(),updatedUser);

	return true;
}

bool UserManager::findGroupByDbId(uint64_t dbId,Group *group)
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<Group>::iterator iter;
	for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ )
	{
		if ( iter->getDbId()==dbId )
		{
			if ( group!=NULL ) {
				*group = *iter;
			}

			return true;
		}
	}

	return false;
}

bool UserManager::findGroupByGuid(const std::string &guid,Group *group)
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<Group>::iterator iter;
	for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ )
	{
		if ( iter->getGuid()==guid )
		{
			if ( group!=NULL ) {
				*group = *iter;
			}

			return true;
		}
	}

	return false;
}

bool UserManager::findGroupByName(const std::string &name,Group *group)
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<Group>::iterator iter;
	for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ )
	{
		if ( boost::iequals(name,iter->getName()) ) 
		{
			if ( group!=NULL ) {
				*group = *iter;
			}

			return true;
		}
	}

	return false;
}

bool UserManager::findUserByDbId(uint64_t dbId,User *user)
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<User>::iterator iter;
	for ( iter=m_users.begin(); iter!=m_users.end(); iter++ )
	{
		if ( iter->getDbId()==dbId ) 
		{
			if ( user!=NULL ) {
				*user = *iter;
			}

			return true;
		}
	}

	return false;
}

bool UserManager::findUserByGuid(const std::string &guid,User *user)
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<User>::iterator iter;
	for ( iter=m_users.begin(); iter!=m_users.end(); iter++ )
	{
		if ( iter->getGuid()==guid ) 
		{
			if ( user!=NULL ) {
				*user = *iter;
			}

			return true;
		}
	}

	return false;
}

bool UserManager::findUserByName(const std::string &name,User *user)
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<User>::iterator iter;
	for ( iter=m_users.begin(); iter!=m_users.end(); iter++ )
	{
		if ( boost::iequals(name,iter->getName()) ) 
		{
			if ( user!=NULL ) {
				*user = *iter;
			}

			return true;
		}
	}

	return false;
}

std::list<Group> UserManager::getGroups()
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<Group> groups;

	std::list<Group>::iterator iter;
	for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ ) {
		groups.push_back(*iter);
	}

	return groups;
}

std::list<User> UserManager::getUsers()
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<User> users;

	std::list<User>::iterator iter;
	for ( iter=m_users.begin(); iter!=m_users.end(); iter++ ) {
		users.push_back(*iter);
	}

	return users;
}

void UserManager::updateMemberships(Group &group,std::list<std::string> users)
{
	// add group to the given users
	std::list<std::string>::iterator iter;
	for ( iter=users.begin(); iter!=users.end(); )
	{
		User *managedUser = NULL;

		// make sure the user exists
		std::list<User>::iterator userIter;
		for ( userIter=m_users.begin(); userIter!=m_users.end(); userIter++ )
		{
			if ( userIter->getGuid()==*iter ) {
				managedUser = &*userIter;
				break;
			}
		}

		if ( managedUser==NULL ) {
			iter = users.erase(iter);
		}
		else
		{
			// add group to the found user
			if ( !managedUser->isMemberOf(group.getGuid()) ) {
				managedUser->addGroup(group.getGuid());
			}

			iter++;
		}
	}

	// remove group from the users that are no longer members
	std::list<User>::iterator userIter;
	for ( userIter=m_users.begin(); userIter!=m_users.end(); userIter++ ) 
	{
		if ( std::find(users.begin(),users.end(),userIter->getGuid())!=users.end() ) {
			continue;
		}

		userIter->removeGroup(group.getGuid());
	}

	group.setUsers(users);
}

void UserManager::updateMemberships(User &user,std::list<std::string> groups)
{
	// add user to the given groups
	std::list<std::string>::iterator iter;
	for ( iter=groups.begin(); iter!=groups.end(); )
	{
		Group *managedGroup = NULL;

		// make sure the group exists
		std::list<Group>::iterator groupIter;
		for ( groupIter=m_groups.begin(); groupIter!=m_groups.end(); groupIter++ )
		{
			if ( groupIter->getGuid()==*iter ) {
				managedGroup = &*groupIter;
				break;
			}
		}

		if ( managedGroup==NULL ) {
			iter = groups.erase(iter);
		}
		else
		{
			// add user to the found group
			if ( !managedGroup->isMember(user.getGuid()) ) {
				managedGroup->addUser(user.getGuid());
			}

			iter++;
		}
	}

	// remove user from groups that the user is no longer a member of
	std::list<Group>::iterator groupIter;
	for ( groupIter=m_groups.begin(); groupIter!=m_groups.end(); groupIter++ ) 
	{
		if ( std::find(groups.begin(),groups.end(),groupIter->getGuid())!=groups.end() ) {
			continue;
		}

		groupIter->removeUser(user.getGuid());
	}

	user.setGroups(groups);
}

void UserManager::removeMemberships(const Group &group)
{
	std::list<User>::iterator iter;
	for ( iter=m_users.begin(); iter!=m_users.end(); iter++ )
	{
		if ( iter->isMemberOf(group.getGuid()) ) {
			iter->removeGroup(group.getGuid());
		}
	}
}

void UserManager::removeMemberships(const User &user)
{
	std::list<Group>::iterator iter;
	for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ )
	{
		if ( iter->isMember(user.getGuid()) ) {
			iter->removeUser(user.getGuid());
		}
	}
}
