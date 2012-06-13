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
#include "sslpage.h"

#include "../server/configmanager.h"

INT CALLBACK SslPage::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
		{
			if ( HIWORD(wParam)==EN_CHANGE || HIWORD(wParam)==BN_CLICKED )
			{
				if ( IsWindowVisible(hWnd) ) {
					SendMessage(GetParent(hWnd),UWM_SETTINGS_CHANGED,NULL,NULL);
				}

				updateDialog();
			}
			
			if ( wParam==IDC_BUTTON_BROWSESSLCERTIFICATEFILE ||
				wParam==IDC_BUTTON_BROWSESSLCERTIFICATEKEYFILE )
			{
				char szCurrentDirectory[2048] = {0};
				char szFile[2048] = {0};

				OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof(ofn));

				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = "All files\0*.*\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				GetCurrentDirectory(2048,szCurrentDirectory);

				if ( GetOpenFileName(&ofn) ) 
				{
					if ( wParam==IDC_BUTTON_BROWSESSLCERTIFICATEFILE ) {
						SetWindowText(m_hEditSslCertificateFile,ofn.lpstrFile);
					}
					else if ( wParam==IDC_BUTTON_BROWSESSLCERTIFICATEKEYFILE ) {
						SetWindowText(m_hEditSslCertificateKeyFile,ofn.lpstrFile);
					}
				}

				// restore current directory so the open file prompt
				// doesn't change path/application path where files are saved
				SetCurrentDirectory(szCurrentDirectory);
			}
		}
		break;
	}

	return 0;
}

void SslPage::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hButtonBrowseSslCertificateFile = GetDlgItem(hWnd,IDC_BUTTON_BROWSESSLCERTIFICATEFILE);
	m_hButtonBrowseSslCertificateKeyFile = GetDlgItem(hWnd,IDC_BUTTON_BROWSESSLCERTIFICATEKEYFILE);
	m_hCheckSslEnabled = GetDlgItem(hWnd,IDC_CHECK_SSLENABLED);
	m_hEditSslCertificateFile = GetDlgItem(hWnd,IDC_EDIT_SSLCERTIFICATEFILE);
	m_hEditSslCertificateKeyFile = GetDlgItem(hWnd,IDC_EDIT_SSLCERTIFICATEKEYFILE);
	m_hEditSslCertificateKeyPassword = GetDlgItem(hWnd,IDC_EDIT_SSLCERTIFICATEKEYPASSWORD);

	SetWindowPos(hWnd,HWND_TOP,158,0,-1,-1,SWP_NOSIZE);
}

void SslPage::updateDialog()
{
	EnableWindow(m_hEditSslCertificateFile,Button_GetCheck(m_hCheckSslEnabled));
	EnableWindow(m_hEditSslCertificateKeyFile,Button_GetCheck(m_hCheckSslEnabled));
	EnableWindow(m_hEditSslCertificateKeyPassword,Button_GetCheck(m_hCheckSslEnabled));
	EnableWindow(m_hButtonBrowseSslCertificateFile,Button_GetCheck(m_hCheckSslEnabled));
	EnableWindow(m_hButtonBrowseSslCertificateKeyFile,Button_GetCheck(m_hCheckSslEnabled));
}

void SslPage::loadPage()
{
	Button_SetCheck(m_hCheckSslEnabled,
		ConfigManager::getInstance()->getBool(ConfigManager::HTTPSERVER_CONNECTOR_SSLENABLED));

	SetWindowText(m_hEditSslCertificateFile,
		ConfigManager::getInstance()->getString(ConfigManager::HTTPSERVER_CONNECTOR_SSLCERTIFICATEFILE).c_str());

	SetWindowText(m_hEditSslCertificateKeyFile,
		ConfigManager::getInstance()->getString(ConfigManager::HTTPSERVER_CONNECTOR_SSLCERTIFICATEKEYFILE).c_str());

	SetWindowText(m_hEditSslCertificateKeyPassword,
		ConfigManager::getInstance()->getString(ConfigManager::HTTPSERVER_CONNECTOR_SSLCERTIFICATEKEYPASSWORD).c_str());

	updateDialog();
}
	
void SslPage::savePage()
{
	char sz[255];

	ConfigManager::getInstance()->setBool(ConfigManager::HTTPSERVER_CONNECTOR_SSLENABLED,
		Button_GetCheck(m_hCheckSslEnabled));

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditSslCertificateFile,sz,sizeof(sz));
	ConfigManager::getInstance()->setString(ConfigManager::HTTPSERVER_CONNECTOR_SSLCERTIFICATEFILE,std::string(sz));

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditSslCertificateKeyFile,sz,sizeof(sz));
	ConfigManager::getInstance()->setString(ConfigManager::HTTPSERVER_CONNECTOR_SSLCERTIFICATEKEYFILE,std::string(sz));

	memset(sz,0,sizeof(sz));
	GetWindowText(m_hEditSslCertificateKeyPassword,sz,sizeof(sz));
	ConfigManager::getInstance()->setString(ConfigManager::HTTPSERVER_CONNECTOR_SSLCERTIFICATEKEYPASSWORD,std::string(sz));
}
