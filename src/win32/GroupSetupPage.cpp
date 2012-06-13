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
#include "groupsetuppage.h"

void GroupSetupPage::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hCheckDisabled = GetDlgItem(hWnd,IDC_CHECK_DISABLED);
	m_hEditName = GetDlgItem(hWnd,IDC_EDIT_NAME);

	SetWindowPos(m_hEditName,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hCheckDisabled,m_hEditName,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(hWnd,HWND_TOP,166,5,-1,-1,SWP_NOSIZE);
}

void GroupSetupPage::loadPage(Group *pGroup)
{
	SetWindowText(m_hEditName,pGroup->getName().c_str());
	Button_SetCheck(m_hCheckDisabled,pGroup->isDisabled());
}

void GroupSetupPage::savePage(Group *pGroup)
{
	char sz[255];

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditName,sz,sizeof(sz));
	pGroup->setName(sz);
	pGroup->setDisabled(Button_GetCheck(m_hCheckDisabled));
}
