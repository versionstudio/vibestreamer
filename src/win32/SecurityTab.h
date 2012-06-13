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

#ifndef guard_securitytab_h
#define guard_securitytab_h

#include "../server/sitemanager.h"

#include "dialogwindow.h"

class SecurityTabSiteContainer
{
public:
	SecurityTabSiteContainer(Site *pSite,bool containerVisible) {
		m_pSite = pSite;
		m_containerVisible = containerVisible;
	}

	Site *getSite() {
		return m_pSite;
	}

	const std::list<Permission>& getPermissions() {
		return m_permissions;
	}

	const bool isContainerVisible() const {
		return m_containerVisible;
	}

	void setContainerVisible(bool containerVisible) {
		m_containerVisible = containerVisible;
	}

	void setPermissions(const std::list<Permission> &permissions) {
		m_permissions = permissions;
	}

private:
	std::list<Permission> m_permissions;

	Site *m_pSite;

	bool m_containerVisible;
};

class SecurityTab : public DialogWindow
{
public:
	virtual INT CALLBACK dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual void dialogInit(HWND hWnd,LPARAM lParam);
	virtual void dialogRefresh();

private:
	LRESULT customDrawListItems(LPARAM lParam);

	void openPermissionDialog(Site *pSite,Permission *pPermission);	

	void toggle(int siteIndex);

	bool checkSite(int index);

	Site* findOwnerSiteByIndex(int index);

	std::list<SecurityTabSiteContainer> m_siteContainers;

	HWND m_hListSitePermissions;
	
	HIMAGELIST m_hImageList;
};

#endif
