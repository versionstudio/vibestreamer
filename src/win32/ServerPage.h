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

#ifndef guard_serverpage_h
#define guard_serverpage_h

#include "dialogwindow.h"

class ServerPage : public DialogWindow
{
public:
	virtual INT CALLBACK dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual void dialogInit(HWND hWnd,LPARAM lParam);

	void loadPage();
	void savePage();

private:
	HWND m_hCheckAutoConfigureRouter;
	HWND m_hComboIpAddress;
	HWND m_hEditAddress;
	HWND m_hEditMaxSessions;
	HWND m_hEditPort;
	HWND m_hEditSessionTimeout;
};

#endif
