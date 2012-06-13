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
#include "serverpage.h"

#include "hyperlinks.h"

#include "../server/configmanager.h"

INT CALLBACK ServerPage::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
		{
			if ( wParam==IDC_STATIC_WHATISTHIS ) {
				ShellExecute(hWnd,"open","http://www.vibestreamer.com/setup-server-address/",NULL,NULL,SW_SHOWNORMAL);
			}
			else
			{
				if ( HIWORD(wParam)==EN_CHANGE || HIWORD(wParam)==BN_CLICKED ||
					HIWORD(wParam)==CBN_SELCHANGE || HIWORD(wParam)==CBN_EDITCHANGE )
				{
					if ( IsWindowVisible(hWnd) ) {
						SendMessage(GetParent(hWnd),UWM_SETTINGS_CHANGED,NULL,NULL);
					}
				}
			}
		}
		break;
	}

	return 0;
}

void ServerPage::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hCheckAutoConfigureRouter = GetDlgItem(hWnd,IDC_CHECK_AUTOCONFIGUREROUTER);
	m_hComboIpAddress = GetDlgItem(hWnd,IDC_COMBO_IPADDRESS);
	m_hEditAddress = GetDlgItem(hWnd,IDC_EDIT_ADDRESS);
	m_hEditMaxSessions = GetDlgItem(hWnd,IDC_EDIT_MAXSESSIONS);
	m_hEditPort = GetDlgItem(hWnd,IDC_EDIT_PORT);
	m_hEditSessionTimeout = GetDlgItem(hWnd,IDC_EDIT_SESSIONTIMEOUT);

	ConvertStaticToHyperlink(GetDlgItem(hWnd,IDC_STATIC_WHATISTHIS));

	SetWindowPos(hWnd,HWND_TOP,158,0,-1,-1,SWP_NOSIZE);
}

void ServerPage::loadPage()
{
	SetWindowText(m_hEditAddress,
		ConfigManager::getInstance()->getString(ConfigManager::HTTPSERVER_CONNECTOR_SERVERADDRESS).c_str());

	SetWindowText(m_hEditPort,
		ConfigManager::getInstance()->getString(ConfigManager::HTTPSERVER_CONNECTOR_SERVERPORT).c_str());

    OSVERSIONINFO osvi;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

	// auto configure router - enabled only for windows xp and higher
	if ( osvi.dwMajorVersion>5 || (osvi.dwMajorVersion==5 && osvi.dwMinorVersion>0) ) {
		Button_SetCheck(m_hCheckAutoConfigureRouter,
			SettingsManager::getInstance()->getBool(SettingsManager::UPNPENABLED));
	}
	else {
		EnableWindow(m_hCheckAutoConfigureRouter,false);
	}

	SetWindowText(m_hEditMaxSessions,
		ConfigManager::getInstance()->getString(ConfigManager::HTTPSERVER_SESSIONMANAGER_MAXSESSIONS).c_str());

	int sessionTimeoutSeconds = ConfigManager::getInstance()->getInt(ConfigManager::HTTPSERVER_SESSIONMANAGER_SESSIONTIMEOUT)/1000;
	SetWindowText(m_hEditSessionTimeout,
		Util::ConvertUtil::toString(sessionTimeoutSeconds).c_str());
}

void ServerPage::savePage()
{
	char sz[255];

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditAddress,sz,sizeof(sz));
	ConfigManager::getInstance()->setString(ConfigManager::HTTPSERVER_CONNECTOR_SERVERADDRESS,std::string(sz));

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditPort,sz,sizeof(sz));
	ConfigManager::getInstance()->setString(ConfigManager::HTTPSERVER_CONNECTOR_SERVERPORT,std::string(sz));

	SettingsManager::getInstance()->setBool(SettingsManager::UPNPENABLED,Button_GetCheck(m_hCheckAutoConfigureRouter));

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditMaxSessions,sz,sizeof(sz));
	ConfigManager::getInstance()->setString(ConfigManager::HTTPSERVER_SESSIONMANAGER_MAXSESSIONS,std::string(sz));

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditSessionTimeout,sz,sizeof(sz));
	ConfigManager::getInstance()->setInt(ConfigManager::HTTPSERVER_SESSIONMANAGER_SESSIONTIMEOUT,
		(Util::ConvertUtil::toInt(sz)*1000));
}
