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

#include "common.h"
#include "mainwindow.h"

#define LOGGER_CLASSNAME "MainWindow"

#include "../server/core.h"
#include "../server/configmanager.h"
#include "../server/sharehandler.h"

#include "aboutdialog.h"
#include "cleanupdialog.h"
#include "startupdialog.h"
#include "upnpmanager.h"

LRESULT CALLBACK MainWindow::wndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case UWM_SESSIONS_UPDATED:
		{
			int connectedSessions = HttpServer::getInstance()->getSessionManager().getSessionCount();
			SendMessage(m_hStatusBar,SB_SETTEXT,1,(LPARAM)std::string("\t" 
				+ Util::StringUtil::format(WinUtil::ResourceUtil::loadString(IDS_FORMAT_CONNECTEDSESSIONS).c_str(),connectedSessions)).c_str());
		}
		break;

		case WM_CREATE:
		{
			Core::init();

			HttpServer::getInstance()->addListener(this);
			HttpServer::getInstance()->getSessionManager().addListener(this);

			SettingsManager::newInstance();
			UpnpManager::newInstance();

			SettingsManager::getInstance()->load();

			StartupDialog startupDialog;
			if ( startupDialog.init(hWnd,MAKEINTRESOURCE(IDD_STARTUP),NULL)!=0 ) {
				MessageBox(hWnd,"An error occurred while initializing application.\r\nCheck your server log for details.",APP_NAME,MB_ICONERROR);
				PostQuitMessage(0);
				return 0;
			}

			createMenu(hWnd);
			createStatusBar(hWnd);
			createTabControl(hWnd);
			createDialogs(hWnd);
			createTopBitmap(hWnd);
			createTrayIcon(hWnd);

			SetTimer(hWnd,IDT_MAIN,1000,NULL);

			ShowWindow(hWnd,SW_SHOW);
			UpdateWindow(hWnd);

			LogManager::getInstance()->info(LOGGER_CLASSNAME,"%s %s%s",APP_NAME,APP_VERSION,APP_MILESTONE);
			if ( !std::string(APP_MILESTONE).empty() ) {
				LogManager::getInstance()->info(LOGGER_CLASSNAME,"Please note - This is a milestone release and not the final version");
			}

			if ( SettingsManager::getInstance()->getBool(SettingsManager::AUTOSTART)==true ) {
				m_pStatusTab->startServer();
			}

			if ( SettingsManager::getInstance()->getBool(SettingsManager::CHECKFORUPDATES)==true ) {
				m_pUpdateDialog->checkForUpdates();
			}
		}
		break;

		case WM_CLOSE:
		{
			if ( SettingsManager::getInstance()->getBool(SettingsManager::CONFIRMEXIT)==false 
				|| MessageBox(hWnd,WinUtil::ResourceUtil::loadString(IDS_REALLYEXIT).c_str(),TEXT(APP_NAME),MB_ICONQUESTION | MB_YESNO)==IDYES ) {
				DestroyWindow(hWnd);
			}
		}
		break;

		case WM_TIMER:
		{
			if ( wParam==IDT_MAIN ) {
				updateBandwidth();
			}
		}
		break;

		case WM_COMMAND:
		{
			switch ( wParam )
			{
				case ID_FILE_EXIT:
				{
					PostMessage(hWnd,WM_CLOSE,NULL,NULL);
				}
				break;

				case ID_FILE_RESTOREDEFAULTSETTINGS:
				{
					if ( MessageBox(hWnd,WinUtil::ResourceUtil::loadString(IDS_RESTOREDEFAULTSETTINGS_CONFIRM).c_str(),
						WinUtil::ResourceUtil::loadString(IDS_RESTOREDEFAULTSETTINGS).c_str(),MB_YESNO)==IDYES )
					{
						ConfigManager::getInstance()->restoreDefaults();
						ConfigManager::getInstance()->save();

						SettingsManager::getInstance()->restoreDefaults();
						SettingsManager::getInstance()->save();

						m_pSettingsTab->dialogRefresh();
					}
				}
				break;

				case ID_VIEW_INDEXINGPROGRESS:
				{
					if ( !IsWindowVisible(m_pIndexingDialog->getHwnd()) ) {
						ShowWindow(m_pIndexingDialog->getHwnd(),SW_SHOW);
					}
					else {
						ShowWindow(m_pIndexingDialog->getHwnd(),SW_HIDE);
					}
				}
				break;

				case ID_VIEW_REFRESH:
				{
					int selectedTabIndex = TabCtrl_GetCurSel(m_hTabControl);
					DialogWindow *pTabDialog = (DialogWindow*)WinUtil::TabControlUtil::getItemParam(m_hTabControl,selectedTabIndex);
					if ( pTabDialog!=NULL ) {
						pTabDialog->dialogRefresh();
					}
				}
				break;

				case ID_WINDOW_MINIMIZETOTRAY:
				{
					ShowWindow(hWnd,SW_HIDE);
				}
				break;

				case ID_HELP_ABOUT:
				{
					AboutDialog aboutDialog;
					aboutDialog.init(hWnd, MAKEINTRESOURCE(IDD_ABOUT), NULL);
				}
				break;

				case ID_HELP_CHECKFORUPDATES:
				{
					if ( !IsWindowVisible(m_pUpdateDialog->getHwnd()) ) {
						ShowWindow(m_pUpdateDialog->getHwnd(),SW_SHOW);
					}

					m_pUpdateDialog->checkForUpdates();
				}
				break;

				case ID_HELP_DOCUMENTATION:
				{
					ShellExecute(hWnd,"open","http://www.vibestreamer.com/documentation/",NULL,NULL,SW_SHOWNORMAL);
				}
				break;

				case ID_HELP_WEBSITE:
				{
					ShellExecute(hWnd,"open","http://www.vibestreamer.com/",NULL,NULL,SW_SHOWNORMAL);
				}
				break;

				case UID_TRAY_EXIT:
				{
					PostMessage(hWnd,WM_CLOSE,NULL,NULL);
				}
				break;

				case UID_TRAY_MINIMIZETOTRAY:
				{
					ShowWindow(hWnd,SW_HIDE);
				}
				break;

				case UID_TRAY_SHOWWINDOW:
				{
					ShowWindow(hWnd,SW_SHOW);
				}
				break;
			}
		}
		break;

		case WM_DESTROY:
		{
			KillTimer(hWnd,IDT_MAIN);

			HttpServer::getInstance()->removeListener(this);
			HttpServer::getInstance()->getSessionManager().removeListener(this);

			if ( m_pSettingsTab!=NULL ) {
				delete m_pSettingsTab;
			}
			if ( m_pSharingTab!=NULL ) {
				delete m_pSharingTab;
			}
			if ( m_pUsersTab!=NULL ) {
				delete m_pUsersTab;
			}
			if ( m_pStatusTab!=NULL ) {
				delete m_pStatusTab;
			}

			CleanupDialog cleanupDialog;			
			cleanupDialog.init(hWnd,MAKEINTRESOURCE(IDD_CLEANUP),NULL);

			removeTrayIcon(hWnd);

			SettingsManager::getInstance()->save();
			SettingsManager::deleteInstance();
			UpnpManager::deleteInstance();

			PostQuitMessage(0);
		}
		break;

		case WM_NOTIFY: 
		{
			LPNMHDR pNmhdr = (LPNMHDR)lParam;
			if ( pNmhdr->hwndFrom==m_hTabControl && pNmhdr->code==TCN_SELCHANGING ) 
			{
				int selectedIndex = TabCtrl_GetCurSel(m_hTabControl);
				DialogWindow *pTabDialog = (DialogWindow*)WinUtil::TabControlUtil::getItemParam(m_hTabControl,selectedIndex);
				if ( pTabDialog!=NULL ) {
					ShowWindow(pTabDialog->getHwnd(),SW_HIDE);
				}
			}
			else if ( pNmhdr->hwndFrom==m_hTabControl && pNmhdr->code==TCN_SELCHANGE ) 
			{
				int selectedIndex = TabCtrl_GetCurSel(m_hTabControl);
				DialogWindow *pTabDialog = (DialogWindow*)WinUtil::TabControlUtil::getItemParam(m_hTabControl,selectedIndex);
				if ( pTabDialog!=NULL ) {
					ShowWindow(pTabDialog->getHwnd(),SW_SHOW);
				}
			}
		}
		break;

		case WM_SIZE:
		{
			if ( wParam==SIZE_MINIMIZED && SettingsManager::getInstance()->getBool(SettingsManager::MINIMIZETOTRAY)==true ) {
				ShowWindow(hWnd,SW_HIDE);
			}
		}
		break;

		case UWN_TRAY_CALLBACK:
		{
			if (wParam!=UID_TRAY_ICON ) {
				break;	
			}

			switch (lParam )
			{
				case WM_LBUTTONDBLCLK:
				{
					if ( IsWindowVisible(hWnd) ) {
						ShowWindow(hWnd,FALSE);
					}
					else {
						ShowWindow(hWnd,TRUE);
						SetForegroundWindow(hWnd);
					}
				}
				break;

				case WM_RBUTTONDOWN:
				{
					HMENU trayMenu;

					trayMenu = CreatePopupMenu();
					if ( trayMenu!=NULL )
					{						
						AppendMenu(trayMenu,MF_STRING,UID_TRAY_SHOWWINDOW,WinUtil::ResourceUtil::loadString(IDS_SHOWWINDOW).c_str());
						AppendMenu(trayMenu,MF_STRING,UID_TRAY_MINIMIZETOTRAY,WinUtil::ResourceUtil::loadString(IDS_MINIMIZETOTRAY).c_str());
						AppendMenu(trayMenu,MF_SEPARATOR,NULL,NULL);
						AppendMenu(trayMenu,MF_STRING,UID_TRAY_EXIT,WinUtil::ResourceUtil::loadString(IDS_EXIT).c_str());

						POINT ptMouse;
						GetCursorPos(&ptMouse);

						SetForegroundWindow(hWnd);
						TrackPopupMenu(trayMenu,NULL,ptMouse.x,ptMouse.y,0,hWnd,NULL);
						SendMessage(hWnd,WM_NULL,0,0); // send beneign message to make sure window goes away
					}
				}
				break;
			}
		}
		break;

		case WM_QUERYENDSESSION:
		{
			// called when the computer is shutdown/rebooted
			// without exiting the application
			DestroyWindow(hWnd);

			return TRUE;
		}
		break;

		default:
		{
			return DefWindowProc(hWnd,msg,wParam,lParam);
		}        
	}

	return 0;
}

void MainWindow::createDialogs(HWND hWnd)
{
	m_pIndexingDialog = new IndexingDialog();
	m_pIndexingDialog->init(hWnd,MAKEINTRESOURCE(IDD_INDEXING),NULL);

	m_pUpdateDialog = new UpdateDialog();
	m_pUpdateDialog->init(hWnd,MAKEINTRESOURCE(IDD_UPDATE),NULL);
}

void MainWindow::createMenu(HWND hWnd)
{
	m_hMainMenu = LoadMenu(GetModuleHandle(NULL),MAKEINTRESOURCE(IDR_MAINMENU));
	SetMenu(hWnd,m_hMainMenu);
}

void MainWindow::createStatusBar(HWND hWnd)
{
	m_hStatusBar = CreateWindowEx(0,STATUSCLASSNAME,"",WS_CHILD | WS_VISIBLE,
		0,0,0,0,hWnd,NULL,GetModuleHandle(NULL),NULL);

	int partWidths[3];
	partWidths[0] = WinUtil::DpiUtil::scaleX(194);
	partWidths[1] = WinUtil::DpiUtil::scaleX(374);
	partWidths[2] = -1;

	SendMessage(m_hStatusBar,SB_SETPARTS,3,(LPARAM)partWidths);
	SendMessage(m_hStatusBar,SB_SETTEXT,0,(LPARAM)std::string("\t"
		+ WinUtil::ResourceUtil::loadString(IDS_SERVERNOTRUNNING)).c_str());

	SendMessage(m_hStatusBar,SB_SETTEXT,1,(LPARAM)std::string("\t"
		+ Util::StringUtil::format(WinUtil::ResourceUtil::loadString(IDS_FORMAT_CONNECTEDSESSIONS).c_str(),0)).c_str());

	SendMessage(m_hStatusBar,SB_SETTEXT,2,(LPARAM)std::string("\t" 
		+ Util::StringUtil::format(WinUtil::ResourceUtil::loadString(IDS_FORMAT_STREAMING).c_str(),m_lastBandwidth)).c_str());
}

void MainWindow::createTabControl(HWND hWnd)
{
	m_hTabControl = CreateWindow(WC_TABCONTROL,"",WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
		WinUtil::DpiUtil::scaleX(10),WinUtil::DpiUtil::scaleY(57),
		WinUtil::DpiUtil::scaleX(540),WinUtil::DpiUtil::scaleY(338),hWnd,NULL,GetModuleHandle(NULL),NULL);

	SendMessage(m_hTabControl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),MAKELPARAM(TRUE,0));

	m_pSecurityTab = new SecurityTab();
	m_pSettingsTab = new SettingsTab();
	m_pSharingTab = new SharingTab();
	m_pStatusTab = new StatusTab(m_hStatusBar);
	m_pUsersTab = new UsersTab();

	m_pSecurityTab->init(m_hTabControl,MAKEINTRESOURCE(IDD_TAB_SECURITY),NULL);
	m_pSettingsTab->init(m_hTabControl,MAKEINTRESOURCE(IDD_TAB_SETTINGS),NULL);
	m_pSharingTab->init(m_hTabControl,MAKEINTRESOURCE(IDD_TAB_SHARING),NULL);
	m_pStatusTab->init(m_hTabControl,MAKEINTRESOURCE(IDD_TAB_STATUS),NULL);
	m_pUsersTab->init(m_hTabControl,MAKEINTRESOURCE(IDD_TAB_USERS),NULL);	

	WinUtil::TabControlUtil::insertItem(m_hTabControl,0,WinUtil::ResourceUtil::loadString(IDS_STATUS).c_str(),(LPARAM)m_pStatusTab);
	WinUtil::TabControlUtil::insertItem(m_hTabControl,1,WinUtil::ResourceUtil::loadString(IDS_USERSGROUPS).c_str(),(LPARAM)m_pUsersTab);
	WinUtil::TabControlUtil::insertItem(m_hTabControl,2,WinUtil::ResourceUtil::loadString(IDS_SHARING).c_str(),(LPARAM)m_pSharingTab);
	WinUtil::TabControlUtil::insertItem(m_hTabControl,3,WinUtil::ResourceUtil::loadString(IDS_SETTINGS).c_str(),(LPARAM)m_pSettingsTab);
	WinUtil::TabControlUtil::insertItem(m_hTabControl,4,WinUtil::ResourceUtil::loadString(IDS_SECURITY).c_str(),(LPARAM)m_pSecurityTab);

	TabCtrl_SetCurSel(m_hTabControl,0);

	NMHDR nmhdr;
	nmhdr.code = TCN_SELCHANGE;
	nmhdr.idFrom = (UINT_PTR)m_hTabControl;
	nmhdr.hwndFrom = m_hTabControl;

	SendMessage(hWnd,WM_NOTIFY,TCN_SELCHANGE,(LPARAM)&nmhdr);
}

void MainWindow::createTopBitmap(HWND hWnd)
{
	HANDLE bitmap = LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_TOP),NULL,0,0,NULL);
	if ( bitmap==NULL ) {
		return;
	}

	m_hTopBitmap = CreateWindow("STATIC","logo",WS_VISIBLE|WS_CHILD|SS_BITMAP,0,-1,0,0,
		hWnd,NULL,GetModuleHandle(NULL),NULL);

	if ( m_hTopBitmap!=NULL ) {
		SendMessage(m_hTopBitmap,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)bitmap);
	}
}

void MainWindow::createTrayIcon(HWND hWnd)
{
	NOTIFYICONDATA nid = {0};
	nid.cbSize = sizeof(nid);
	nid.hWnd = hWnd;
	nid.uID = UID_TRAY_ICON;
	nid.uFlags = NIF_ICON | NIF_MESSAGE;
	nid.uCallbackMessage = UWN_TRAY_CALLBACK;
	nid.hIcon = (HICON)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_VIBE),IMAGE_ICON,16,16,0);

	Shell_NotifyIcon(NIM_ADD,&nid);
}

void MainWindow::removeTrayIcon(HWND hWnd)
{
	NOTIFYICONDATA nid = {0};
	nid.cbSize = sizeof(nid);
	nid.hWnd = hWnd;
	nid.uID = UID_TRAY_ICON;

	Shell_NotifyIcon(NIM_DELETE,&nid);
}

void MainWindow::updateBandwidth()
{
	uint64_t bandwidth = 0;

	if ( HttpServer::getInstance()->getSessionManager().getSessionCount()>0 )
	{
		std::vector<HttpSession::Ptr> sessions = HttpServer::getInstance()->getSessionManager().getSessions();
		for ( std::vector<HttpSession::Ptr>::iterator iter=sessions.begin();
			iter!=sessions.end(); iter++ )
		{
			std::string sessionBandwidth;
			if ( (*iter)->getAttribute(ShareHandler::ATTRIBUTE_BANDWIDTH,&sessionBandwidth) ) {
				bandwidth += Util::ConvertUtil::toInt(sessionBandwidth);
			}
		}
	}

	if ( bandwidth!=m_lastBandwidth )
	{
		SendMessage(m_hStatusBar,SB_SETTEXT,2,(LPARAM)std::string("\t"
			+ Util::StringUtil::format(WinUtil::ResourceUtil::loadString(IDS_FORMAT_STREAMING).c_str(),bandwidth)).c_str());

		m_lastBandwidth = bandwidth;
	}
}

void MainWindow::on(HttpServerListener::Start)
{
	SendMessage(m_hStatusBar,SB_SETTEXT,0,(LPARAM)std::string("\t" + WinUtil::ResourceUtil::loadString(IDS_SERVERRUNNING)).c_str());	
}

void MainWindow::on(HttpServerListener::Stop)
{
	SendMessage(m_hStatusBar,SB_SETTEXT,0,(LPARAM)std::string("\t" + WinUtil::ResourceUtil::loadString(IDS_SERVERNOTRUNNING)).c_str());	
}

void MainWindow::on(HttpSessionManagerListener::SessionAdded,HttpSession::Ptr sessionPtr)
{
	PostMessage(GetParent(m_hStatusBar),UWM_SESSIONS_UPDATED,NULL,NULL);
}

void MainWindow::on(HttpSessionManagerListener::SessionRemoved,HttpSession::Ptr sessionPtr,
	HttpSessionManagerListener::SessionRemovedEventArgs e)
{
	PostMessage(GetParent(m_hStatusBar),UWM_SESSIONS_UPDATED,NULL,NULL);
}
