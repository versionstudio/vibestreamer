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
#include "dialogboxwindow.h"

INT CALLBACK DialogBoxWindow::dialogProcHandler(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_INITDIALOG:
		{
			std::pair<DialogBoxWindow*,LPARAM> *pInitParameter = (std::pair<DialogBoxWindow*,LPARAM>*)lParam;

			HANDLE hPrivate = (HANDLE)pInitParameter->first;
			SetProp(hWnd,"private",hPrivate);
			
			DialogBoxWindow *pDialogBoxWindow = (DialogBoxWindow*)hPrivate;
			pDialogBoxWindow->setHwnd(hWnd);
			pDialogBoxWindow->dialogInit(hWnd,LPARAM(pInitParameter->second));
		}
		break;

		case WM_DESTROY:
		{
			RemoveProp(hWnd,"private");
		}
		break;
	}

	HANDLE hPrivate = GetProp(hWnd,"private");
	if ( hPrivate!=NULL ) {
		return ((DialogBoxWindow*)hPrivate)->dialogProc(hWnd,msg,wParam,lParam);
	}

	return 0;
}

INT_PTR DialogBoxWindow::init(HWND owner,LPCTSTR lpTemplateName,LPARAM lParam)
{
	std::pair<DialogBoxWindow*,LPARAM> initParameter = std::make_pair(this,lParam);

	return DialogBoxParam(GetModuleHandle(NULL),lpTemplateName,owner,
		DialogBoxWindow::dialogProcHandler,(LPARAM)&initParameter);
}
