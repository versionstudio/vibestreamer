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
#include "aboutdialog.h"

#include "hyperlinks.h"

INT CALLBACK AboutDialog::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{		
		case WM_COMMAND:
		{
			switch ( wParam )
			{
				case IDC_BUTTON_CLOSE:
				{
					EndDialog(hWnd, NULL);
				}
				break;

				case IDC_STATIC_URL:
				{
					char sz[255] = {0};
					GetWindowText(GetDlgItem(hWnd,IDC_STATIC_URL),sz,255);
					ShellExecute(hWnd,"open",sz,NULL,NULL,SW_SHOWNORMAL);
				}
				break;
			}
		}
		break;
	}

	return 0;
}

void AboutDialog::dialogInit(HWND hWnd,LPARAM lParam)
{
	SetWindowText(GetDlgItem(hWnd,IDC_STATIC_APPLICATION), TEXT(APP_NAME) TEXT(" ") TEXT(APP_VERSION) TEXT(APP_MILESTONE));
	ConvertStaticToHyperlink(GetDlgItem(hWnd,IDC_STATIC_URL));
	WinUtil::WindowUtil::centerParent(hWnd);
	SetFocus(GetDlgItem(hWnd, IDC_BUTTON_CLOSE));
}
