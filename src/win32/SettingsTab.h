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

#ifndef guard_settingstab_h
#define guard_settingstab_h

#include "dialogwindow.h"
#include "generalpage.h"
#include "indexingpage.h"
#include "loggingpage.h"
#include "serverpage.h"
#include "sslpage.h"

class SettingsTab : public DialogWindow
{
public:
	virtual INT CALLBACK dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual void dialogInit(HWND hWnd,LPARAM lParam);
	virtual void dialogRefresh();

private:
	HWND m_hButtonSaveChanges;
	HWND m_hTreeView;

	GeneralPage m_generalPage;
	IndexingPage m_indexingPage;
	LoggingPage m_loggingPage;
	ServerPage m_serverPage;
	SslPage m_sslPage;
};

#endif
