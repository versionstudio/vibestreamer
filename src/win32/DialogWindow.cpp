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
#include "dialogwindow.h"

INT CALLBACK DialogWindow::dialogProcHandler(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case WM_INITDIALOG:
		{
			std::pair<DialogWindow*,LPARAM> *pInitParameter = (std::pair<DialogWindow*,LPARAM>*)lParam;
			
			HANDLE hPrivate = (HANDLE)pInitParameter->first;
			SetProp(hWnd,"private",hPrivate);
			
			DialogWindow *pDialogWindow = (DialogWindow*)hPrivate;
			pDialogWindow->setHwnd(hWnd);
			pDialogWindow->dialogInit(hWnd,LPARAM(pInitParameter->second));
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
		return ((DialogWindow*)hPrivate)->dialogProc(hWnd,msg,wParam,lParam);
	}

	return 0;
}

int DialogWindow::init(HWND owner,LPCTSTR lpTemplateName,LPARAM lParam)
{
	HRSRC hResource = FindResource(GetModuleHandle(NULL),lpTemplateName,RT_DIALOG);
	if ( hResource==NULL ) {
		return 1;
	}

    HGLOBAL hGlobal = LoadResource(GetModuleHandle(NULL),hResource);
	if ( hGlobal==NULL ) {
		return 1;
	}

	LPVOID lpTemplate = LockResource(hGlobal);
	if (lpTemplate==NULL ) {
		return 1;
	}

	std::pair<DialogWindow*,LPARAM> initParameter = std::make_pair(this,lParam);

	if ( CreateDialogIndirectParam(GetModuleHandle(NULL),(LPDLGTEMPLATE)lpTemplate,
		owner,DialogWindow::dialogProcHandler,(LPARAM)&initParameter)==NULL ) {
		return 1;
	}

	resizeDialog();

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	return 0;
}

void DialogWindow::resizeDialog()
{

}
