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

#ifndef guard_userstab_h
#define guard_userstab_h

#include "dialogwindow.h"

class UsersTab : public DialogWindow
{
public:
	virtual INT CALLBACK dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual void dialogInit(HWND hWnd,LPARAM lParam);
	virtual void dialogRefresh();

	void openGroupDialog(std::string *pGroupGuid);
	void openUserDialog(std::string *pUserGuid);

private:
	std::list<std::string> m_groupGuids;
	std::list<std::string> m_userGuids;

	HWND m_hListGroups;
	HWND m_hListUsers;
};

#endif
