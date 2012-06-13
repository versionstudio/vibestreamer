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
#include "cleanupdialog.h"

#include "../server/core.h"

void CleanupDialog::cleanupCallback(void *arg,const std::string &s)
{
	SetWindowText((HWND)arg,s.c_str());
}

void CleanupDialog::dialogInit(HWND hWnd,LPARAM lParam)
{
	SetWindowText(hWnd,WinUtil::ResourceUtil::loadString(IDS_CLOSING).c_str());
	WinUtil::WindowUtil::center(hWnd);
	ShowWindow(hWnd,SW_SHOW);

	Core::cleanup(CleanupDialog::cleanupCallback,GetDlgItem(hWnd,IDC_STATIC_MESSAGE));

	EndDialog(hWnd,NULL);
}
