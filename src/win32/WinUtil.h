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

#ifndef guard_winutil_h
#define guard_winutil_h

namespace WinUtil
{
	class ConvertUtil
	{
	public:
		static void toBstr(const std::string &s,BSTR *target);
		static std::string toString(const BSTR &bstr);
	};

	class DebugUtil
	{
	public:
		static void outputDebugString(const char *format,...);
	};

	class DpiUtil
	{
	public:
		static int getDpiX() { init(); return m_dpiX; }
		static int getDpiY() { init(); return m_dpiY; }

		static int scaleX(int x) { init(); return MulDiv(x,m_dpiX,96); }
		static int scaleY(int y) { init(); return MulDiv(y,m_dpiY,96); }

		static int unscaleX(int x) { init(); return MulDiv(x,96,m_dpiX); } 
		static int unscaleY(int y) { init(); return MulDiv(y,96,m_dpiY); } 

	private:
		static void init();

		static int m_dpiX;
		static int m_dpiY;

		static bool m_initialized;
	};

	class FormatUtil
	{
	public:
		static std::string formatFileSize(const int64_t bytes);
	};

	class MenuUtil
	{
	public:
		static void showPopupMenu(HWND hWnd,HMENU hPopupMenu);
	};

	class ListViewUtil
	{
	public:
		static int findItemByText(HWND hWnd,std::string text);
		static int findItemByParam(HWND hWnd,LPARAM lParam);

		static void insertColumn(HWND hWnd,int index,std::string text,int width,bool alignRight);
		static int insertItem(HWND hWnd,int index,std::string text,LPARAM lParam,int imageIndex=-1,int imageIndent=-1);
		static void insertSubItem(HWND hWnd,int index,int subIndex,std::string text);

		static void selectItem(HWND hWnd,int index);

		static int getItemImageIndent(HWND hWnd,int index);
		static int getItemImageIndex(HWND hWnd,int index);
		static std::string getItemText(HWND hWnd,int index,int subIndex);
		static LPARAM getItemParam(HWND hWnd,int index);

		static void setItemImageIndex(HWND hWnd,int index,int imageIndex);
	};

	class ResourceUtil
	{
	public:
		static std::string loadString(UINT uID);
	};

	class TabControlUtil
	{
	public:
		static void insertItem(HWND hWnd,int index,std::string text,LPARAM lParam);
		static LPARAM getItemParam(HWND hWnd,int index);
	};

	class TreeViewUtil
	{
	public:
		static HTREEITEM insertItem(HWND hWnd,HTREEITEM hParentItem,std::string text,LPARAM lParam);
		static LPARAM getItemParam(HWND hWnd,HTREEITEM hItem);
	};

	class WindowUtil
	{
	public:
		static void center(HWND hWnd);
		static void centerParent(HWND hWnd);

		static std::string getWindowText(HWND hWnd);
	};
};

#endif
