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
#include "securitytab.h"

#include "../server/sitemanager.h"
#include "../server/usermanager.h"

#include "permissiondialog.h"

INT CALLBACK SecurityTab::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
		{
			switch ( wParam )
			{
				case UID_ADD:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListSitePermissions);
					if ( selectedIndex!=-1 ) {
						Site *pSite = findOwnerSiteByIndex(selectedIndex);
						openPermissionDialog(pSite,NULL);
					}
				}
				break;

				case UID_EDIT:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListSitePermissions);
					if ( selectedIndex!=-1 ) 
					{
						Site *pSite = findOwnerSiteByIndex(selectedIndex);
						openPermissionDialog(pSite,
							(Permission*)WinUtil::ListViewUtil::getItemParam(m_hListSitePermissions,selectedIndex));
					}
				}
				break;

				case UID_REMOVE:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListSitePermissions);
					if ( selectedIndex!=-1 ) 
					{
						Site *pSite = findOwnerSiteByIndex(selectedIndex);
						Permission *pPermission = (Permission*)WinUtil::ListViewUtil::getItemParam(m_hListSitePermissions,selectedIndex);
						if ( pPermission!=NULL ) {
							pSite->removePermission(*pPermission);
						}

						dialogRefresh();
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
				{
					if ( pNmh->hwndFrom==m_hListSitePermissions ) {
						SendMessage(m_hListSitePermissions,LVM_SETCOLUMNWIDTH,2,LVSCW_AUTOSIZE_USEHEADER);
					}
				}
				break;

				case NM_CUSTOMDRAW:
				{
					if ( pNmh->hwndFrom==m_hListSitePermissions ) {
						SendMessage(m_hListSitePermissions,LVM_SETCOLUMNWIDTH,2,LVSCW_AUTOSIZE_USEHEADER);
						SetWindowLong(hWnd,DWL_MSGRESULT,(LONG)customDrawListItems(lParam));
						return TRUE;
					}
				}
				break;

				case NM_DBLCLK:
				{
					if ( pNmh->hwndFrom==m_hListSitePermissions )
					{
						LPNMITEMACTIVATE pNmItem = (LPNMITEMACTIVATE)lParam;
						if ( pNmItem->iItem!=-1 )
						{
							if ( checkSite(pNmItem->iItem) ) {
								toggle(pNmItem->iItem);
							}
							else
							{
								Site *pSite = findOwnerSiteByIndex(pNmItem->iItem);
								openPermissionDialog(pSite,
									(Permission*)WinUtil::ListViewUtil::getItemParam(m_hListSitePermissions,pNmItem->iItem));
							}
						}
					}
				}
				break;

				case NM_RCLICK:
				{
					if ( pNmh->hwndFrom==m_hListSitePermissions )
					{
						HMENU hPopupMenu;
						hPopupMenu = CreatePopupMenu();

						LPNMITEMACTIVATE pNmItem = (LPNMITEMACTIVATE)lParam;
						if ( pNmItem->iItem!=-1 ) 
						{
							AppendMenu(hPopupMenu,MF_STRING,UID_ADD,WinUtil::ResourceUtil::loadString(IDS_ADD).c_str());

							// check indent to see if item is a site or a permission
							if ( WinUtil::ListViewUtil::getItemImageIndent(m_hListSitePermissions,pNmItem->iItem)<1 ) {
								AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_EDIT).c_str());
								AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_REMOVE).c_str());
							}
							else {
								AppendMenu(hPopupMenu,MF_STRING,UID_EDIT,WinUtil::ResourceUtil::loadString(IDS_EDIT).c_str());
								AppendMenu(hPopupMenu,MF_STRING,UID_REMOVE,WinUtil::ResourceUtil::loadString(IDS_REMOVE).c_str());
							}
						}
						else
						{
							AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_ADD).c_str());
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

void SecurityTab::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hListSitePermissions = GetDlgItem(hWnd,IDC_LIST_SITEPERMISSIONS);
	m_hImageList = ImageList_Create(16,16,ILC_COLOR16|ILC_MASK,2,0);

	HANDLE hBitmap = LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_TOGGLE),IMAGE_BITMAP,0,0,LR_SHARED);
	if ( hBitmap!=NULL ) {
		ImageList_AddMasked(m_hImageList,(HBITMAP)hBitmap,RGB(255,0,255));
	}

	hBitmap = LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_USERGROUP),IMAGE_BITMAP,0,0,LR_SHARED);
	if ( hBitmap!=NULL ) {
		ImageList_AddMasked(m_hImageList,(HBITMAP)hBitmap,RGB(255,0,255));
	}

	SendMessage(m_hListSitePermissions,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT);
	ListView_SetImageList(m_hListSitePermissions,m_hImageList,LVSIL_SMALL);
	WinUtil::ListViewUtil::insertColumn(m_hListSitePermissions,0,WinUtil::ResourceUtil::loadString(IDS_NAME).c_str(),WinUtil::DpiUtil::scaleX(270),false);
	WinUtil::ListViewUtil::insertColumn(m_hListSitePermissions,1,WinUtil::ResourceUtil::loadString(IDS_IPADDRESS).c_str(),WinUtil::DpiUtil::scaleX(115),false);
	WinUtil::ListViewUtil::insertColumn(m_hListSitePermissions,2,WinUtil::ResourceUtil::loadString(IDS_ALLOWED).c_str(),0,false);

	// create all site containers
	std::list<Site> &sites = SiteManager::getInstance()->getSites();
	std::list<Site>::iterator iter;
	for ( iter=sites.begin(); iter!=sites.end(); iter++ ) {
		m_siteContainers.push_back(SecurityTabSiteContainer(&(*iter),true));
	}

	SetWindowPos(hWnd,HWND_TOP,WinUtil::DpiUtil::scaleX(15),WinUtil::DpiUtil::scaleY(30),-1,-1,SWP_NOSIZE);

	dialogRefresh();
}

void SecurityTab::dialogRefresh()
{
	ListView_DeleteAllItems(m_hListSitePermissions);

	// insert all sites and permissions from site containers
	std::list<SecurityTabSiteContainer>::iterator iter;
	for ( iter=m_siteContainers.begin(); iter!=m_siteContainers.end(); iter++ )
	{
		Site *pSite = iter->getSite();

		if ( iter->isContainerVisible() )
		{
			int index = WinUtil::ListViewUtil::insertItem(m_hListSitePermissions,0,pSite->getName(),(LPARAM)pSite,1);
			iter->setPermissions(pSite->getPermissions());
			const std::list<Permission> &permissions = iter->getPermissions();

			// iterate through all permissions and add them to the list
			std::list<Permission>::const_iterator permissionIter;
			for ( permissionIter=permissions.begin(); permissionIter!=permissions.end(); permissionIter++ ) 
			{
				std::string name;
				std::string ipAddress;
				std::string allowed;

				int imageIndex = 0;

				if ( !permissionIter->getUserGuid().empty() ) 
				{
					User user;
					if ( !UserManager::getInstance()->findUserByGuid(permissionIter->getUserGuid(),&user) ) {
						continue;
					}

					name = user.getName();
					imageIndex = 2;
				}
				else if ( !permissionIter->getGroupGuid().empty() ) 
				{
					Group group;
					if ( !UserManager::getInstance()->findGroupByGuid(permissionIter->getGroupGuid(),&group) ) {
						continue;
					}

					name = group.getName();
					imageIndex = 3;
				}
				else {
					name = WinUtil::ResourceUtil::loadString(IDS_EVERYONE);
					imageIndex = 3;
				}

				ipAddress = permissionIter->getRemoteAddress();
				if ( permissionIter->getRemoteAddress().empty() ) {
					ipAddress = WinUtil::ResourceUtil::loadString(IDS_ANY);
				}

				if ( permissionIter->isAllowed() ) {
					allowed = WinUtil::ResourceUtil::loadString(IDS_YES);
				}
				else {
					allowed = WinUtil::ResourceUtil::loadString(IDS_NO);
				}

				index = WinUtil::ListViewUtil::insertItem(m_hListSitePermissions,++index,name,(LPARAM)&(*permissionIter),imageIndex,1);
				WinUtil::ListViewUtil::insertSubItem(m_hListSitePermissions,index,1,ipAddress);
				WinUtil::ListViewUtil::insertSubItem(m_hListSitePermissions,index,2,allowed);
			}
		}
		else {
			WinUtil::ListViewUtil::insertItem(m_hListSitePermissions,0,pSite->getName(),(LPARAM)pSite,0);
		}
	}
}

LRESULT SecurityTab::customDrawListItems(LPARAM lParam)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;

    switch(lplvcd->nmcd.dwDrawStage) 
    {
        case CDDS_PREPAINT:
		{
            return CDRF_NOTIFYITEMDRAW;
		}
		break;
            
        case CDDS_ITEMPREPAINT:
		{
			if ( checkSite((int)lplvcd->nmcd.dwItemSpec) ) {
				lplvcd->clrText = RGB(80,80,80);
				SelectFont(lplvcd->nmcd.hdc,GetStockObject(DEFAULT_GUI_FONT));
			}
		}
		break;
    }

    return CDRF_DODEFAULT;
}

void SecurityTab::openPermissionDialog(Site *pSite,Permission *pPermission)
{
	PermissionDialog dialog;

	if ( pPermission!=NULL )
	{
		Permission newPermission = *pPermission;

		if ( dialog.init(this->getHwnd(),MAKEINTRESOURCE(IDD_PERMISSION),(LPARAM)&newPermission) ) 
		{
			pSite->removePermission(*pPermission);
			pSite->addPermission(newPermission);
			dialogRefresh();
		}
	}
	else
	{
		Permission permission;
		if ( dialog.init(this->getHwnd(),MAKEINTRESOURCE(IDD_PERMISSION),(LPARAM)&permission) ) 
		{
			pSite->addPermission(permission);
			dialogRefresh();
		}
	}
}

void SecurityTab::toggle(int siteIndex)
{
	Site *pSite = (Site*)WinUtil::ListViewUtil::getItemParam(m_hListSitePermissions,siteIndex);
	SecurityTabSiteContainer *pSiteContainer = NULL;

	std::list<SecurityTabSiteContainer>::iterator iter;
	for ( iter=m_siteContainers.begin(); iter!=m_siteContainers.end(); iter++ ) {
		if ( iter->getSite()==pSite ) {
			pSiteContainer = &(*iter);
		}
	}

	int imageIndex = WinUtil::ListViewUtil::getItemImageIndex(m_hListSitePermissions,siteIndex);
	if ( imageIndex==0 ) {
		WinUtil::ListViewUtil::setItemImageIndex(m_hListSitePermissions,siteIndex,1);
		pSiteContainer->setContainerVisible(true);
	}
	else {
		WinUtil::ListViewUtil::setItemImageIndex(m_hListSitePermissions,siteIndex,0);
		pSiteContainer->setContainerVisible(false);
	}

	dialogRefresh();
}

bool SecurityTab::checkSite(int index) 
{
	// check indent to see if item is a site or a permission
	if ( WinUtil::ListViewUtil::getItemImageIndent(m_hListSitePermissions,index)<1 ) {
		return true;
	}

	return false;
}

Site* SecurityTab::findOwnerSiteByIndex(int index)
{
	while ( index>-1 ) 
	{
		if ( checkSite(index) ) {
			return (Site*)WinUtil::ListViewUtil::getItemParam(m_hListSitePermissions,index);
		}

		index--;
	}

	return NULL;
}
