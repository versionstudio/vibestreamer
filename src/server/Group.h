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

#ifndef guard_group_h
#define guard_group_h

class UserManager; // forward declaration

/**
* Group.
* An instance of this class is always kept in the UserManager.
* Any modifications outside of the UserManager will always be performed 
* on a clone of the instance which should be returned to the UserManager
* for transaction synchronization.
*/
class Group
{
public:
	enum DownloadPeriod	{
		DAY = 1,
		WEEK = 2,
		MONTH = 3
	};

	enum TransactionType
	{
		TRANSACTION_NONE						= 0x00000000,
		TRANSACTION_SETADMIN					= 0x00000001,
		TRANSACTION_SETBROWSER					= 0x00000002,
		TRANSACTION_SETBYPASSLIMITS				= 0x00000004,
		TRANSACTION_SETDISABLED					= 0x00000008,
		TRANSACTION_SETLISTENER					= 0x00000010,
		TRANSACTION_SETMAXBANDWIDTH				= 0x00000020,
		TRANSACTION_SETMAXBANDWIDTHENABLED		= 0x00000040,
		TRANSACTION_SETMAXDOWNLOADBYTES			= 0x00000080,
		TRANSACTION_SETMAXDOWNLOADENABLED		= 0x00000100,
		TRANSACTION_SETMAXDOWNLOADPERIOD		= 0x00000200,
		TRANSACTION_SETMAXSESSIONS				= 0x00000400,
		TRANSACTION_SETMAXSESSIONSENABLED		= 0x00000800,
		TRANSACTION_SETMAXSESSIONSPERIP			= 0x00001000,
		TRANSACTION_SETMAXSESSIONSPERIPENABLED	= 0x00002000,
		TRANSACTION_SETNAME						= 0x00004000,
		TRANSACTION_SETOPTIONS					= 0x00008000,
		TRANSACTION_SETUSERS					= 0x00010000
	};

	/**
	* Default constructor.
	* @return instance
	*/
	Group() : m_admin(false),
		m_browser(false),
		m_bypassLimits(false),
		m_dbId(0),
		m_disabled(false),
		m_manager(NULL),
		m_maxBandwidth(100),
		m_maxBandwidthEnabled(false),
		m_maxDownloadBytes(0),
		m_maxDownloadEnabled(false),
		m_maxDownloadPeriod(0),
		m_maxSessions(1),
		m_maxSessionsEnabled(false),
		m_maxSessionsPerIp(1),
		m_maxSessionsPerIpEnabled(false),
		m_transactions(TransactionType::TRANSACTION_NONE)
	{

	}

	/**
	* Add a user to the group.
	* @param guid the guid of the user to add to the group
	*/
	void addUser(std::string guid) {
		m_users.push_back(guid);
		m_transactions |= TransactionType::TRANSACTION_SETUSERS;
	}

	/**
	* Remove the option with the given name.
	* @param name the option to remove
	*/
	void removeOption(std::string name);

	/**
	* Remove a user from the group.
	* @param guid the guid of the user to remove from the group
	*/
	void removeUser(std::string guid);

	/**
	* Get the database id.
	* @return the database id
	*/
	const uint64_t getDbId() const {
		return m_dbId;
	}

	/**
	* Get the guid.
	* @return the guid
	*/
	const std::string getGuid() const { 
		return m_guid; 
	}

    /**
     * Get the manager within which this group is valid.
	 * @return the manager within which this group is valid
     */
	UserManager* getManager() {
		return m_manager;
	}

	/** 
	* Get the max allowed bandwidth for the group, measured in kbps.
	* @return the max allowed bandwidth
	*/
	const int getMaxBandwidth() const {
		return m_maxBandwidth;
	}

	/**
	* Get the max allowed download bytes for the group.
	* @return the max allowed download bytes
	*/
	const uint64_t getMaxDownloadBytes() const {
		return m_maxDownloadBytes; 
	}

	/**
	* Get the period that indicates how the max allowed
	* download bytes limit applies.
	* @see HistoryManager::Period for available values.
	* @return the max download period
	*/
	const int getMaxDownloadPeriod() const {
		return m_maxDownloadPeriod; 
	}

	/**
	* Get the max allowed connected sessions for members of the group.
	* @return the max allowed sessions
	*/
	const int getMaxSessions() const {
		return m_maxSessions;
	}

	/**
	* Get the max allowed connected sessions per ip address for members of the group.
	* @return the max allowed sessions per ip
	*/
	const int getMaxSessionsPerIp() const {
		return m_maxSessionsPerIp;
	}

	/**
	* Get the name of the group.
	* @return the name of the group
	*/
	const std::string getName() const { 
		return m_name; 
	}

	/**
	* Get the option with the given name.
	* @param name the option to retrieve
	* @param value out parameter for the found option value
	* @return true if the option was found
	*/
	bool getOption(std::string name,std::string *value);

	/**
	* Get all options.
	* @return all options
	*/
	const std::map<std::string,std::string>& getOptions() const {
		return m_options;
	}

	/**
	* Get the transactions mask.
	* @return the transactions mask
	*/
	const unsigned int getTransactions() const {
		return m_transactions;
	}

	/**
	* Get all users that are a member of the group.
	* @return a collection of user guids of all group members
	*/
	const std::list<std::string>& getUsers() const {
		return m_users;
	}
	
	/**
	* Get whether the group has admin privileges
	* @return true if the group has admin privileges
	*/
	const bool isAdmin() const { 
		return m_admin; 
	}

	/**
	* Get whether the group is only allowed to browse shares.
	* @return true if the group is only allowed to browse shares
	*/
	const bool isBrowser() const { 
		return m_browser; 
	}

	/**
	* Get whether the group can bypass server limits such as
	* maxSessions and maxSessionsPerIp.
	* @return true if the group can bypass server limits
	*/
	const bool isBypassLimits() const { 
		return m_bypassLimits; 
	}

	/**
	* Get whether the group is disabled.
	* @return true if the group is disabled
	*/
	const bool isDisabled() const { 
		return m_disabled;
	}

	/**
	* Get whether max bandwidth limit is enabled for the group.
	* @return true if max bandwidth limit is enabled
	*/
	const bool isMaxBandwidthEnabled() const {
		return m_maxBandwidthEnabled; 
	}

	/**
	* Get whether max download limit is enabled for the group.
	* @return true if max download limit is enabled for the group
	*/
	const bool isMaxDownloadEnabled() const { 
		return m_maxDownloadEnabled;
	}

	/**
	* Get whether max sessions limit is enabled for the group.
	* @return true if max sessions limit is enabled for the group
	*/
	const bool isMaxSessionsEnabled() const { 
		return m_maxSessionsEnabled; 
	}

	/**
	* Get whether max sessions per ip limit is enabled for the group.
	* @return true if max sessions per ip limit is enabled for the group
	*/
	const bool isMaxSessionsPerIpEnabled() const { 
		return m_maxSessionsPerIpEnabled; 
	}

	/**
	* Get whether the given user is a member of the group
	* @param guid the guid of the user to check
	* @return true if the given user is a member of the group
	*/
	const bool isMember(std::string guid);

	/**
	* Set whether the group should have admin privileges
	* @param admin true if the group should have admin privileges
	*/
	void setAdmin(bool admin) {
		m_admin = admin;
		m_transactions |= TransactionType::TRANSACTION_SETADMIN;
	}

	/**
	* Set whether the group should be only allowed to browse shares.
	* @param browser true if the group should be only allowed to browse shares
	*/
	void setBrowser(bool browser) {
		m_browser = browser;
		m_transactions |= TransactionType::TRANSACTION_SETBROWSER;
	}

	/**
	* Set whether the group should be able to bypass server limits.
	* @param bypassLimits true if the group should be able to bypass server limits
	*/
	void setBypassLimits(bool bypassLimits) {
		m_bypassLimits = bypassLimits;
		m_transactions |= TransactionType::TRANSACTION_SETBYPASSLIMITS;
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
	* Set whether the group should be disabled.
	* @param enabled true if the group should be disabled
	*/
	void setDisabled(bool disabled) {
		m_disabled = disabled;
		m_transactions |= TransactionType::TRANSACTION_SETDISABLED;
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
    * Set the manager within which this group is valid.
	* This method is not a transaction and is only used upon
	* initial creation of the group.
	* @param manager the manager within which this group is valid
    */
	void setManager(UserManager *manager) {
		m_manager = manager;
	}

	/**
	* Set the max bandwidth allowed, measured in kbps.
	* @param the max bandwidth allowed
	*/
	void setMaxBandwidth(int maxBandwidth) {
		m_maxBandwidth = maxBandwidth;
		m_transactions |= TransactionType::TRANSACTION_SETMAXBANDWIDTH;
	}

	/**
	* Set the max allowed download bytes.
	* @param maxDownloadBytes the max allowed download bytes
	*/
	void setMaxDownloadBytes(uint64_t maxDownloadBytes) {
		m_maxDownloadBytes = maxDownloadBytes;
		m_transactions |= TransactionType::TRANSACTION_SETMAXDOWNLOADBYTES;
	}

	/**
	* Set whether max bandwidth limit should be enabled.
	* @param true if max bandwidth limit should be enabled
	*/
	void setMaxBandwidthEnabled(bool maxBandwidthEnabled) {
		m_maxBandwidthEnabled = maxBandwidthEnabled;
		m_transactions |= TransactionType::TRANSACTION_SETMAXBANDWIDTHENABLED;
	}

	/**
	* Set whether max download limit should be enabled.
	* @param true if max download limit should be enabled
	*/
	void setMaxDownloadEnabled(bool maxDownloadEnabled) {
		m_maxDownloadEnabled = maxDownloadEnabled;
		m_transactions |= TransactionType::TRANSACTION_SETMAXDOWNLOADENABLED;
	}

	/**
	* Set the period that indicates how the max allowed
	* download bytes limit applies.
	* @see HistoryManager::Period for available values.
	* @param maxDownloadPeriod the max download period
	*/
	void setMaxDownloadPeriod(int maxDownloadPeriod) {
		m_maxDownloadPeriod = maxDownloadPeriod;
		m_transactions |= TransactionType::TRANSACTION_SETMAXDOWNLOADPERIOD;
	}

	/**
	* Set the max allowed connected sessions for members of the group.
	* @param maxSessions the max allowed sessions
	*/
	void setMaxSessions(int maxSessions) {
		m_maxSessions = maxSessions;
		m_transactions |= TransactionType::TRANSACTION_SETMAXSESSIONS;
	}

	/**
	* Set whether max sessions limit should be enabled.
	* @param maxSessionsEnabled true if max sessions limit should be enabled
	*/
	void setMaxSessionsEnabled(bool maxSessionsEnabled) {
		m_maxSessionsEnabled = maxSessionsEnabled;
		m_transactions |= TransactionType::TRANSACTION_SETMAXSESSIONSENABLED;
	}

	/**
	* Set the max allowed connected sessions per ip address
	* @param maxSessionsPerIp the max allowed sessions per ip
	*/
	void setMaxSessionsPerIp(int maxSessionsPerIp) {
		m_maxSessionsPerIp = maxSessionsPerIp;
		m_transactions |= TransactionType::TRANSACTION_SETMAXSESSIONSPERIP;
	}

	/**
	* Set whether max sessions per ip limit should be enabled.
	* @param maxSessionsPerIpEnabled true if max sessions per ip limit should be enabled
	*/
	void setMaxSessionsPerIpEnabled(bool maxSessionsPerIpEnabled) {
		m_maxSessionsPerIpEnabled = maxSessionsPerIpEnabled;
		m_transactions |= TransactionType::TRANSACTION_SETMAXSESSIONSPERIPENABLED;
	}

	/**
	* Set the group name.
	* @param name the group name
	*/
	void setName(std::string name) {
		m_name = name;
		m_transactions |= TransactionType::TRANSACTION_SETNAME;
	}

	/**
	* Set the option with the given name.
	* @param name the option to set
	* @param value the value of the option
	*/
	void setOption(std::string name,std::string value) {
		m_options[name] = value;
		m_transactions |= TransactionType::TRANSACTION_SETOPTIONS;
	}

	/**
	* Set all options.
	* @param options the options
	*/
	void setOptions(const std::map<std::string,std::string>& options) {
		m_options = options;
		m_transactions |= TransactionType::TRANSACTION_SETOPTIONS;
	}

	/**
	* Set the members of the groups.
	* @param users a collection of user guids that should be members of the group
	*/
	void setUsers(std::list<std::string> users) {
		m_users = users;
		m_transactions |= TransactionType::TRANSACTION_SETUSERS;
	}

	/**
	* Set the transactions mask.
	* @param transactions the transactions mask
	*/
	void setTransactions(unsigned int transactions) {
		m_transactions = transactions;
	}

private:
	UserManager *m_manager;

	std::map<std::string,std::string> m_options;

	std::list<std::string> m_users;

	std::string m_guid;
	std::string m_name;

	uint64_t m_dbId;
	uint64_t m_maxDownloadBytes;

	unsigned int m_transactions;

	int m_maxBandwidth;
	int m_maxDownloadPeriod;
	int m_maxSessions;
	int m_maxSessionsPerIp;

	bool m_admin;
	bool m_browser;
	bool m_bypassLimits;
	bool m_disabled;
	bool m_maxBandwidthEnabled;
	bool m_maxDownloadEnabled;
	bool m_maxSessionsEnabled;
	bool m_maxSessionsPerIpEnabled;
};


#endif
