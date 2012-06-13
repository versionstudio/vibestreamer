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
#include "sharemanager.h"

#define LOGGER_CLASSNAME "ShareManager"

#include "../tinyxml/tinyxml.h"

#include "databasemanager.h"
#include "logmanager.h"
#include "usermanager.h"

int ShareManager::load()
{
	LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Loading");

	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	m_shares.clear();

	TiXmlDocument document;
	document.LoadFile("conf\\shares.xml");
	TiXmlNode *sharesNode = document.FirstChildElement("shares");
	if ( sharesNode!=NULL )
	{
		// create all share instances
		TiXmlNode *shareNode = sharesNode->FirstChildElement("share");
		while ( shareNode!=NULL )
		{
			Share share;

			TiXmlNode *node = NULL;

			node = shareNode->FirstChild("guid");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				share.setGuid(node->FirstChild()->Value());
			}

			node = shareNode->FirstChild("dbId");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				share.setDbId(Util::ConvertUtil::toUnsignedInt64(node->FirstChild()->Value()));
			}

			node = shareNode->FirstChild("name");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				share.setName(node->FirstChild()->Value());
			}

			node = shareNode->FirstChild("path");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				share.setPath(node->FirstChild()->Value());
			}

			node = shareNode->FirstChild("autoIndex");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				share.setAutoIndex(true);
			}

			node = shareNode->FirstChild("autoIndexInterval");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				share.setAutoIndexInterval(Util::ConvertUtil::toUnsignedInt64(node->FirstChild()->Value()));
			}

			node = shareNode->FirstChild("lastIndexedTime");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				share.setLastIndexedTime(Util::ConvertUtil::toUnsignedInt64(node->FirstChild()->Value()));
			}

			TiXmlNode *permissionsNode = shareNode->FirstChildElement("permissions");
			if ( permissionsNode!=NULL )
			{
				std::list<Permission> permissions;
				TiXmlNode *permissionNode = permissionsNode->FirstChildElement("permission");
				while ( permissionNode!=NULL )
				{
					std::string userGuid;
					std::string groupGuid;
					std::string remoteAddress;
					
					bool allowed = false;
					bool validPermission = true;

					node = permissionNode->FirstChild("userGuid");
					if ( node!=NULL && node->FirstChild()!=NULL ) {
						userGuid = node->FirstChild()->Value();
					}

					node = permissionNode->FirstChild("groupGuid");
					if ( node!=NULL && node->FirstChild()!=NULL ) {
						groupGuid = node->FirstChild()->Value();
					}

					node = permissionNode->FirstChild("remoteAddress");
					if ( node!=NULL && node->FirstChild()!=NULL ) {
						remoteAddress = node->FirstChild()->Value();
					}

					node = permissionNode->FirstChild("allowed");
					if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
						allowed = true;
					}

					// make sure specified user exists
					if ( !userGuid.empty() && !UserManager::getInstance()->findUserByGuid(userGuid,NULL) ) {
						validPermission = false;
					}

					// make sure specified group exists
					if ( !groupGuid.empty() && !UserManager::getInstance()->findGroupByGuid(groupGuid,NULL) ) {
						validPermission = false;
					}

					if ( validPermission ) {
						permissions.push_back(Permission(userGuid,groupGuid,remoteAddress,allowed));
					}

					permissionNode = permissionsNode->IterateChildren("permission",permissionNode);
				}

				share.setPermissions(permissions);
			}

			share.setTransactions(Share::TRANSACTION_NONE);

			prepareDbEntry(share);

			share.setManager(this);
			m_shares.push_back(share);

			shareNode = sharesNode->IterateChildren("share",shareNode);
		}
	}

	return 0;
}

int ShareManager::save()
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Saving");

	TiXmlDocument document;
	document.LoadFile("conf\\shares.xml");
	TiXmlNode *sharesNode = document.FirstChildElement("shares");
	if ( sharesNode==NULL ) {
		sharesNode = document.InsertEndChild(TiXmlElement("shares"));
	}

	if ( sharesNode!=NULL )
	{
		sharesNode->Clear();

		// save all share instances
		std::list<Share>::iterator iter;
		for ( iter=m_shares.begin(); iter!=m_shares.end(); iter++ ) 
		{
			TiXmlNode *shareNode = sharesNode->InsertEndChild(TiXmlElement("share"));
			if ( shareNode!=NULL )	
			{
				TiXmlNode *node = NULL;

				node = shareNode->InsertEndChild(TiXmlElement("guid"));
				node->InsertEndChild(TiXmlText(iter->getGuid().c_str()));

				node = shareNode->InsertEndChild(TiXmlElement("dbId"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getDbId()).c_str()));

				node = shareNode->InsertEndChild(TiXmlElement("name"));
				node->InsertEndChild(TiXmlText(iter->getName().c_str()));

				node = shareNode->InsertEndChild(TiXmlElement("path"));
				node->InsertEndChild(TiXmlText(iter->getPath().c_str()));

				node = shareNode->InsertEndChild(TiXmlElement("autoIndex"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isAutoIndex()).c_str()));

				node = shareNode->InsertEndChild(TiXmlElement("autoIndexInterval"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getAutoIndexInterval()).c_str()));

				node = shareNode->InsertEndChild(TiXmlElement("lastIndexedTime"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->getLastIndexedTime()).c_str()));

				TiXmlNode *permissionsNode = shareNode->InsertEndChild(TiXmlElement("permissions"));
				if ( permissionsNode!=NULL ) 
				{
					std::list<Permission> permissions = iter->getPermissions();
					std::list<Permission>::const_iterator iter;
					for ( iter=permissions.begin(); iter!=permissions.end(); iter++ ) 
					{
						TiXmlNode *permissionNode = permissionsNode->InsertEndChild(TiXmlElement("permission"));
						if ( permissionNode!=NULL )
						{
							TiXmlNode *node = NULL;

							node = permissionNode->InsertEndChild(TiXmlElement("userGuid"));
							node->InsertEndChild(TiXmlText(iter->getUserGuid().c_str()));

							node = permissionNode->InsertEndChild(TiXmlElement("groupGuid"));
							node->InsertEndChild(TiXmlText(iter->getGroupGuid().c_str()));

							node = permissionNode->InsertEndChild(TiXmlElement("remoteAddress"));
							node->InsertEndChild(TiXmlText(iter->getRemoteAddress().c_str()));

							node = permissionNode->InsertEndChild(TiXmlElement("allowed"));
							node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isAllowed()).c_str()));
						}
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

bool ShareManager::prepareDbEntry(Share &share)
{
	bool success = false;

	DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_SERVER,true);
	if ( conn!=NULL )
	{
		if ( share.getDbId()>0 )
		{
			try	{
				conn->getSqliteConn().executeint64(
					"SELECT shareId FROM [shares] WHERE shareId=" + Util::ConvertUtil::toString(share.getDbId()));

				success = true;
			}
			catch(exception &ex) {
				LogManager::getInstance()->warning(LOGGER_CLASSNAME,
					"Could not find database entry for share '%s' [%s]",share.getName().c_str(),ex.what());
			}
		}

		if ( !success )
		{
			try	{
				conn->getSqliteConn().executenonquery("INSERT INTO [shares] (guid) "
					"VALUES ('" + conn->quote(share.getGuid()) + "')");

				share.setDbId(conn->getSqliteConn().insertid());
				success = true;
			}
			catch(exception &ex) {
				LogManager::getInstance()->warning(LOGGER_CLASSNAME,
					"Could not create database entry for share '%s' [%s]",share.getName().c_str(),ex.what());
			}
		}

		DatabaseManager::getInstance()->releaseConnection(conn);
	}

	return success;
}

void ShareManager::deleteDbEntry(const Share &share)
{
	DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_SERVER,true);
	if ( conn!=NULL )
	{
		try {
			conn->getSqliteConn().executenonquery("DELETE FROM [shares] WHERE shareId=" 
				+ Util::ConvertUtil::toString(share.getDbId()));
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,
				"Could not delete database entry for share '%s' [%s]",share.getName().c_str(),ex.what());
		}

		DatabaseManager::getInstance()->releaseConnection(conn);
	}
}

bool ShareManager::addShare(Share &share)
{
	share.setGuid(Util::CryptoUtil::generateGuid());

	if ( prepareDbEntry(share) )
	{
		share.setManager(this);

		m_mutex.acquire();
		m_shares.push_back(share);
		m_mutex.release();

		LogManager::getInstance()->info(LOGGER_CLASSNAME,"Added share '%s'",share.getName().c_str());
		fireEvent(ShareManagerListener::ShareAdded(),share);

		return true;
	}

	return false;
}

bool ShareManager::removeShare(const Share &share)
{
	Share *removedShare = NULL;

	m_mutex.acquire();

	std::list<Share>::iterator iter;
	for ( iter=m_shares.begin(); iter!=m_shares.end(); iter++ )
	{
		if ( iter->getGuid()==share.getGuid() ) {
			removedShare = new Share(*iter);
			m_shares.erase(iter);
			break;
		}
	}

	m_mutex.release();

	if ( removedShare==NULL ) {
		return false;
	}

	deleteDbEntry(*removedShare);

	LogManager::getInstance()->info(LOGGER_CLASSNAME,"Removed share '%s'",removedShare->getName().c_str());
	fireEvent(ShareManagerListener::ShareRemoved(),*removedShare);

	delete removedShare;

	return true;
}

bool ShareManager::updateShare(Share &share)
{
	Share *managedShare = NULL;

	// make sure there are transactions ready to be made
	const unsigned int transactions = share.getTransactions();
	if ( transactions==Share::TRANSACTION_NONE ) {
		return false;
	}

	m_mutex.acquire();

	// find the managed share
	std::list<Share>::iterator iter;
	for ( iter=m_shares.begin(); iter!=m_shares.end(); iter++ ) {
		if ( iter->getGuid()==share.getGuid() ) {
			managedShare = &(*iter);
			break;
		}
	}

	if ( managedShare==NULL ) {
		m_mutex.release();
		return false;
	}

	if ( transactions & Share::TRANSACTION_SETAUTOINDEX ) {
		managedShare->setAutoIndex(share.isAutoIndex());
	}

	if ( transactions & Share::TRANSACTION_SETAUTOINDEXINTERVAL ) {
		managedShare->setAutoIndexInterval(share.getAutoIndexInterval());
	}

	if ( transactions & Share::TRANSACTION_SETDIRECTORIES ) {
		managedShare->setDirectories(share.getDirectories());
	}

	if ( transactions & Share::TRANSACTION_SETFILES ) {
		managedShare->setFiles(share.getFiles());
	}

	if ( transactions & Share::TRANSACTION_SETLASTINDEXEDTIME ) {
		managedShare->setLastIndexedTime(share.getLastIndexedTime());
	}

	if ( transactions & Share::TRANSACTION_SETNAME ) {
		managedShare->setName(share.getName());
	}

	if ( transactions & Share::TRANSACTION_SETPATH ) {
		managedShare->setPath(share.getPath());
	}

	if ( transactions & Share::TRANSACTION_SETPERMISSIONS ) {
		managedShare->setPermissions(share.getPermissions());
	}

	if ( transactions & Share::TRANSACTION_SETSIZE ) {
		managedShare->setSize(share.getSize());
	}

	share.setTransactions(Share::TRANSACTION_NONE);
	managedShare->setTransactions(Share::TRANSACTION_NONE);

	Share updatedShare = *managedShare;

	m_mutex.release();

	fireEvent(ShareManagerListener::ShareUpdated(),updatedShare);

	return true;
}

bool ShareManager::findShareByDbId(uint64_t dbId,Share *share)
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<Share>::iterator iter;
	for ( iter=m_shares.begin(); iter!=m_shares.end(); iter++ ) 
	{
		if ( iter->getDbId()==dbId ) 
		{
			if ( share!=NULL ) {
				*share = *iter;
			}

			return true;
		}
	}

	return false;
}

bool ShareManager::findShareByGuid(const std::string &guid,Share *share)
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<Share>::iterator iter;
	for ( iter=m_shares.begin(); iter!=m_shares.end(); iter++ ) 
	{
		if ( iter->getGuid()==guid ) 
		{
			if ( share!=NULL ) {
				*share = *iter;
			}

			return true;
		}
	}

	return false;
}

bool ShareManager::findShareByName(const std::string &name,Share *share)
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<Share>::iterator iter;
	for ( iter=m_shares.begin(); iter!=m_shares.end(); iter++ ) 
	{
		if ( boost::iequals(name,iter->getName()) )
		{
			if ( share!=NULL ) {
				*share = *iter;
			}

			return true;
		}
	}

	return false;
}

std::list<Share> ShareManager::getShares()
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<Share> shares;

	std::list<Share>::iterator iter;
	for ( iter=m_shares.begin(); iter!=m_shares.end(); iter++ ) {
		shares.push_back(*iter);
	}

	return shares;
}

std::list<Share> ShareManager::getSharesToIndex()
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	std::list<Share> shares;

	time_t currentTime = Util::TimeUtil::getCalendarTime();

	std::list<Share>::iterator iter;
	for ( iter=m_shares.begin(); iter!=m_shares.end(); iter++ ) 
	{
		if ( iter->getLastIndexedTime()==0 ) {
			shares.push_back(*iter);
		}
		else if ( iter->isAutoIndex() 
			&& difftime(currentTime,iter->getLastIndexedTime())>(iter->getAutoIndexInterval()*60) ) {
			shares.push_back(*iter);
		}
	}

	return shares;
}
