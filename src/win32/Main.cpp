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

#include "mainwindow.h"

#include "../server/version.h"

MainWindow mainWindow;

LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return mainWindow.wndProc(hWnd,msg,wParam,lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwICC = ICC_LISTVIEW_CLASSES;
    InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);

	if ( !InitCommonControlsEx(&InitCtrls) ) {
        MessageBox(NULL,"Failed to load common controls!","Error!",MB_ICONEXCLAMATION | MB_OK);
        return(0);
	}

	if ( CoInitialize(NULL)!=S_OK ) {
		MessageBox(NULL,"Failed to load com library!","Error!",MB_ICONEXCLAMATION | MB_OK);
		return(0);
	}

	if ( LoadLibrary("riched32.dll")==NULL ) {
		MessageBox(NULL,"Failed to load rich edit control!","Error!",MB_ICONEXCLAMATION | MB_OK);
		return(0);
	}

    WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = wndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_VIBE));
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "MainWindow";
    wc.hIconSm       = NULL;

	if (!RegisterClassEx(&wc)) {
        MessageBox(NULL,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION | MB_OK);
        return(0);
    }

	HWND hWnd = CreateWindow(wc.lpszClassName,TEXT(APP_NAME),WS_OVERLAPPED | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT,CW_USEDEFAULT,WinUtil::DpiUtil::scaleX(566),WinUtil::DpiUtil::scaleY(478),NULL,NULL,hInstance,NULL);
	
	if (hWnd == NULL ) {
        MessageBox(NULL,"Window Creation Failed!","Error!",MB_ICONEXCLAMATION | MB_OK);
        return(0);
    }

    MSG msg;
	while( true )
    {        
		if ( PeekMessage(&msg,NULL,0,0,PM_NOREMOVE) )
        {
            if (!GetMessage(&msg, NULL, 0, 0)) {
                return (int)msg.wParam;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		else {
			Sleep(1);
		}
    }

	CoUninitialize(); // uninitialize com library
}
