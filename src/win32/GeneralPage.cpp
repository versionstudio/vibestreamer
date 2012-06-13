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
#include "generalpage.h"

INT CALLBACK GeneralPage::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
		{
			if ( HIWORD(wParam)==EN_CHANGE )
			{
				if ( IsWindowVisible(hWnd) ) {
					SendMessage(GetParent(hWnd),UWM_SETTINGS_CHANGED,NULL,NULL);
				}
			}
		}
		break;

		case WM_NOTIFY:
		{
			LPNMHDR pNmh = (LPNMHDR)lParam;
			switch ( pNmh->code )
			{
				case LVN_ITEMCHANGED:
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

void GeneralPage::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hListOptions = GetDlgItem(hWnd,IDC_LIST_OPTIONS);

	SendMessage(m_hListOptions,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_CHECKBOXES);
	SetWindowPos(hWnd,HWND_TOP,158,0,-1,-1,SWP_NOSIZE);
}

void GeneralPage::loadPage()
{
	int index = -1;

	ListView_DeleteAllItems(m_hListOptions);

	// option: auto-start server on startup
	index = WinUtil::ListViewUtil::insertItem(m_hListOptions,0,WinUtil::ResourceUtil::loadString(IDS_SETTINGS_AUTOSTART).c_str(),NULL);
	if ( SettingsManager::getInstance()->getBool(SettingsManager::AUTOSTART)==true ) {
		ListView_SetCheckState(m_hListOptions,index,true);
	}

	// option: automatically check for updates
	index = WinUtil::ListViewUtil::insertItem(m_hListOptions,1,WinUtil::ResourceUtil::loadString(IDS_SETTINGS_CHECKFORUPDATES).c_str(),NULL);
	if ( SettingsManager::getInstance()->getBool(SettingsManager::CHECKFORUPDATES)==true ) {
		ListView_SetCheckState(m_hListOptions,index,true);
	}

	// option: confirm on application exit
	index = WinUtil::ListViewUtil::insertItem(m_hListOptions,2,WinUtil::ResourceUtil::loadString(IDS_SETTINGS_CONFIRMEXIT).c_str(),NULL);
	if ( SettingsManager::getInstance()->getBool(SettingsManager::CONFIRMEXIT)==true ) {
		ListView_SetCheckState(m_hListOptions,index,true);
	}

	// option: minimize to tray
	index = WinUtil::ListViewUtil::insertItem(m_hListOptions,3,WinUtil::ResourceUtil::loadString(IDS_SETTINGS_MINIMIZETOTRAY).c_str(),NULL);
	if ( SettingsManager::getInstance()->getBool(SettingsManager::MINIMIZETOTRAY)==true ) {
		ListView_SetCheckState(m_hListOptions,index,true);
	}
}
	
void GeneralPage::savePage()
{
	// option: auto-start server on startup
	SettingsManager::getInstance()->setBool(SettingsManager::AUTOSTART,ListView_GetCheckState(m_hListOptions,0));

	// option: automatically check for updates
	SettingsManager::getInstance()->setBool(SettingsManager::CHECKFORUPDATES,ListView_GetCheckState(m_hListOptions,1));

	// option: confirm on application exit
	SettingsManager::getInstance()->setBool(SettingsManager::CONFIRMEXIT,ListView_GetCheckState(m_hListOptions,2));

	// option: minimize to tray
	SettingsManager::getInstance()->setBool(SettingsManager::MINIMIZETOTRAY,ListView_GetCheckState(m_hListOptions,3));
}
