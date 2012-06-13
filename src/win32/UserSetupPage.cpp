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
#include "usersetuppage.h"

#include "../server/usermanager.h"

const std::string UserSetupPage::ENCRYPTED_PASSWORD = "********";

INT CALLBACK UserSetupPage::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
		{
			WORD command = HIWORD(wParam);

			switch ( command )
			{
				case EN_CHANGE:
				{
					if ( (HWND)lParam==m_hEditPassword ) 
					{
						if ( GetWindowTextLength(m_hEditPassword)>0 )
						{
							char sz[255];
							memset(sz,0,sizeof(sz));
							GetWindowText(m_hEditPassword,sz,255);
							if ( std::string(sz)!=ENCRYPTED_PASSWORD ) {
								m_newPassword = sz;
							}
						}
					}
				}
				break;
				case EN_KILLFOCUS:
				{
					if ( (HWND)lParam==m_hEditPassword ) {
						if ( GetWindowTextLength(m_hEditPassword)>0 ) {
							SetWindowText(m_hEditPassword,ENCRYPTED_PASSWORD.c_str());
						}
					}
				}
				break;

				case EN_SETFOCUS:
				{
					if ( (HWND)lParam==m_hEditPassword ) {
						SetWindowText(m_hEditPassword,"");
					}
				}
				break;
			}
		}
		break;
	}

	return 0;
}

void UserSetupPage::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hCheckDisabled = GetDlgItem(hWnd,IDC_CHECK_DISABLED);
	m_hListGroups = GetDlgItem(hWnd,IDC_LIST_GROUPS);
	m_hEditName = GetDlgItem(hWnd,IDC_EDIT_NAME);
	m_hEditPassword = GetDlgItem(hWnd,IDC_EDIT_PASSWORD);

	SetWindowPos(m_hEditName,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hEditPassword,m_hEditName,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hCheckDisabled,m_hEditPassword,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hListGroups,m_hCheckDisabled,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);

	SendMessage(m_hListGroups,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES );
	WinUtil::ListViewUtil::insertColumn(m_hListGroups,0,"",WinUtil::DpiUtil::scaleX(306),false);

	SetWindowPos(hWnd,HWND_TOP,166,5,-1,-1,SWP_NOSIZE);
}

void UserSetupPage::loadPage(User *pUser)
{
	SetWindowText(m_hEditName,pUser->getName().c_str());

	if ( !pUser->getPassword().empty() ) {
		SetWindowText(m_hEditPassword,ENCRYPTED_PASSWORD.c_str());
	}

	Button_SetCheck(m_hCheckDisabled,pUser->isDisabled());

	m_groups = UserManager::getInstance()->getGroups();

	// add all groups to list
	std::list<Group>::iterator iter;
	for ( iter=m_groups.begin(); iter!=m_groups.end(); iter++ )
	{
		// add group to list with group instance as lparam
		int index = WinUtil::ListViewUtil::insertItem(m_hListGroups,-1,
			iter->getName().c_str(),(LPARAM)&(*iter));
		
		// check if user is a member of this group
		if ( pUser->isMemberOf(iter->getGuid()) ) {
			ListView_SetCheckState(m_hListGroups,index,true);
		}
	}
}

void UserSetupPage::savePage(User *pUser)
{
	char sz[255];

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditName,sz,sizeof(sz));
	pUser->setName(sz);

	if ( !m_newPassword.empty() ) {
		pUser->setPassword(m_newPassword.c_str());
	}

	pUser->setDisabled(Button_GetCheck(m_hCheckDisabled));

	std::list<std::string> groupGuids;

	int groupsCount = ListView_GetItemCount(m_hListGroups);
	for ( int i=0; i<groupsCount; i++ ) 
	{
		if ( ListView_GetCheckState(m_hListGroups,i) ) {
			Group *pGroup = (Group*)WinUtil::ListViewUtil::getItemParam(m_hListGroups,i);
			groupGuids.push_back(pGroup->getGuid());
		}
	}

	pUser->setGroups(groupGuids);
}
