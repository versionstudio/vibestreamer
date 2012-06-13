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

#ifndef guard_sitemanager_h
#define guard_sitemanager_h

#include <ace/synch.h>

#include "mutexpool.h"
#include "persistentmanager.h"
#include "singleton.h"
#include "site.h"
/**
* SiteManager.
* Singleton class that manages all sites.
*/
class SiteManager : public Singleton<SiteManager>,
					public PersistentManager
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	SiteManager() {
		m_mutexPool.init(5); // create mutex pool (for locking individual sites)
	}

	/**
	* @override
	*/
	virtual int load();

	/**
	* @override
	*/
	virtual int save();

	/**
	* Lock the given site.
	* Should only be used by a site instance for temporary locking.
	* @param site the site to lock
	*/
	void lockSite(Site *site) {
		m_mutexPool.lock(site);
	}

	/**
	* Release lock on the given site.
	* Should only be used by a site instance for temporary locking.
	* @param site the site to release the lock on
	*/
	void releaseSite(Site *site) {
		m_mutexPool.release(site);
	}

	/**
	* Get the site that the given path belongs to.
	* @return the site that the given path belongs to
	*/
	Site* findSiteByPath(const std::string &path);

	/**
	* Get all sites.
	* @return a collection of all sites
	*/
	std::list<Site>& getSites() {
		return m_sites;
	}

private:
	ACE_Mutex m_mutex;

	MutexPool m_mutexPool;

	std::list<Site> m_sites;
};

#endif
