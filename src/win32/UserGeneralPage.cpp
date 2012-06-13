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
#include "usergeneralpage.h"

INT CALLBACK UserGeneralPage::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
		{
			if ( HIWORD(wParam)==BN_CLICKED ) {
				updateDialog();
			}
		}
		break;
	}

	return 0;
}

void UserGeneralPage::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hCheckAdmin = GetDlgItem(hWnd,IDC_CHECK_ADMIN);
	m_hCheckBrowser = GetDlgItem(hWnd,IDC_CHECK_BROWSER);
	m_hCheckBypassLimits = GetDlgItem(hWnd,IDC_CHECK_BYPASSLIMITS);
	m_hCheckMaxBandwidth = GetDlgItem(hWnd,IDC_CHECK_MAXBANDWIDTH);
	m_hCheckMaxDownload = GetDlgItem(hWnd,IDC_CHECK_MAXDOWNLOAD);
	m_hComboMaxDownloadPeriod = GetDlgItem(hWnd,IDC_COMBO_MAXDOWNLOADPERIOD);
	m_hCheckMaxSessions = GetDlgItem(hWnd,IDC_CHECK_MAXSESSIONS);
	m_hCheckMaxSessionsPerIp = GetDlgItem(hWnd,IDC_CHECK_MAXSESSIONSPERIP);
	m_hEditMaxBandwidth = GetDlgItem(hWnd,IDC_EDIT_MAXBANDWIDTH);
	m_hEditMaxDownload = GetDlgItem(hWnd,IDC_EDIT_MAXDOWNLOAD);
	m_hEditMaxSessions = GetDlgItem(hWnd,IDC_EDIT_MAXSESSIONS);
	m_hEditMaxSessionsPerIp = GetDlgItem(hWnd,IDC_EDIT_MAXSESSIONSPERIP);

	ComboBox_AddString(m_hComboMaxDownloadPeriod,"Day");
	ComboBox_AddString(m_hComboMaxDownloadPeriod,"Week");
	ComboBox_AddString(m_hComboMaxDownloadPeriod,"Month");
	ComboBox_SetItemData(m_hComboMaxDownloadPeriod,0,(int)User::DownloadPeriod::DAY);
	ComboBox_SetItemData(m_hComboMaxDownloadPeriod,1,(int)User::DownloadPeriod::WEEK);
	ComboBox_SetItemData(m_hComboMaxDownloadPeriod,2,(int)User::DownloadPeriod::MONTH);

	SetWindowPos(m_hCheckAdmin,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hCheckBrowser,m_hCheckBrowser,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hCheckBypassLimits,m_hCheckBrowser,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hCheckMaxSessions,m_hCheckBypassLimits,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hEditMaxSessions,m_hCheckMaxSessions,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hCheckMaxSessionsPerIp,m_hEditMaxSessions,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hEditMaxSessionsPerIp,m_hCheckMaxSessionsPerIp,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hCheckMaxDownload,m_hEditMaxSessionsPerIp,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hEditMaxDownload,m_hCheckMaxDownload,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hComboMaxDownloadPeriod,m_hEditMaxDownload,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hCheckMaxBandwidth,m_hComboMaxDownloadPeriod,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	SetWindowPos(m_hEditMaxBandwidth,m_hCheckMaxBandwidth,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);

	SetWindowPos(hWnd,HWND_TOP,166,5,-1,-1,SWP_NOSIZE);
}

void UserGeneralPage::updateDialog()
{
	EnableWindow(m_hEditMaxSessions,Button_GetCheck(m_hCheckMaxSessions));
	EnableWindow(m_hEditMaxSessionsPerIp,Button_GetCheck(m_hCheckMaxSessionsPerIp));

	if ( Button_GetCheck(m_hCheckMaxDownload) ) {
		EnableWindow(m_hEditMaxDownload,true);
		EnableWindow(m_hComboMaxDownloadPeriod,true);
	}
	else {
		EnableWindow(m_hEditMaxDownload,false);
		EnableWindow(m_hComboMaxDownloadPeriod,false);
	}

	EnableWindow(m_hEditMaxBandwidth,Button_GetCheck(m_hCheckMaxBandwidth));
}

void UserGeneralPage::loadPage(User *pUser)
{
	Button_SetCheck(m_hCheckAdmin,pUser->isAdmin());
	Button_SetCheck(m_hCheckBrowser,pUser->isBrowser());
	Button_SetCheck(m_hCheckBypassLimits,pUser->isBypassLimits());

	Button_SetCheck(m_hCheckMaxSessions,pUser->isMaxSessionsEnabled());
	SetWindowText(m_hEditMaxSessions,
		Util::ConvertUtil::toString(pUser->getMaxSessions()).c_str());

	Button_SetCheck(m_hCheckMaxSessionsPerIp,pUser->isMaxSessionsPerIpEnabled());
	SetWindowText(m_hEditMaxSessionsPerIp,
		Util::ConvertUtil::toString(pUser->getMaxSessionsPerIp()).c_str());

	Button_SetCheck(m_hCheckMaxDownload,pUser->isMaxDownloadEnabled());
	SetWindowText(m_hEditMaxDownload,
		Util::ConvertUtil::toString(pUser->getMaxDownloadBytes()/1024/1024).c_str());

	ComboBox_SetCurSel(m_hComboMaxDownloadPeriod,0);
	int maxDownloadPeriodCount = ComboBox_GetCount(m_hComboMaxDownloadPeriod);
	for ( int i=0; i<maxDownloadPeriodCount; i++ ) {
		if ( ComboBox_GetItemData(m_hComboMaxDownloadPeriod,i)==pUser->getMaxDownloadPeriod() ) {
			ComboBox_SetCurSel(m_hComboMaxDownloadPeriod,i);
			break;
		}
	}

	Button_SetCheck(m_hCheckMaxBandwidth,pUser->isMaxBandwidthEnabled());
	SetWindowText(m_hEditMaxBandwidth,
		Util::ConvertUtil::toString(pUser->getMaxBandwidth()).c_str());

	updateDialog();
}

void UserGeneralPage::savePage(User *pUser)
{
	char sz[255];

	pUser->setAdmin(Button_GetCheck(m_hCheckAdmin));
	pUser->setBrowser(Button_GetCheck(m_hCheckBrowser));
	pUser->setBypassLimits(Button_GetCheck(m_hCheckBypassLimits));

	pUser->setMaxSessionsEnabled(Button_GetCheck(m_hCheckMaxSessions));
	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditMaxSessions,sz,sizeof(sz));
	pUser->setMaxSessions(Util::ConvertUtil::toInt(sz));

	pUser->setMaxSessionsPerIpEnabled(Button_GetCheck(m_hCheckMaxSessionsPerIp));
	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditMaxSessionsPerIp,sz,sizeof(sz));
	pUser->setMaxSessionsPerIp(Util::ConvertUtil::toInt(sz));

	pUser->setMaxDownloadEnabled(Button_GetCheck(m_hCheckMaxDownload));

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditMaxDownload,sz,sizeof(sz));
	uint64_t maxDownloadBytes = Util::ConvertUtil::toUnsignedInt64(sz)*1024*1024;
	pUser->setMaxDownloadBytes(maxDownloadBytes);

	int index = ComboBox_GetCurSel(m_hComboMaxDownloadPeriod);
	if ( index>-1 ) {
		pUser->setMaxDownloadPeriod(ComboBox_GetItemData(m_hComboMaxDownloadPeriod,index));
	}

	pUser->setMaxBandwidthEnabled(Button_GetCheck(m_hCheckMaxBandwidth));
	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditMaxBandwidth,sz,sizeof(sz));
	pUser->setMaxBandwidth(Util::ConvertUtil::toInt(sz));
}
