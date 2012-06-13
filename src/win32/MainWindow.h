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

#ifndef guard_mainwindow_h
#define guard_mainwindow_h

#include "../server/httpserver.h"

#include "indexingdialog.h"
#include "securitytab.h"
#include "settingstab.h"
#include "sharingtab.h"
#include "statustab.h"
#include "updatedialog.h"
#include "userstab.h"

class MainWindow : public HttpServerListener,
						  HttpSessionManagerListener
{
public:
	MainWindow() : m_lastBandwidth(0),
				   m_pIndexingDialog(NULL),
				   m_pSecurityTab(NULL),
				   m_pSettingsTab(NULL),
				   m_pSharingTab(NULL),
				   m_pStatusTab(NULL),
				   m_pUpdateDialog(NULL),
				   m_pUsersTab(NULL)
	{
		
	}
	
	LRESULT CALLBACK wndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

	virtual void on(HttpServerListener::Start);

	virtual void on(HttpServerListener::Stop);

	virtual void on(HttpSessionManagerListener::SessionAdded,HttpSession::Ptr sessionPtr);

	virtual void on(HttpSessionManagerListener::SessionRemoved,
		HttpSession::Ptr sessionPtr,HttpSessionManagerListener::SessionRemovedEventArgs e);

private:
	void createDialogs(HWND hWnd);
	void createMenu(HWND hWnd);
	void createStatusBar(HWND hWnd);
	void createTabControl(HWND hWnd);
	void createTopBitmap(HWND hWnd);
	void createTrayIcon(HWND hWnd);
	void removeTrayIcon(HWND hWnd);

	void updateBandwidth();

	HMENU m_hMainMenu;

	HWND m_hWnd;
	HWND m_hStatusBar;
	HWND m_hTabControl;	
	HWND m_hTopBitmap;

	SecurityTab *m_pSecurityTab;
	SettingsTab *m_pSettingsTab;
	SharingTab *m_pSharingTab;
	StatusTab *m_pStatusTab;
	UsersTab *m_pUsersTab;

	IndexingDialog *m_pIndexingDialog;

	UpdateDialog *m_pUpdateDialog;

	uint64_t m_lastBandwidth;
};

#endif
