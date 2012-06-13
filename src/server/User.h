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

#ifndef guard_user_h
#define guard_user_h

class UserManager; // forward declaration

/**
* User.
* An instance of this class is always kept in the UserManager.
* Any modifications outside of the UserManager will always be performed 
* on a clone of the instance which should be returned to the UserManager
* for transaction synchronization.
*/
class User
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
		TRANSACTION_SETGROUPS					= 0x00000010,
		TRANSACTION_SETLASTKNOWNIP				= 0x00000020,
		TRANSACTION_SETLASTLOGINTIME			= 0x00000040,
		TRANSACTION_SETLISTENER					= 0x00000080,
		TRANSACTION_SETMAXBANDWIDTH				= 0x00000100,
		TRANSACTION_SETMAXBANDWIDTHENABLED		= 0x00000200,
		TRANSACTION_SETMAXDOWNLOADBYTES			= 0x00000400,
		TRANSACTION_SETMAXDOWNLOADENABLED		= 0x00000800,
		TRANSACTION_SETMAXDOWNLOADPERIOD		= 0x00001000,
		TRANSACTION_SETMAXSESSIONS				= 0x00002000,
		TRANSACTION_SETMAXSESSIONSENABLED		= 0x00004000,
		TRANSACTION_SETMAXSESSIONSPERIP			= 0x00008000,
		TRANSACTION_SETMAXSESSIONSPERIPENABLED	= 0x00010000,
		TRANSACTION_SETNAME						= 0x00020000,
		TRANSACTION_SETLOGINS					= 0x00040000,
		TRANSACTION_SETOPTIONS					= 0x00080000,
		TRANSACTION_SETPASSWORD					= 0x00100000
	};

	/**
	* Default constructor.
	* @return instance
	*/
	User() : m_admin(false),
		m_browser(false),
		m_bypassLimits(false),
		m_dbId(0),
		m_disabled(false),
		m_lastLoginTime(0),
		m_logins(0),
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
	* Add a group membership to the user.
	* @param guid the guid of the group the user should become a member of
	*/
	void addGroup(std::string guid) {
		m_groups.push_back(guid);
		m_transactions |= TransactionType::TRANSACTION_SETGROUPS;
	}

	/**
	* Check whether the given password matches the user password.
	* @param password the non-encoded password to check if it matches
	* @return true if the password matches
	*/
	const bool checkPassword(std::string password) const {
		return Util::CryptoUtil::md5Encode(password.c_str(),password.length())==m_password;
	}

	/**
	* Check whether the user is allowed to download the given file size.
	* This method includes any group memberships.
	* @return true if the user is allowed to download the given file size
	*/
	const bool checkRoleAllotment(uint64_t fileSize) const;

	/**
	* Remove the user from the group.
	* @param guid the guid of the group to remove membership from
	*/
	void removeGroup(std::string guid);

	/**
	* Remove the option with the given name.
	* @param name the option to remove
	*/
	void removeOption(std::string name);

	/**
	* Get the database id.
	* @return the database id
	*/
	const uint64_t getDbId() const { 
		return m_dbId;
	}

	/**
	* Get all groups that the user is a member of.
	* @return a collection of guids of the groups the user is a member of
	*/
	const std::list<std::string>& getGroups() const {
		return m_groups;
	}

	/**
	* Get the guid.
	* @return the guid
	*/
	const std::string getGuid() const { 
		return m_guid; 
	}
	
	/**
	* Get the last known ip the user connected from.
	* @return the last known ip the user connected from
	*/
	const std::string getLastKnownIp() const { 
		return m_lastKnownIp; 
	}

	/**
	* Get the last login time of the user.
	* @return the last login time of the user
	*/
	const time_t getLastLoginTime() const {
		return m_lastLoginTime;
	}

	/**
	* Get the number of times the user has logged on.
	* @return the number of times the user has logged on
	*/
	const int getLogins() const {
		return m_logins; 
	}

    /**
     * Get the manager within which this user is valid.
	 * @return the manager within which this user is valid
     */
	UserManager* getManager() {
		return m_manager;
	}

	/** 
	* Get the max allowed bandwidth for the user, measured in kbps.
	* @return the max allowed bandwidth
	*/
	const int getMaxBandwidth() const {
		return m_maxBandwidth;
	}

	/**
	* Get the max allowed download bytes for the user.
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
	* Get the max allowed connected sessions for the user.
	* @return the max allowed sessions
	*/
	const int getMaxSessions() const {
		return m_maxSessions;
	}

	/**
	* Get the max allowed connected sessions per ip address for the user.
	* @return the max allowed sessions per ip
	*/
	const int getMaxSessionsPerIp() const {
		return m_maxSessionsPerIp;
	}

	/**
	* Get the user name.
	* @ return the user name
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
	* Get the encrypted password for the user.
	* @return the encrypted password for the user
	*/
	const std::string getPassword() const {
		return m_password;
	}

	/** 
	* Get the max allowed bandwidth for the user, measured in kbps.
	* This also includes any group memberships.
	* @return the max allowed bandwidth or -1 if no limit
	*/
	const int getRoleMaxBandwidth() const;

	/**
	* Get the max allowed connected sessions for the user.
	* This also includes any group memberships.
	* @return the max allowed sessions or -1 if no limit
	*/
	const int getRoleMaxSessions() const;

	/**
	* Get the max allowed connected sessions per ip address for the user.
	* This also includes any group memberships.
	* @return the max allowed sessions per ip or -1 if no limit
	*/
	const int getRoleMaxSessionsPerIp() const;

	/**
	* Get the option with the given name.
	* This also includes any group memberships.
	* @param name the option to retrieve
	* @param value out parameter for the found option value
	* @return true if the option was found
	*/
	bool getRoleOption(std::string name,std::string *value);

	/**
	* Get all available options options.
	* This also includes any group memberships.
	* @return all options
	*/
	std::map<std::string,std::string> getRoleOptions();

	/**
	* Get the transactions mask.
	* @return the transactions mask
	*/
	const unsigned int getTransactions() const {
		return m_transactions;
	}

	/**
	* Check whether the user is a member of the given group.
	* @param groupGuid the group guid
	* @return true if the user is a member of the given group
	*/
	const bool isMemberOf(std::string groupGuid) const;

	/**
	* Get whether the user has admin privileges
	* @return true if the user has admin privileges
	*/
	const bool isAdmin() const {
		return m_admin;
	}

	/**
	* Get whether the user is only allowed to browse shares.
	* @return true if the user is only allowed to browse shares
	*/
	const bool isBrowser() const { 
		return m_browser; 
	}

	/**
	* Get whether the user can bypass server limits such as
	* maxSessions and maxSessionsPerIp.
	* @return true if the user can bypass server limits
	*/
	const bool isBypassLimits() const { 
		return m_bypassLimits; 
	}

	/**
	* Get whether the user is disabled.
	* @return true if the user is disabled
	*/
	const bool isDisabled() const { 
		return m_disabled;
	}

	/**
	* Get whether max bandwidth limit is enabled for the user.
	* @return true if max bandwidth limit is enabled
	*/
	const bool isMaxBandwidthEnabled() const { 
		return m_maxBandwidthEnabled;
	}

	/**
	* Get whether max download limit is enabled for the user.
	* @return true if max download limit is enabled for the user
	*/
	const bool isMaxDownloadEnabled() const {
		return m_maxDownloadEnabled; 
	}

	/**
	* Get whether max sessions limit is enabled for the user.
	* @return true if max sessions limit is enabled for the user
	*/
	const bool isMaxSessionsEnabled() const { 
		return m_maxSessionsEnabled; 
	}

	/**
	* Get whether max sessions per ip limit is enabled for the user.
	* @return true if max sessions per ip limit is enabled for the user
	*/
	const bool isMaxSessionsPerIpEnabled() const { 
		return m_maxSessionsPerIpEnabled; 
	}

	/**
	* Get whether the user has admin privileges
	* This also includes any group memberships.
	* @return true if the user has admin privileges
	*/
	const bool isRoleAdmin() const;

	/**
	* Get whether the user is only allowed to browse shares.
	* This also includes any group memberships.
	* @return true if the user is only allowed to browse shares
	*/
	const bool isRoleBrowser() const;

	/**
	* Get whether the user can bypass server limits such as
	* maxSessions and maxSessionsPerIp.
	* This also includes any group memberships.
	* @return true if the user can bypass server limits
	*/
	const bool isRoleBypassLimits() const;

	/**
	* Set whether the user should have admin privileges
	* @param admin true if the user should have admin privileges
	*/
	void setAdmin(bool admin) {
		m_admin = admin;
		m_transactions |= TransactionType::TRANSACTION_SETADMIN;
	}

	/**
	* Set whether the user should be only allowed to browse shares.
	* @param browser true if the user should be only allowed to browse shares
	*/
	void setBrowser(bool browser) {
		m_browser = browser;
		m_transactions |= TransactionType::TRANSACTION_SETBROWSER;
	}

	/**
	* Set whether the user should be able to bypass server limits.
	* @param bypassLimits true if the user should be able to bypass server limits
	*/
	void setBypassLimits(bool bypassLimits) {
		m_bypassLimits = bypassLimits;
		m_transactions |= TransactionType::TRANSACTION_SETBYPASSLIMITS;
	}

	/**
	* Set the database id.
	* This method is not a transaction and is only used upon
	* initial creation of the user.
	* @param dbId the database id
	*/
	void setDbId(uint64_t dbId) {
		m_dbId = dbId;
	}

	/**
	* Set whether the user should be disabled.
	* @param enabled true if the user should be disabled
	*/
	void setDisabled(bool disabled) {
		m_disabled = disabled;
		m_transactions |= TransactionType::TRANSACTION_SETDISABLED;
	}

	/**
	* Set an already encrypted password on the user.
	* @param password the encrypted password
	*/
	void setEncryptedPassword(std::string password) {
		m_password = password;
		m_transactions |= TransactionType::TRANSACTION_SETPASSWORD;
	}

	/**
	* Set the groups the user is a member of.
	* @param groups a collection of group guids that the user should be a member of
	*/
	void setGroups(const std::list<std::string> &groups) {
		m_groups = groups;
		m_transactions |= TransactionType::TRANSACTION_SETGROUPS;
	}

	/**
	* Set the guid.
	* This method is not a transaction and is only used upon
	* initial creation of the user.
	* @param guid the guid
	*/
	void setGuid(std::string guid) {
		m_guid = guid;
	}

	/**
	* Set the last known ip the user connected from.
	* @param lastKnownIp the last known ip the user connected from
	*/
	void setLastKnownIp(std::string lastKnownIp) {
		m_lastKnownIp = lastKnownIp;
		m_transactions |= TransactionType::TRANSACTION_SETLASTKNOWNIP;
	}

	/**
	* Set the last login time of the user.
	* @param lastLoginTime the last login time of the user
	*/
	void setLastLoginTime(time_t lastLoginTime) {
		m_lastLoginTime = lastLoginTime;
		m_transactions |= TransactionType::TRANSACTION_SETLASTLOGINTIME;
	}

	/**
	* Set the number of times the user has logged on.
	* @param logins the number of times the user has logged on
	*/
	void setLogins(int logins) {
		m_logins = logins;
		m_transactions |= TransactionType::TRANSACTION_SETLOGINS;
	}

    /**
    * Set the manager within which this user is valid.
	* This method is not a transaction and is only used upon
	* initial creation of the user.
	* @param manager the manager within which this user is valid
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
	* Set whether max bandwidth limit should be enabled.
	* @param true if max bandwidth limit should be enabled
	*/
	void setMaxBandwidthEnabled(bool maxBandwidthEnabled) {
		m_maxBandwidthEnabled = maxBandwidthEnabled;
		m_transactions |= TransactionType::TRANSACTION_SETMAXBANDWIDTHENABLED;
	}

	/**
	* Set the max allowed download bytes.
	* @param maxDownloadKb the max allowed download bytes
	*/
	void setMaxDownloadBytes(uint64_t maxDownloadBytes) {
		m_maxDownloadBytes = maxDownloadBytes;
		m_transactions |= TransactionType::TRANSACTION_SETMAXDOWNLOADBYTES;
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
	* Set the max allowed connected sessions for the user.
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
	* Set the user name.
	* @param name the user name
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
	* Set the password for the user.
	* The given password will be encrypted before set.
	* @param password the password
	*/
	void setPassword(std::string password) {
		m_password = Util::CryptoUtil::md5Encode(password.c_str(),password.length());
		m_transactions |= TransactionType::TRANSACTION_SETPASSWORD;
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

	std::list<std::string> m_groups;

	std::string m_guid;
	std::string m_lastKnownIp;
	std::string m_name;
	std::string m_password;

	time_t m_lastLoginTime;

	uint64_t m_dbId;
	uint64_t m_maxDownloadBytes;

	unsigned int m_transactions;

	int m_logins;
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
