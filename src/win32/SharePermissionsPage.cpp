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
#include "sharepermissionspage.h"

#include "../server/usermanager.h"

#include "permissiondialog.h"

INT CALLBACK SharePermissionsPage::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
		{
			switch ( wParam )
			{
				case UID_ADD:
				{
					openPermissionDialog(NULL);
				}
				break;

				case UID_EDIT:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListPermissions);
					if ( selectedIndex!=-1 ) {
						openPermissionDialog((Permission*)WinUtil::ListViewUtil::getItemParam(m_hListPermissions,selectedIndex));
					}
				}
				break;

				case UID_REMOVE:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListPermissions);
					if ( selectedIndex!=-1 ) {
						Permission *pPermission = (Permission*)WinUtil::ListViewUtil::getItemParam(m_hListPermissions,selectedIndex);
						m_pShare->removePermission(*pPermission);
						updateDialog();
					}
				}
				break;
			}
		}
		break;

		case WM_NOTIFY:
		{
			LPNMHDR pNmh = (LPNMHDR)lParam;
			switch ( pNmh->code )
			{
				case LVN_DELETEITEM:
				case LVN_INSERTITEM:
				case NM_CUSTOMDRAW:
				{
					if ( pNmh->hwndFrom==m_hListPermissions ) {
						SendMessage(m_hListPermissions,LVM_SETCOLUMNWIDTH,2,LVSCW_AUTOSIZE_USEHEADER);
					}
				}
				break;

				case NM_DBLCLK:
				{
					if ( pNmh->hwndFrom==m_hListPermissions )
					{
						LPNMITEMACTIVATE pNmItem = (LPNMITEMACTIVATE)lParam;
						if ( pNmItem->iItem!=-1 ) {
							openPermissionDialog((Permission*)WinUtil::ListViewUtil::getItemParam(m_hListPermissions,pNmItem->iItem));
						}
					}
				}
				break;

				case NM_RCLICK:
				{
					if ( pNmh->hwndFrom==m_hListPermissions )
					{
						HMENU hPopupMenu;
						hPopupMenu = CreatePopupMenu();

						LPNMITEMACTIVATE pNmItem = (LPNMITEMACTIVATE)lParam;
						if ( pNmItem->iItem!=-1 ) 
						{
							AppendMenu(hPopupMenu,MF_STRING,UID_ADD,WinUtil::ResourceUtil::loadString(IDS_ADD).c_str());
							AppendMenu(hPopupMenu,MF_STRING,UID_EDIT,WinUtil::ResourceUtil::loadString(IDS_EDIT).c_str());
							AppendMenu(hPopupMenu,MF_STRING,UID_REMOVE,WinUtil::ResourceUtil::loadString(IDS_REMOVE).c_str());
						}
						else
						{
							AppendMenu(hPopupMenu,MF_STRING,UID_ADD,WinUtil::ResourceUtil::loadString(IDS_ADD).c_str());
							AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_EDIT).c_str());
							AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_REMOVE).c_str());
						}

						WinUtil::MenuUtil::showPopupMenu(hWnd,hPopupMenu);
					}
				}
			}
		}
		break;
	}

	return 0;
}

void SharePermissionsPage::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hListPermissions = GetDlgItem(hWnd,IDC_LIST_PERMISSIONS);
	m_hImageList = ImageList_Create(16,16,ILC_COLOR16|ILC_MASK,2,0);

	HANDLE hBitmap = LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_USERGROUP),IMAGE_BITMAP,0,0,LR_SHARED);
	if ( hBitmap!=NULL ) {
		ImageList_AddMasked(m_hImageList,(HBITMAP)hBitmap,RGB(255,0,255));
	}

	SendMessage(m_hListPermissions,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT );
	ListView_SetImageList(m_hListPermissions,m_hImageList,LVSIL_SMALL);
	WinUtil::ListViewUtil::insertColumn(m_hListPermissions,0,WinUtil::ResourceUtil::loadString(IDS_NAME).c_str(),127,false);
	WinUtil::ListViewUtil::insertColumn(m_hListPermissions,1,WinUtil::ResourceUtil::loadString(IDS_IPADDRESS).c_str(),105,false);
	WinUtil::ListViewUtil::insertColumn(m_hListPermissions,2,WinUtil::ResourceUtil::loadString(IDS_ALLOWED).c_str(),0,false);

	SetWindowPos(hWnd,HWND_TOP,166,5,-1,-1,SWP_NOSIZE);
}

void SharePermissionsPage::loadPage(Share *pShare)
{
	m_pShare = pShare;
	updateDialog();
}

void SharePermissionsPage::savePage(Share *pShare)
{

}

void SharePermissionsPage::updateDialog()
{
	ListView_DeleteAllItems(m_hListPermissions);

	const std::list<Permission> &permissions = m_pShare->getPermissions();

	std::list<Permission>::const_iterator iter;
	for ( iter=permissions.begin(); iter!=permissions.end(); iter++ ) 
	{
		std::string name;
		std::string ipAddress;
		std::string allowed;

		int imageIndex = 0;

		if ( !iter->getUserGuid().empty() )
		{
			User user;
			if ( !UserManager::getInstance()->findUserByGuid(iter->getUserGuid(),&user) ) {
				continue;
			}

			name = user.getName();
			imageIndex = 0;
		}
		else if ( !iter->getGroupGuid().empty() ) 
		{
			Group group;
			if ( !UserManager::getInstance()->findGroupByGuid(iter->getGroupGuid(),&group) ) {
				continue;
			}

			name = group.getName();
			imageIndex = 1;
		}
		else
		{
			name = ipAddress = WinUtil::ResourceUtil::loadString(IDS_EVERYONE);
			imageIndex = 1;
		}

		ipAddress = iter->getRemoteAddress();
		if ( iter->getRemoteAddress().empty() ) {
			ipAddress = WinUtil::ResourceUtil::loadString(IDS_ANY);
		}

		if ( iter->isAllowed() ) {
			allowed = WinUtil::ResourceUtil::loadString(IDS_YES);
		}
		else {
			allowed = WinUtil::ResourceUtil::loadString(IDS_NO);
		}

		int index = WinUtil::ListViewUtil::insertItem(m_hListPermissions,0,name,(LPARAM)&(*iter),imageIndex);
		WinUtil::ListViewUtil::insertSubItem(m_hListPermissions,index,1,ipAddress);
		WinUtil::ListViewUtil::insertSubItem(m_hListPermissions,index,2,allowed);
	}
}

void SharePermissionsPage::openPermissionDialog(Permission *pPermission)
{
	PermissionDialog dialog;

	if ( pPermission!=NULL )
	{
		Permission newPermission = *pPermission;

		if ( dialog.init(this->getHwnd(),MAKEINTRESOURCE(IDD_PERMISSION),(LPARAM)&newPermission) ) {
			m_pShare->removePermission(*pPermission);
			m_pShare->addPermission(newPermission);
			updateDialog();
		}
	}
	else
	{
		Permission permission;
		if ( dialog.init(this->getHwnd(),MAKEINTRESOURCE(IDD_PERMISSION),(LPARAM)&permission) ) {
			m_pShare->addPermission(permission);
			updateDialog();
		}
	}
}
