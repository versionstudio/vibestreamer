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
#include "userstatisticspage.h"

#include "../server/statisticsmanager.h"

void UserStatisticsPage::dialogInit(HWND hWnd,LPARAM lParam)
{
	SetWindowPos(hWnd,HWND_TOP,166,5,-1,-1,SWP_NOSIZE);
}

void UserStatisticsPage::loadPage(User *pUser)
{
	if ( pUser->getGuid().empty() ) {
		return;
	}

	SetWindowText(GetDlgItem(this->getHwnd(),IDC_STATIC_VALUE_NUMBEROFLOGINS),
		Util::ConvertUtil::toString(pUser->getLogins()).c_str());

	std::string lastKnownIp = pUser->getLastKnownIp();
	if ( !lastKnownIp.empty() ) {
		SetWindowText(GetDlgItem(this->getHwnd(),IDC_STATIC_VALUE_LASTKNOWNIP),lastKnownIp.c_str());
	}

	time_t lastLoginTime = pUser->getLastLoginTime();
	if ( lastLoginTime>0 ) {
		tm localTime;
		Util::TimeUtil::getLocalTime(lastLoginTime,&localTime);
		std::string formattedLastLoginTime = Util::TimeUtil::format(localTime,"%Y-%m-%d %H:%M:%S");
		SetWindowText(GetDlgItem(this->getHwnd(),IDC_STATIC_VALUE_LASTLOGIN),formattedLastLoginTime.c_str());
	}

	DownloadStatistics downloadStatistics = StatisticsManager::getInstance()->getDownloadStatistics(*pUser);

	// downloaded today
	std::stringstream dayDownloadStats;
	dayDownloadStats <<  WinUtil::FormatUtil::formatFileSize(downloadStatistics.getDayDownloadedBytes())
		<< " ( " << downloadStatistics.getDayDownloadedFiles() << " files)";

	SetWindowText(GetDlgItem(this->getHwnd(),IDC_STATIC_DAYDOWNLOAD),dayDownloadStats.str().c_str());

	// downloaded this month
	std::stringstream monthDownloadStats;
	monthDownloadStats <<  WinUtil::FormatUtil::formatFileSize(downloadStatistics.getMonthDownloadedBytes())
		<< " ( " << downloadStatistics.getMonthDownloadedFiles() << " files)";

	SetWindowText(GetDlgItem(this->getHwnd(),IDC_STATIC_MONTHDOWNLOAD),monthDownloadStats.str().c_str());

	// downloaded this week
	std::stringstream weekDownloadStats;
	weekDownloadStats <<  WinUtil::FormatUtil::formatFileSize(downloadStatistics.getWeekDownloadedBytes())
		<< " ( " << downloadStatistics.getWeekDownloadedFiles() << " files)";

	SetWindowText(GetDlgItem(this->getHwnd(),IDC_STATIC_WEEKDOWNLOAD),weekDownloadStats.str().c_str());

	// total downloaded
	std::stringstream totalDownloadStats;
	totalDownloadStats <<  WinUtil::FormatUtil::formatFileSize(downloadStatistics.getTotalDownloadedBytes())
		<< " ( " << downloadStatistics.getTotalDownloadedFiles() << " files)";

	SetWindowText(GetDlgItem(this->getHwnd(),IDC_STATIC_TOTALDOWNLOAD),totalDownloadStats.str().c_str());
}

void UserStatisticsPage::savePage(User *pUser)
{

}
