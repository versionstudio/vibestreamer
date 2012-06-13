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
#include "indexingpage.h"

#include "../server/configmanager.h"

INT CALLBACK IndexingPage::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
		{
			if ( HIWORD(wParam)==EN_CHANGE || HIWORD(wParam)==BN_CLICKED )
			{
				if ( IsWindowVisible(hWnd) ) {
					SendMessage(GetParent(hWnd),UWM_SETTINGS_CHANGED,NULL,NULL);
				}
			}
		}
		break;
	}

	return 0;
}

void IndexingPage::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hEditFilePattern = GetDlgItem(hWnd,IDC_EDIT_FILEPATTERN);
	m_hCheckIncludeHidden = GetDlgItem(hWnd,IDC_CHECK_INCLUDEHIDDEN);

	SetWindowPos(hWnd,HWND_TOP,158,0,-1,-1,SWP_NOSIZE);
}

void IndexingPage::loadPage()
{
	SetWindowText(m_hEditFilePattern,
		ConfigManager::getInstance()->getString(ConfigManager::INDEXER_FILEPATTERN).c_str());

	Button_SetCheck(m_hCheckIncludeHidden,
		ConfigManager::getInstance()->getBool(ConfigManager::INDEXER_INCLUDEHIDDEN));
}

void IndexingPage::savePage()
{
	char sz[255];

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditFilePattern,sz,sizeof(sz));
	ConfigManager::getInstance()->setString(ConfigManager::INDEXER_INCLUDEHIDDEN,std::string(sz));

	ConfigManager::getInstance()->setBool(ConfigManager::INDEXER_INCLUDEHIDDEN,
		Button_GetCheck(m_hCheckIncludeHidden));
}
