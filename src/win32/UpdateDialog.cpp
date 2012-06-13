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
#include "updatedialog.h"

#define LOGGER_CLASSNAME "UpdateDialog"

#include "../server/httprequest.h"
#include "../server/httpresponse.h"
#include "../server/logmanager.h"
#include "../server/version.h"

#include "hyperlinks.h"
#include "settingsmanager.h"

INT CALLBACK UpdateDialog::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
		{
			switch ( wParam )
			{
				case IDC_BUTTON_CHECKAGAIN:
				{
					checkForUpdates();
				}
				break;

				case IDC_STATIC_DOWNLOADLINK:
				{
					ShellExecute(hWnd,"open","http://www.vibestreamer.com/downloads/",NULL,NULL,SW_SHOWNORMAL);
				}
				break;
			}
		}
		break;

		case WM_CLOSE:
		{
			ShowWindow(hWnd,SW_HIDE);
		}
		break;
	}

	return 0;
}

void UpdateDialog::dialogInit(HWND hWnd,LPARAM lParam)
{	
	m_hStaticStatus = GetDlgItem(hWnd,IDC_STATIC_STATUS);
	m_hStaticDownloadLink = GetDlgItem(hWnd,IDC_STATIC_DOWNLOADLINK);

	ConvertStaticToHyperlink(m_hStaticDownloadLink);
	WinUtil::WindowUtil::centerParent(hWnd);
}

void UpdateDialog::checkForUpdates()
{
	WaitForSingleObject(m_mutex,INFINITE);

	if ( m_state==State::IDLE) {
		m_state = State::CHECKING;
		_beginthread(threadCallback,0,this);
	}

	ReleaseMutex(m_mutex);
}

void UpdateDialog::run()
{
	std::string statusMsg = "Checking for new updates";

	LogManager::getInstance()->info(LOGGER_CLASSNAME,statusMsg.c_str());
	SetWindowText(m_hStaticStatus,statusMsg.c_str());

	State newState = State::IDLE;

	std::string url;
	std::string currentVersion = APP_VERSION;
	if ( !std::string(APP_MILESTONE).empty() ) {
		currentVersion.append(APP_MILESTONE);
		url = "/vibestreamer/currentmilestone.php";
	}
	else {
		url = "/vibestreamer/currentversion.php";
	}

	HttpClientRequest request("www.vibestreamer.com",url,80);
	HttpClientResponse response;
	if ( request.execute(response) )
	{
		if ( response.getStatusCode()==HttpClientResponse::HTTP_OK )
		{
			// check if app version differs from latest
			std::string latestVersion = response.getBody();
			if ( latestVersion!=currentVersion )
			{
				statusMsg = "Found a new version (" + std::string(APP_NAME) + " " + latestVersion + ")";
				ShowWindow(m_hStaticDownloadLink,SW_SHOW);
				if ( !IsWindowVisible(this->getHwnd()) ) {
					ShowWindow(this->getHwnd(),SW_SHOW);
				}

				newState = State::FOUND;
			}
			else {
				statusMsg = "You are running the latest available version of " + std::string(APP_NAME);
			}
		}
		else {
			statusMsg = "Received invalid response from the update server";
		}
	}
	else {
		statusMsg = "Could not connect to the update server";
	}

	LogManager::getInstance()->info(LOGGER_CLASSNAME,statusMsg.c_str());
	SetWindowText(m_hStaticStatus,statusMsg.c_str());

	WaitForSingleObject(m_mutex,INFINITE);
	m_state = newState;
	ReleaseMutex(m_mutex);
}

void UpdateDialog::threadCallback(void *arg)
{
	((UpdateDialog*)arg)->run();
}
