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

#ifndef guard_share_h
#define guard_share_h

#include "permission.h"

class ShareManager; // forward declaration

/**
* Share.
* An instance of this class is always kept in the ShareManager.
* Any modifications outside of the ShareManager will always be performed 
* on a clone of the instance which should be returned to the ShareManager
* for transaction synchronization.
*/
class Share
{
public:
	enum TransactionType
	{
		TRANSACTION_NONE				 = 0x00000000,
		TRANSACTION_SETAUTOINDEX	     = 0x00000001,
		TRANSACTION_SETAUTOINDEXINTERVAL = 0x00000002,
		TRANSACTION_SETDIRECTORIES		 = 0x00000004,
		TRANSACTION_SETFILES		     = 0x00000008,
		TRANSACTION_SETLASTINDEXEDTIME	 = 0x00000010,
		TRANSACTION_SETNAME				 = 0x00000020,
		TRANSACTION_SETPATH				 = 0x00000040,
		TRANSACTION_SETPERMISSIONS		 = 0x00000080,
		TRANSACTION_SETSIZE				 = 0x00000100
	};

	/**
	* Default constructor.
	* @return instance
	*/
	Share() : m_autoIndex(false),
		m_autoIndexInterval(60),
		m_dbId(0),
		m_directories(0),
		m_files(0),
		m_lastIndexedTime(0),
		m_manager(NULL),
		m_size(0), 
		m_transactions(TransactionType::TRANSACTION_NONE) 
	{
		
	}

	/**
	* Add a permission to the share.
	* @param permission the permission to add
	*/
	void addPermission(const Permission &permission) {
		m_permissions.push_back(permission);
		m_transactions |= TransactionType::TRANSACTION_SETPERMISSIONS;
	}

	/**
	* Check if a connected user has permission to this share.
	* @param user the connected user to check
	* @param remoteAddress the remote ip address the user is connected as
	* @return true if the user has permission
	*/
	bool checkPermission(const User &user,const std::string &remoteAddress) {
		return Permission::checkPermission(m_permissions,user,remoteAddress);
	}

	/**
	* Remove a permission to the share.
	* @param the permission to remove
	*/
	void removePermission(const Permission &permission);

	/**
	* Get the interval at which the share is auto indexed.
	* @return the interval at which the share is auto indexed
	*/
	const int getAutoIndexInterval() const {
		return m_autoIndexInterval;
	}

	/**
	* Get the database id.
	* @return the database id
	*/
	const uint64_t getDbId() const {
		return m_dbId;
	}

	/**
	* Get the number of directories in the share.
	* @return the number of directories in the share
	*/
	const uint64_t getDirectories() const {
		return m_directories;
	}

	/**
	* Get the number of files in the share.
	* @return the number of files in the share
	*/
	const uint64_t getFiles() const {
		return m_files;
	}

	/**
	* Get the guid.
	* @return the guid
	*/
	const std::string getGuid() const { 
		return m_guid; 
	}
	
	/**
	* Get the last time the share was indexed.
	* @return the last time this share was indexed
	*/
	const time_t getLastIndexedTime() const {
		return m_lastIndexedTime;
	}

    /**
     * Get the manager within which this share is valid.
	 * @return the manager within which this share is valid
     */
	ShareManager* getManager() {
		return m_manager;
	}

	/**
	* Get the name of the share.
	* @return the name of the share
	*/
	const std::string getName() const { 
		return m_name; 
	}

	/**
	* Get the path to the share.
	* @return the path to the share
	*/
	const std::string getPath() const {
		return m_path;
	}

	/**
	* Get all permission entries for the share.
	* @return a collection of all permission entries
	*/
	const std::list<Permission>& getPermissions() const {
		return m_permissions;
	}

	/**
	* Get the total size of all files in the share.
	* @return the total size of all files in the share
	*/
	const uint64_t getSize() const {
		return m_size;
	}

	/**
	* Get the transactions mask.
	* @return the transactions mask
	*/
	const unsigned int getTransactions() const {
		return m_transactions;
	}

	/**
	* Get whether auto indexing of the share is enabled.
	* @return true if auto indexing of the share is enabled
	*/
	const bool isAutoIndex() const {
		return m_autoIndex;
	}

	/**
	* Set whether the share should be auto indexed.
	* @param autoIndex true if the share should be auto indexed
	*/
	void setAutoIndex(bool autoIndex) {
		m_autoIndex = autoIndex;
		m_transactions |= TransactionType::TRANSACTION_SETAUTOINDEX;
	}

	/**
	* Set the interval at which the share should be auto indexed.
	* @param autoIndexInterval the interval at which the share should be auto indexed
	*/
	void setAutoIndexInterval(int autoIndexInterval) {
		m_autoIndexInterval = autoIndexInterval;
		m_transactions |= TransactionType::TRANSACTION_SETAUTOINDEXINTERVAL;
	}

	/**
	* Set the database id.
	* This method is not a transaction and is only used upon
	* initial creation of the group.
	* @param dbId the database id
	*/
	void setDbId(uint64_t dbId) {
		m_dbId = dbId;
	}

	/**
	* Set the number of directories in the share.
	* @param directories the number of directories in the share
	*/
	void setDirectories(uint64_t directories) {
		m_directories = directories;
		m_transactions |= TransactionType::TRANSACTION_SETDIRECTORIES;
	}

	/**
	* Set the number of files in the share.
	* @param files the number of files in the share
	*/
	void setFiles(uint64_t files) {
		m_files = files;
		m_transactions |= TransactionType::TRANSACTION_SETFILES;
	}

	/**
	* Set the guid.
	* This method is not a transaction and is only used upon
	* initial creation of the group.
	* @param guid the guid
	*/
	void setGuid(std::string guid) {
		m_guid = guid;
	}

	/**
	* Set the last time the share was indexed.
	* @param lastIndexedTime the last time this share was indexed
	*/
	void setLastIndexedTime(time_t lastIndexedTime) {
		m_lastIndexedTime = lastIndexedTime;
		m_transactions |= TransactionType::TRANSACTION_SETLASTINDEXEDTIME;
	}

	/**
	* Set the manager within which this share is valid.
	* This method is not a transaction and is only used upon
	* initial creation of the share.
	* @param manager the manager within which this share is valid
	*/
	void setManager(ShareManager *manager) {
		m_manager = manager;
	}

	/**
	* Set the name of the share.
	* @param name the name of the share
	*/
	void setName(std::string name) {
		m_name=name;
		m_transactions |= TransactionType::TRANSACTION_SETNAME;
	}

	/**
	* Set the path to the share.
	* @param path the path to the share
	*/
	void setPath(std::string path) { 
		m_path=path;
		m_transactions |= TransactionType::TRANSACTION_SETPATH;
	}

	/**
	* Set all permission entries for the share.
	* @param permissions a collection of all permission entries
	*/
	void setPermissions(const std::list<Permission> &permissions) {
		m_permissions = permissions;
		m_transactions |= TransactionType::TRANSACTION_SETPERMISSIONS;
	}

	/**
	* Set the total size of all files in the share.
	* @param size the total size of all files in the share
	*/
	void setSize(uint64_t size) {
		m_size = size;
		m_transactions |= TransactionType::TRANSACTION_SETSIZE;
	}

	/**
	* Set the transactions mask.
	* @return transactions the transactions mask
	*/
	void setTransactions(unsigned int transactions) {
		m_transactions = transactions;
	}

private:
	ShareManager *m_manager;

	std::list<Permission> m_permissions;

	std::string m_guid;
	std::string m_name;
	std::string m_path;

	time_t m_lastIndexedTime;

	uint64_t m_dbId;
	uint64_t m_directories;
	uint64_t m_files;
	uint64_t m_size;

	unsigned int m_transactions;

	int m_autoIndexInterval;

	bool m_autoIndex;
};

#endif
