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
#include "userstab.h"

#include "../server/usermanager.h"

#include "groupdialog.h"
#include "userdialog.h"

INT CALLBACK UsersTab::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
		{
			switch ( wParam )
			{
				case UID_ADDGROUP:
				{
					openGroupDialog(NULL);
				}
				break;

				case UID_EDITGROUP:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListGroups);
					if ( selectedIndex!=-1 ) 
					{
						std::string *pGroupGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListGroups,selectedIndex);
						openGroupDialog(pGroupGuid);
					}
				}
				break;

				case UID_DELETEGROUP:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListGroups);
					if ( selectedIndex!=-1 ) 
					{
						std::string *pGroupGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListGroups,selectedIndex);

						Group group;
						if ( UserManager::getInstance()->findGroupByGuid(*pGroupGuid,&group) ) {
							UserManager::getInstance()->removeGroup(group);
						}
						
						dialogRefresh();
					}
				}
				break;

				case UID_ADDUSER:
				{
					openUserDialog(NULL);
				}
				break;

				case UID_EDITUSER:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListUsers);
					if ( selectedIndex!=-1 ) 
					{
						std::string *pUserGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListUsers,selectedIndex);
						openUserDialog(pUserGuid);
					}
				}
				break;

				case UID_DELETEUSER:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListUsers);
					if ( selectedIndex!=-1 ) 
					{
						std::string *pUserGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListUsers,selectedIndex);

						User user;
						if ( UserManager::getInstance()->findUserByGuid(*pUserGuid,&user) ) {
							UserManager::getInstance()->removeUser(user);
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
				case NM_DBLCLK:
				{
					if ( pNmh->hwndFrom==m_hListUsers )
					{
						LPNMITEMACTIVATE pNmItem = (LPNMITEMACTIVATE)lParam;
						if ( pNmItem->iItem!=-1 ) {
							std::string *pUserGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListUsers,pNmItem->iItem);
							openUserDialog(pUserGuid);
						}
					}
					else if ( pNmh->hwndFrom==m_hListGroups )
					{
						LPNMITEMACTIVATE pNmItem = (LPNMITEMACTIVATE)lParam;
						if ( pNmItem->iItem!=-1 ) {
							std::string *pGroupGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListGroups,pNmItem->iItem);
							openGroupDialog(pGroupGuid);
						}
					}
				}
				break;

				case NM_CUSTOMDRAW:
				{
					if ( pNmh->hwndFrom==m_hListGroups ) {
						SendMessage(m_hListGroups,LVM_SETCOLUMNWIDTH,0,LVSCW_AUTOSIZE_USEHEADER);
					}
					else if ( pNmh->hwndFrom==m_hListUsers ) {
						SendMessage(m_hListUsers,LVM_SETCOLUMNWIDTH,0,LVSCW_AUTOSIZE_USEHEADER);
					}
				}
				break;

				case NM_RCLICK:
				{
					if ( pNmh->hwndFrom==m_hListUsers )
					{
						HMENU hPopupMenu;
						hPopupMenu = CreatePopupMenu();

						LPNMITEMACTIVATE pNmItem = (LPNMITEMACTIVATE)lParam;
						if ( pNmItem->iItem!=-1 ) 
						{
							AppendMenu(hPopupMenu,MF_STRING,UID_ADDUSER,WinUtil::ResourceUtil::loadString(IDS_ADD).c_str());
							AppendMenu(hPopupMenu,MF_STRING,UID_EDITUSER,WinUtil::ResourceUtil::loadString(IDS_EDIT).c_str());
							AppendMenu(hPopupMenu,MF_STRING,UID_DELETEUSER,WinUtil::ResourceUtil::loadString(IDS_DELETE).c_str());
						}
						else
						{
							AppendMenu(hPopupMenu,MF_STRING,UID_ADDUSER,WinUtil::ResourceUtil::loadString(IDS_ADD).c_str());
							AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_EDIT).c_str());
							AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_DELETE).c_str());
						}

						WinUtil::MenuUtil::showPopupMenu(hWnd,hPopupMenu);
					}
					else if ( pNmh->hwndFrom==m_hListGroups )
					{
						HMENU hPopupMenu;
						hPopupMenu = CreatePopupMenu();

						LPNMITEMACTIVATE pNmItem = (LPNMITEMACTIVATE)lParam;
						if ( pNmItem->iItem!=-1 ) 
						{
							AppendMenu(hPopupMenu,MF_STRING,UID_ADDGROUP,WinUtil::ResourceUtil::loadString(IDS_ADD).c_str());
							AppendMenu(hPopupMenu,MF_STRING,UID_EDITGROUP,WinUtil::ResourceUtil::loadString(IDS_EDIT).c_str());
							AppendMenu(hPopupMenu,MF_STRING,UID_DELETEGROUP,WinUtil::ResourceUtil::loadString(IDS_DELETE).c_str());
						}
						else
						{
							AppendMenu(hPopupMenu,MF_STRING,UID_ADDGROUP,WinUtil::ResourceUtil::loadString(IDS_ADD).c_str());
							AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_EDIT).c_str());
							AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_DELETE).c_str());
						}

						WinUtil::MenuUtil::showPopupMenu(hWnd,hPopupMenu);
					}
				}
				break;
			}
		}
		break;
	}

	return 0;
}

void UsersTab::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hListGroups = GetDlgItem(hWnd,IDC_LIST_GROUPS);
	m_hListUsers = GetDlgItem(hWnd,IDC_LIST_USERS);

	SendMessage(m_hListGroups,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT);
	WinUtil::ListViewUtil::insertColumn(m_hListGroups,0,"",0,false);

	SendMessage(m_hListUsers,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT);
	WinUtil::ListViewUtil::insertColumn(m_hListUsers,0,"",0,false);

	SetWindowText(GetDlgItem(hWnd,IDC_STATIC_GROUP_GROUPS),WinUtil::ResourceUtil::loadString(IDS_GROUPS).c_str());
	SetWindowText(GetDlgItem(hWnd,IDC_STATIC_GROUP_USERS),WinUtil::ResourceUtil::loadString(IDS_USERS).c_str());
	SetWindowPos(hWnd,HWND_TOP,WinUtil::DpiUtil::scaleX(15),WinUtil::DpiUtil::scaleY(30),-1,-1,SWP_NOSIZE);

	dialogRefresh();
}

void UsersTab::dialogRefresh()
{
	ListView_DeleteAllItems(m_hListGroups);
	m_groupGuids.clear();

	ListView_DeleteAllItems(m_hListUsers);
	m_userGuids.clear();

	// load all groups from the user manager
	std::list<Group> groups = UserManager::getInstance()->getGroups();
	std::list<Group>::iterator iterGroup;
	for ( iterGroup=groups.begin(); iterGroup!=groups.end(); iterGroup++ ) {
		m_groupGuids.push_back(iterGroup->getGuid());
		WinUtil::ListViewUtil::insertItem(m_hListGroups,0,iterGroup->getName().c_str(),(LPARAM)&m_groupGuids.back());
	}

	// load all users from the user manager
	std::list<User> users = UserManager::getInstance()->getUsers();
	std::list<User>::iterator iterUser;
	for ( iterUser=users.begin(); iterUser!=users.end(); iterUser++ ) {
		m_userGuids.push_back(iterUser->getGuid());
		WinUtil::ListViewUtil::insertItem(m_hListUsers,0,iterUser->getName().c_str(),(LPARAM)&m_userGuids.back());
	}
}

void UsersTab::openGroupDialog(std::string *pGroupGuid)
{
	GroupDialog groupDialog;

	if ( pGroupGuid!=NULL )
	{
		Group group;
		if ( !UserManager::getInstance()->findGroupByGuid(*pGroupGuid,&group) ) {
			MessageBox(this->getHwnd(),"Group no longer exists",APP_NAME,MB_ICONEXCLAMATION);
			return;
		}

		if ( groupDialog.init(this->getHwnd(),MAKEINTRESOURCE(IDD_GROUP),(LPARAM)&group) ) {
			UserManager::getInstance()->updateGroup(group);
			dialogRefresh();
		}
	}
	else
	{
		Group group;
		if ( groupDialog.init(this->getHwnd(),MAKEINTRESOURCE(IDD_GROUP),(LPARAM)&group) ) {
			UserManager::getInstance()->addGroup(group);
			dialogRefresh();
		}		
	}
}

void UsersTab::openUserDialog(std::string *pUserGuid)
{
	UserDialog userDialog;

	if ( pUserGuid!=NULL )
	{
		User user;
		if ( !UserManager::getInstance()->findUserByGuid(*pUserGuid,&user) ) {
			MessageBox(this->getHwnd(),"User no longer exists",APP_NAME,MB_ICONEXCLAMATION);
			return;
		}

		if ( userDialog.init(this->getHwnd(),MAKEINTRESOURCE(IDD_USER),(LPARAM)&user) ) {
			UserManager::getInstance()->updateUser(user);
			dialogRefresh();
		}
	}
	else
	{
		User user;
		if ( userDialog.init(this->getHwnd(),MAKEINTRESOURCE(IDD_USER),(LPARAM)&user) ) {
			UserManager::getInstance()->addUser(user);
			dialogRefresh();
		}		
	}
}
