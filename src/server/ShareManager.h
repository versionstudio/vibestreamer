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

#ifndef guard_sharemanager_h
#define guard_sharemanager_h

#include <ace/synch.h>

#include "eventbroadcaster.h"
#include "persistentmanager.h"
#include "share.h"
#include "singleton.h"

/**
* ShareManagerListener.
* Abstract class containing event definitions for the ShareManager class.
*/
class ShareManagerListener
{
public:
	template<int I>	struct X { enum { TYPE = I };  };

	typedef X<0> ShareAdded;
	typedef X<1> ShareRemoved;
	typedef X<2> ShareUpdated;

	/**
	* Fired when a share is added.
	* @param share the share that was added
	*/
	virtual void on(ShareAdded,const Share &share) = 0;

	/**
	* Fired when a share is removed.
	* @param share the share that was removed
	*/
	virtual void on(ShareRemoved,const Share &share) = 0;

	/**
	* Fired when a share is updated.
	* @param share the share that was updated
	*/
	virtual void on(ShareUpdated,const Share &share) = 0;
};

/**
* ShareManager.
* Singleton class that manages all shares.
*/
class ShareManager : public Singleton<ShareManager>,
					 public EventBroadcaster<ShareManagerListener>,
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
	* Add a share to the manager.
	* The added share will be given a guid and a database id.
	* @param share the share to add to the manager
	* @return true if the share was successfully added
	*/
	bool addShare(Share &share);

	/**
	* Remove a share from the manager.
	* @param share the share to remove
	* @return true if the share was successfully removed
	*/
	bool removeShare(const Share &share);

	/**
	* Update the managed share with any 
	* transactions that's been done in the given share instance.
	* @param share the share to update the managed share with
	* @return true if the share was successfully updated
	*/
	bool updateShare(Share &share);

	/**
	* Get a share by database id.
	* @param dbId the database id of the share to look for
	* @param share the out parameter where the found share is returned
	* @return true if a share was found
	*/
	bool findShareByDbId(uint64_t dbId,Share *share);

	/**
	* Get a share by guid.
	* @param guid the guid of the share to look for
	* @param share the out parameter where the found share is returned
	* @return true if a share was found
	*/
	bool findShareByGuid(const std::string &guid,Share *share);

	/**
	* Get a share by name.
	* @param name the name of the share to look for
	* @param share the out parameter where the found share is returned
	* @return true if a share was found
	*/
	bool findShareByName(const std::string &name,Share *share);

	/**
	* Get the number of shares in the manager.
	* @return the number of shares in the manager
	*/
	const size_t getShareCount() {
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex);
		return m_shares.size();
	}

	/**
	* Get all shares.
	* @return a collection of all shares
	*/
	std::list<Share> getShares();

	/**
	* Get all shares ready to be indexed.
	* Checks for any shares that haven't been indexed yet or
	* should be auto indexed.
	* This method is used by the indexer.
	* @return a collection of all shares that should be indexed
	*/
	std::list<Share> getSharesToIndex();

private:
	/**
	* Prepare a database entry for the given share.
	* If no database entry exists for this share, a new one will be created.
	* @param share the share to prepare a database entry for
	* @return true if the database entry was successfully prepared
	*/
	bool prepareDbEntry(Share &share);

	/**
	* Delete the database entry for the given share
	* @param share the share to delete the database entry for
	*/
	void deleteDbEntry(const Share &share);

	ACE_Mutex m_mutex;

	std::list<Share> m_shares;
};

#endif
