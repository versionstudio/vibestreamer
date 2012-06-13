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
#include "sharesetuppage.h"

INT CALLBACK ShareSetupPage::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
		{
			if ( HIWORD(wParam)==BN_CLICKED ) {
				updateDialog();
			}

			switch ( wParam )
			{
				case IDC_BUTTON_BROWSE:
				{
					char path[MAX_PATH] = {0};
					char virtualName[MAX_PATH] = {0};

					BROWSEINFO bi = {0};
					bi.hwndOwner = hWnd;
					bi.pszDisplayName = virtualName;

					LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
					if ( pidl!=NULL ) {
						SHGetPathFromIDList(pidl,path);
						SetWindowText(m_hEditPath,path);
						if ( GetWindowTextLength(m_hEditVirtualName)==0 ) {
							SetWindowText(m_hEditVirtualName,virtualName);
						}
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

void ShareSetupPage::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hButtonBrowse = GetDlgItem(hWnd,IDC_BUTTON_BROWSE);
	m_hCheckAutoIndex = GetDlgItem(hWnd,IDC_CHECK_AUTOINDEX);
	m_hEditAutoIndexInterval = GetDlgItem(hWnd,IDC_EDIT_AUTOINDEXINTERVAL);
	m_hEditPath = GetDlgItem(hWnd,IDC_EDIT_PATH);
	m_hEditVirtualName = GetDlgItem(hWnd,IDC_EDIT_VIRTUALNAME);

	SetWindowPos(m_hEditPath,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hButtonBrowse,m_hEditPath,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hEditVirtualName,m_hButtonBrowse,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hCheckAutoIndex,m_hEditVirtualName,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hEditAutoIndexInterval,m_hCheckAutoIndex,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(hWnd,HWND_TOP,166,5,-1,-1,SWP_NOSIZE);
}

void ShareSetupPage::updateDialog()
{
	EnableWindow(m_hEditAutoIndexInterval,Button_GetCheck(m_hCheckAutoIndex));
}

void ShareSetupPage::loadPage(Share *pShare)
{
	SetWindowText(m_hEditPath,pShare->getPath().c_str());
	SetWindowText(m_hEditVirtualName,pShare->getName().c_str());
	Button_SetCheck(m_hCheckAutoIndex,pShare->isAutoIndex());

	SetWindowText(m_hEditAutoIndexInterval,
		Util::ConvertUtil::toString(pShare->getAutoIndexInterval()).c_str());

	updateDialog();
}

void ShareSetupPage::savePage(Share *pShare)
{
	char sz[255];

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditPath,sz,sizeof(sz));
	pShare->setPath(sz);

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditVirtualName,sz,sizeof(sz));
	pShare->setName(sz);

	pShare->setAutoIndex(Button_GetCheck(m_hCheckAutoIndex));

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditAutoIndexInterval,sz,sizeof(sz));
	pShare->setAutoIndexInterval(Util::ConvertUtil::toInt(sz));
}
