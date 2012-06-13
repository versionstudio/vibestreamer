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

#ifndef guard_usersetuppage_h
#define guard_usersetuppage_h

#include "../server/user.h"
#include "../server/group.h"

#include "dialogwindow.h"

class UserSetupPage : public DialogWindow
{
public:
	virtual INT CALLBACK dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual void dialogInit(HWND hWnd,LPARAM lParam);

	void loadPage(User *pUser);
	void savePage(User *pUser);

private:
	static const std::string ENCRYPTED_PASSWORD;

	HWND m_hCheckDisabled;
	HWND m_hEditName;
	HWND m_hEditPassword;
	HWND m_hListGroups;

	std::list<Group> m_groups;
	
	std::string m_newPassword;
};

#endif
