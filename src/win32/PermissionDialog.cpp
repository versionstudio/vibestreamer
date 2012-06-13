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
#include "permissiondialog.h"

#include "../server/usermanager.h"

INT CALLBACK PermissionDialog::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
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
					char sz[255];
					bool allowed = Button_GetCheck(m_hRadioAllow);

					std::string ipAddress;
					if ( Button_GetCheck(m_hCheckIpAddressRestriction) )
					{
						memset(sz,0,sizeof(sz));
						GetWindowText(m_hEditIpAddress1,sz,sizeof(sz));
						std::string ipAddress1 = sz;

						memset(sz,0,sizeof(sz));
						GetWindowText(m_hEditIpAddress2,sz,sizeof(sz));
						std::string ipAddress2 = sz;

						memset(sz,0,sizeof(sz));
						GetWindowText(m_hEditIpAddress3,sz,sizeof(sz));
						std::string ipAddress3 = sz;

						memset(sz,0,sizeof(sz));
						GetWindowText(m_hEditIpAddress4,sz,sizeof(sz));
						std::string ipAddress4 = sz;

						ipAddress = ipAddress1 + "." + ipAddress2 + "." +
									ipAddress3 + "." + ipAddress4;

						if ( !Util::StringUtil::isIpAddress(ipAddress,true) ) {
							MessageBox(hWnd,WinUtil::ResourceUtil::loadString(IDS_INVALIDIPADDRESS).c_str(),APP_NAME,MB_ICONEXCLAMATION);
							break;
						}
					}

					std::string groupGuid;
					std::string userGuid;

					int selectedIndex = ComboBox_GetCurSel(m_hComboUsersGroups);
					if ( ComboBox_GetItemData(m_hComboUsersGroups,selectedIndex)!=NULL )
					{
						std::string *pGuid = (std::string*)ComboBox_GetItemData(m_hComboUsersGroups,selectedIndex);

						// check if guid is a group
						std::list<std::string>::iterator iterGroup;
						for ( iterGroup=m_groupGuids.begin(); iterGroup!=m_groupGuids.end(); iterGroup++ ) {
							if ( pGuid==&(*iterGroup) ) {
								groupGuid = *pGuid;
							}
						}

						// check if guid is a user
						std::list<std::string>::iterator iterUser;
						for ( iterUser=m_userGuids.begin(); iterUser!=m_userGuids.end(); iterUser++ ) {
							if ( pGuid==&(*iterUser) ) {
								userGuid = *pGuid;
							}
						}
					}

					*m_pPermission = Permission(userGuid,groupGuid,ipAddress,allowed);

					EndDialog(hWnd,1);
				}
				break;

				default:
				{
					if ( HIWORD(wParam)==BN_CLICKED ) {
						updateDialog();
					}
				}
				break;
			}
		}
		break;
	}

	return 0;
}

void PermissionDialog::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hCheckIpAddressRestriction = GetDlgItem(hWnd,IDC_CHECK_IPADDRESSRESTRICTION);
	m_hComboUsersGroups = GetDlgItem(hWnd,IDC_COMBO_USERSGROUPS);
	m_hEditIpAddress1 = GetDlgItem(hWnd,IDC_EDIT_IPADDRESS1);
	m_hEditIpAddress2 = GetDlgItem(hWnd,IDC_EDIT_IPADDRESS2);
	m_hEditIpAddress3 = GetDlgItem(hWnd,IDC_EDIT_IPADDRESS3);
	m_hEditIpAddress4 = GetDlgItem(hWnd,IDC_EDIT_IPADDRESS4);
	m_hRadioAllow = GetDlgItem(hWnd,IDC_RADIO_ALLOW);
	m_hRadioDeny = GetDlgItem(hWnd,IDC_RADIO_DENY);

	SetWindowPos(m_hRadioAllow,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hComboUsersGroups,m_hRadioAllow,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hCheckIpAddressRestriction,m_hComboUsersGroups,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hEditIpAddress1,m_hCheckIpAddressRestriction,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hEditIpAddress2,m_hEditIpAddress1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hEditIpAddress3,m_hEditIpAddress2,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hEditIpAddress4,m_hEditIpAddress3,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);

	WinUtil::WindowUtil::centerParent(hWnd);

	m_pPermission = (Permission*)lParam;

	SetWindowText(hWnd,"Permission");

	if ( m_pPermission->isAllowed() ) {
		Button_SetCheck(m_hRadioAllow,true);
	}
	else {
		Button_SetCheck(m_hRadioDeny,true);
	}

	// add "everyone" group to combo box
	ComboBox_InsertString(m_hComboUsersGroups,0,WinUtil::ResourceUtil::loadString(IDS_EVERYONE).c_str());
	ComboBox_SelectString(m_hComboUsersGroups,-1,WinUtil::ResourceUtil::loadString(IDS_EVERYONE).c_str());

	bool selectedUserGroupFound = false;

	// add all groups to list
	std::list<Group> groups = UserManager::getInstance()->getGroups();
	std::list<Group>::iterator iterGroup;
	for ( iterGroup=groups.begin(); iterGroup!=groups.end(); iterGroup++ )
	{
		m_groupGuids.push_back(iterGroup->getGuid());

		int index = ComboBox_AddString(m_hComboUsersGroups,iterGroup->getName().c_str());
		ComboBox_SetItemData(m_hComboUsersGroups,index,(LPARAM)&m_groupGuids.back());

		// check if current group should be selected for this permission
		if ( m_pPermission->getGroupGuid()==iterGroup->getGuid() ) {
			ComboBox_SetCurSel(m_hComboUsersGroups,index);
			selectedUserGroupFound = true;
		}
	}

	// add all users to list
	std::list<User> users = UserManager::getInstance()->getUsers();
	std::list<User>::iterator iterUser;
	for ( iterUser=users.begin(); iterUser!=users.end(); iterUser++ ) 
	{
		m_userGuids.push_back(iterUser->getGuid());

		int index = ComboBox_AddString(m_hComboUsersGroups,iterUser->getName().c_str());
		ComboBox_SetItemData(m_hComboUsersGroups,index,(LPARAM)&m_userGuids.back());

		// check if current user should be selected for this permission
		if ( m_pPermission->getUserGuid()==iterUser->getGuid() ) {
			ComboBox_SetCurSel(m_hComboUsersGroups,index);
			selectedUserGroupFound = true;
		}
	}

	if ( Util::StringUtil::isIpAddress(m_pPermission->getRemoteAddress(),true) ) 
	{
		std::vector<std::string> tokens;
		boost::split(tokens,m_pPermission->getRemoteAddress(),boost::is_any_of("."));

		SetWindowText(m_hEditIpAddress1,tokens[0].c_str());
		SetWindowText(m_hEditIpAddress2,tokens[1].c_str());
		SetWindowText(m_hEditIpAddress3,tokens[2].c_str());
		SetWindowText(m_hEditIpAddress4,tokens[3].c_str());
		
		Button_SetCheck(m_hCheckIpAddressRestriction,true);
	}

	updateDialog();

	if ( ( !m_pPermission->getUserGuid().empty() || !m_pPermission->getGroupGuid().empty() )
		&& !selectedUserGroupFound ) {
		MessageBox(this->getHwnd(),"The user or group this permission applies to no longer exists",APP_NAME,MB_ICONEXCLAMATION);
	}
}

void PermissionDialog::updateDialog()
{
	if ( Button_GetCheck(m_hCheckIpAddressRestriction) )
	{
		EnableWindow(m_hEditIpAddress1,true);
		EnableWindow(m_hEditIpAddress2,true);
		EnableWindow(m_hEditIpAddress3,true);
		EnableWindow(m_hEditIpAddress4,true);
	}
	else
	{
		EnableWindow(m_hEditIpAddress1,false);
		EnableWindow(m_hEditIpAddress2,false);
		EnableWindow(m_hEditIpAddress3,false);
		EnableWindow(m_hEditIpAddress4,false);
	}
}
