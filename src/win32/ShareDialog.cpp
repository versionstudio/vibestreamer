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
#include "sharedialog.h"

#include "../server/indexer.h"
#include "../server/sharemanager.h"

INT CALLBACK ShareDialog::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
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
					std::string name = WinUtil::WindowUtil::getWindowText(GetDlgItem(m_setupPage.getHwnd(),IDC_EDIT_VIRTUALNAME));
					std::string path = WinUtil::WindowUtil::getWindowText(GetDlgItem(m_setupPage.getHwnd(),IDC_EDIT_PATH));
					std::string oldPath = m_pShare->getPath();

					if ( name.empty() ) {
						MessageBox(hWnd,WinUtil::ResourceUtil::loadString(IDS_INVALIDVIRTUALNAME).c_str(),APP_NAME,MB_ICONEXCLAMATION);
						break;
					}

					if ( path.empty() ) {
						MessageBox(hWnd,WinUtil::ResourceUtil::loadString(IDS_INVALIDPATH).c_str(),APP_NAME,MB_ICONEXCLAMATION);
						break;
					}

					// make sure a share with this name doesn't already exist
					if ( !boost::iequals(m_pShare->getName(),name)
						&& ShareManager::getInstance()->findShareByName(name,NULL) ) {
						MessageBox(hWnd,WinUtil::ResourceUtil::loadString(IDS_SHAREEXISTS).c_str(),APP_NAME,MB_ICONEXCLAMATION);
						break;
					}

					m_setupPage.savePage(m_pShare);
					m_permissionsPage.savePage(m_pShare);

					// check if path was changed
					if ( !m_pShare->getGuid().empty() && m_pShare->getPath()!=oldPath ) {
						if ( MessageBox(this->getHwnd(),"Share path has been changed. Do you want to re-index this share?",
							TEXT(APP_NAME),MB_ICONQUESTION | MB_YESNO)==IDYES ) {
							Indexer::getInstance()->queue(IndexerJob(m_pShare->getDbId(),false));
						}
					}

					// check if no permissions was set
					if ( m_pShare->getPermissions().empty() )
					{
						if ( MessageBox(this->getHwnd(),"Should all authenticated users be able to access this share?",
							TEXT(APP_NAME),MB_ICONQUESTION | MB_YESNO)==IDYES ) 
						{
							// add permission for everyone
							m_pShare->addPermission(Permission("","","",true));
						}
					}

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

void ShareDialog::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hTreeView = GetDlgItem(hWnd,IDC_TREE_SETTINGS);

	WinUtil::TreeViewUtil::insertItem(m_hTreeView,NULL,"Setup",(LPARAM)&m_setupPage);
	WinUtil::TreeViewUtil::insertItem(m_hTreeView,NULL,"Permissions",(LPARAM)&m_permissionsPage);

	m_setupPage.init(hWnd,MAKEINTRESOURCE(IDD_PAGE_SHARESETUP),NULL);
	m_permissionsPage.init(hWnd,MAKEINTRESOURCE(IDD_PAGE_SHAREPERMISSIONS),NULL);

	WinUtil::WindowUtil::centerParent(hWnd);

	m_pShare = (Share*)lParam;
	if ( m_pShare->getName().empty() ) {
		SetWindowText(hWnd,WinUtil::ResourceUtil::loadString(IDS_ADDSHARE).c_str());
	}
	else {
		SetWindowText(hWnd,WinUtil::ResourceUtil::loadString(IDS_EDITSHARE).c_str());
	}

	TreeView_SelectItem(m_hTreeView,TreeView_GetRoot(m_hTreeView));
	SetFocus(m_hTreeView);

	m_setupPage.loadPage(m_pShare);
	m_permissionsPage.loadPage(m_pShare);
}
