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
#include "loggingpage.h"

#include "../server/configmanager.h"

INT CALLBACK LoggingPage::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
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

			switch ( wParam )
			{
				case IDC_BUTTON_BROWSE:
				{
					char path[MAX_PATH] = {0};

					BROWSEINFO bi = {0};
					bi.hwndOwner = hWnd;
					
					LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
					if ( pidl!=NULL ) {
						SHGetPathFromIDList(pidl,path);
						SetWindowText(m_hEditPath,path);
					}

					IMalloc *pMalloc = NULL;
					if ( SUCCEEDED(SHGetMalloc(&pMalloc)) ) {
						pMalloc->Free(pidl);
						pMalloc->Release();
					}
				}
				break;
			}
		}
		break;
	}

	return 0;
}

void LoggingPage::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hEditPath = GetDlgItem(hWnd,IDC_EDIT_PATH);
	SetWindowPos(hWnd,HWND_TOP,158,0,-1,-1,SWP_NOSIZE);
}

void LoggingPage::loadPage()
{
	SetWindowText(m_hEditPath,
		ConfigManager::getInstance()->getString(ConfigManager::LOGMANAGER_PATH).c_str());
}
	
void LoggingPage::savePage()
{
	char sz[255];

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditPath,sz,sizeof(sz));
	ConfigManager::getInstance()->setString(ConfigManager::LOGMANAGER_PATH,std::string(sz));
}
