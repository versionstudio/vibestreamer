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
#include "settingstab.h"

#include "../server/configmanager.h"

INT CALLBACK SettingsTab::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case UWM_SETTINGS_CHANGED:
		{
			if ( !IsWindowEnabled(m_hButtonSaveChanges) ) {
				EnableWindow(m_hButtonSaveChanges,true);
			}
		}
		break;

		case WM_COMMAND:
		{
			switch ( wParam )
			{
				case IDC_BUTTON_SAVECHANGES:
				{
					m_serverPage.savePage();
					m_generalPage.savePage();
					m_indexingPage.savePage();
					m_sslPage.savePage();
					m_loggingPage.savePage();

					ConfigManager::getInstance()->save();
					SettingsManager::getInstance()->save();

					EnableWindow(m_hButtonSaveChanges,false);
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

void SettingsTab::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hButtonSaveChanges = GetDlgItem(hWnd,IDC_BUTTON_SAVECHANGES);
	m_hTreeView = GetDlgItem(hWnd,IDC_TREE_SETTINGS);

	WinUtil::TreeViewUtil::insertItem(m_hTreeView,NULL,"Server",(LPARAM)&m_serverPage);
	WinUtil::TreeViewUtil::insertItem(m_hTreeView,NULL,"General",(LPARAM)&m_generalPage);
	WinUtil::TreeViewUtil::insertItem(m_hTreeView,NULL,"Indexing",(LPARAM)&m_indexingPage);
	WinUtil::TreeViewUtil::insertItem(m_hTreeView,NULL,"SSL",(LPARAM)&m_sslPage);	
	WinUtil::TreeViewUtil::insertItem(m_hTreeView,NULL,"Logging",(LPARAM)&m_loggingPage);

	m_serverPage.init(hWnd,MAKEINTRESOURCE(IDD_PAGE_SERVER),NULL);
	m_generalPage.init(hWnd,MAKEINTRESOURCE(IDD_PAGE_GENERAL),NULL);
	m_indexingPage.init(hWnd,MAKEINTRESOURCE(IDD_PAGE_INDEXING),NULL);
	m_sslPage.init(hWnd,MAKEINTRESOURCE(IDD_PAGE_SSL),NULL);
	m_loggingPage.init(hWnd,MAKEINTRESOURCE(IDD_PAGE_LOGGING),NULL);

	SetWindowPos(hWnd,HWND_TOP,WinUtil::DpiUtil::scaleX(15),WinUtil::DpiUtil::scaleY(30),-1,-1,SWP_NOSIZE);

	TreeView_SelectItem(m_hTreeView,TreeView_GetRoot(m_hTreeView));

	dialogRefresh();
}

void SettingsTab::dialogRefresh()
{
	m_serverPage.loadPage();
	m_generalPage.loadPage();
	m_indexingPage.loadPage();
	m_sslPage.loadPage();
	m_loggingPage.loadPage();

	EnableWindow(m_hButtonSaveChanges,false);
}
