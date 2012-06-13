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
#include "userdialog.h"

#include "../server/usermanager.h"

INT CALLBACK UserDialog::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
		{
			switch ( wParam )
			{
				case IDCANCEL:
				{
					EndDialog(hWnd,0);
				}
				break;

				case IDOK:
				{
					std::string name = WinUtil::WindowUtil::getWindowText(GetDlgItem(m_setupPage.getHwnd(),IDC_EDIT_NAME));
					std::string password = WinUtil::WindowUtil::getWindowText(GetDlgItem(m_setupPage.getHwnd(),IDC_EDIT_PASSWORD));

					if ( name.empty() ) {
						MessageBox(hWnd,WinUtil::ResourceUtil::loadString(IDS_INVALIDUSERNAME).c_str(),APP_NAME,MB_ICONEXCLAMATION);
						break;
					}

					if ( password.empty() ) {
						MessageBox(hWnd,WinUtil::ResourceUtil::loadString(IDS_INVALIDPASSWORD).c_str(),APP_NAME,MB_ICONEXCLAMATION);
						break;
					}

					// make sure a user with this name doesn't already exist
					if ( !boost::iequals(m_pUser->getName(),name)
						&& UserManager::getInstance()->findUserByName(name,NULL) ) {
						MessageBox(hWnd,WinUtil::ResourceUtil::loadString(IDS_USEREXISTS).c_str(),APP_NAME,MB_ICONEXCLAMATION);
						break;
					}

					m_setupPage.savePage(m_pUser);
					m_generalPage.savePage(m_pUser);
					m_statisticsPage.savePage(m_pUser);

					EndDialog(hWnd,1);
				}
				break;
			}
		}
		break;

		case WM_NOTIFY: 
		{
			LPNMHDR pNmhdr = (LPNMHDR)lParam;
			if ( pNmhdr->hwndFrom==m_hTreeView && pNmhdr->code==TVN_SELCHANGING )
			{
				HTREEITEM selectedItem = TreeView_GetSelection(m_hTreeView);
				DialogWindow *pSelectedDialog = (DialogWindow*)WinUtil::TreeViewUtil::getItemParam(m_hTreeView,selectedItem);
				if ( pSelectedDialog!=NULL ) {
					ShowWindow(pSelectedDialog->getHwnd(),SW_HIDE);
				}
			}
			else if ( pNmhdr->hwndFrom==m_hTreeView && pNmhdr->code==TVN_SELCHANGED )
			{
				HTREEITEM selectedItem = TreeView_GetSelection(m_hTreeView);
				DialogWindow *pSelectedDialog = (DialogWindow*)WinUtil::TreeViewUtil::getItemParam(m_hTreeView,selectedItem);
				if ( pSelectedDialog!=NULL ) {
					ShowWindow(pSelectedDialog->getHwnd(),SW_SHOW);
				}
			}
		}
		break;

	}

	return 0;
}

void UserDialog::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hTreeView = GetDlgItem(hWnd,IDC_TREE_SETTINGS);

	WinUtil::TreeViewUtil::insertItem(m_hTreeView,NULL,"Setup",(LPARAM)&m_setupPage);
	WinUtil::TreeViewUtil::insertItem(m_hTreeView,NULL,"General",(LPARAM)&m_generalPage);
	WinUtil::TreeViewUtil::insertItem(m_hTreeView,NULL,"Statistics",(LPARAM)&m_statisticsPage);

	m_setupPage.init(hWnd,MAKEINTRESOURCE(IDD_PAGE_USERSETUP),NULL);
	m_generalPage.init(hWnd,MAKEINTRESOURCE(IDD_PAGE_USERGENERAL),NULL);
	m_statisticsPage.init(hWnd,MAKEINTRESOURCE(IDD_PAGE_USERSTATISTICS),NULL);
	
	WinUtil::WindowUtil::centerParent(hWnd);

	m_pUser = (User*)lParam;

	if ( m_pUser->getGuid().empty() ) {
		SetWindowText(hWnd,WinUtil::ResourceUtil::loadString(IDS_ADDUSER).c_str());
	}
	else {
		SetWindowText(hWnd,WinUtil::ResourceUtil::loadString(IDS_EDITUSER).c_str());
	}

	TreeView_SelectItem(m_hTreeView,TreeView_GetRoot(m_hTreeView));
	SetFocus(m_hTreeView);

	m_setupPage.loadPage(m_pUser);
	m_generalPage.loadPage(m_pUser);
	m_statisticsPage.loadPage(m_pUser);
}
