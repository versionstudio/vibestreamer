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

#ifndef guard_sharesetuppage_h
#define guard_sharesetuppage_h

#include "../server/share.h"

#include "dialogwindow.h"

class ShareSetupPage : public DialogWindow
{
public:
	virtual INT CALLBACK dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual void dialogInit(HWND hWnd,LPARAM lParam);

	void loadPage(Share *pShare);
	void savePage(Share *pShare);

private:
	void updateDialog();

	HWND m_hButtonBrowse;
	HWND m_hCheckAutoIndex;
	HWND m_hEditAutoIndexInterval;
	HWND m_hEditPath;
	HWND m_hEditVirtualName;
};

#endif
