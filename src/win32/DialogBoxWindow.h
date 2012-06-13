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

#ifndef guard_dialogboxwindow_h
#define guard_dialogboxwindow_h

class DialogBoxWindow
{
public:
	DialogBoxWindow() : m_hWnd(NULL) {
		
	}

	static INT CALLBACK dialogProcHandler(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

	INT_PTR init(HWND owner,LPCTSTR lpTemplateName,LPARAM lParam);

	virtual INT CALLBACK dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam) {
		return 0;
	}

	virtual void dialogInit(HWND hWnd,LPARAM lParam) {

	}

	virtual void dialogRefresh() {

	}

	HWND getHwnd() {
		return m_hWnd;
	}

	void setHwnd(HWND hWnd) {
		m_hWnd = hWnd;
	}

private:
	HWND m_hWnd;
};

#endif
