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
#include "winutil.h"

#include <comdef.h>

int WinUtil::DpiUtil::m_dpiX = 96;
int WinUtil::DpiUtil::m_dpiY = 96;
bool WinUtil::DpiUtil::m_initialized = false;

void WinUtil::ConvertUtil::toBstr(const std::string &s,BSTR *target)
{	
	_bstr_t str(s.c_str());
	*target = str.copy();
}

std::string WinUtil::ConvertUtil::toString(const BSTR &bstr)
{
	_bstr_t aux(bstr,true);
	return std::string(aux);
}

void WinUtil::DebugUtil::outputDebugString(const char *format,...)
{
	char formatted[2048];

	va_list args;

	ZeroMemory(&formatted,sizeof(formatted));
	va_start(args,format);
	_vsnprintf(formatted,sizeof(formatted),format,args);
	va_end(args);

	OutputDebugString(formatted);
}

std::string WinUtil::FormatUtil::formatFileSize(const int64_t bytes)
{
	char fileSize[255] = {0};

	if ( bytes>pow(1024,3) ) {
		sprintf(fileSize,"%.2f GB",bytes/pow(1024,3));
	}
	else if ( bytes>pow(1024,2) ) {
		sprintf(fileSize,"%.2f MB",bytes/pow(1024,2));
	}
	else if ( bytes>1024 ) {		
		sprintf(fileSize,"%.2f KB",bytes/1024);
	}
	else {
		sprintf(fileSize,"%.2f B",bytes);
	}

	return std::string(fileSize);
}

void WinUtil::DpiUtil::init()
{
	if ( !m_initialized ) 
	{ 
		HDC hdc = GetDC(NULL); 
		if ( hdc ) {
			m_dpiX = GetDeviceCaps(hdc,LOGPIXELSX);
			m_dpiY = GetDeviceCaps(hdc,LOGPIXELSY);
			ReleaseDC(NULL,hdc);
		}
		
		m_initialized = true;
	} 
}

void WinUtil::MenuUtil::showPopupMenu(HWND hWnd,HMENU hPopupMenu)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);

	SetForegroundWindow(hWnd);
	TrackPopupMenu(hPopupMenu,NULL,ptMouse.x,ptMouse.y,0,hWnd,NULL);
	SendMessage(hWnd,WM_NULL,0,0);
}

int WinUtil::ListViewUtil::findItemByText(HWND hWnd,std::string text)
{
	LVFINDINFO findInfo = {0};

	findInfo.flags = LVFI_STRING;
	findInfo.psz = (LPSTR)text.c_str();

	return ListView_FindItem(hWnd,-1,&findInfo);
}

int WinUtil::ListViewUtil::findItemByParam(HWND hWnd,LPARAM lParam)
{
	LVFINDINFO findInfo = {0};
	findInfo.flags = LVFI_PARAM;
	findInfo.lParam = lParam;

	return ListView_FindItem(hWnd,-1,&findInfo);
}

void WinUtil::ListViewUtil::insertColumn(HWND hWnd,int index,std::string text,int width,bool alignRight)
{
	LVCOLUMN column = {0};
	column.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_FMT;

	if ( alignRight ) {
		 column.fmt=LVCFMT_RIGHT;
	}
	else {
		column.fmt=LVCFMT_LEFT;	
	}

	column.cx=width;
	column.pszText=(LPSTR)text.c_str();

	ListView_InsertColumn(hWnd,index,&column);
}

int WinUtil::ListViewUtil::insertItem(HWND hWnd,int index,std::string text,LPARAM lParam,
		int imageIndex,int imageIndent)
{
	LVITEM item = {0};

	item.mask=LVIF_TEXT | LVIF_PARAM;
	item.lParam = lParam;
	item.cchTextMax = 255;
	item.iItem=index;
	item.iImage=imageIndex;
	item.iIndent=imageIndent;
	item.pszText = (LPSTR)text.c_str();

	if ( imageIndex>-1 ) {
		item.mask |= LVIF_IMAGE;
	}

	if ( imageIndent>-1 ) {
		item.mask |= LVIF_INDENT;
	}

	return ListView_InsertItem(hWnd,&item);
}

void WinUtil::ListViewUtil::insertSubItem(HWND hWnd,int index,int subIndex,std::string text)
{
	LVITEM item = {0};

	item.mask = LVIF_TEXT;
	item.cchTextMax = 255;
	item.iItem = index;
	item.iSubItem = subIndex;
	item.pszText = (LPSTR)text.c_str();

	ListView_SetItem(hWnd,&item);
}

void WinUtil::ListViewUtil::selectItem(HWND hWnd,int index)
{
	ListView_SetSelectionMark(hWnd,index);
	ListView_SetItemState(hWnd,index,LVIS_SELECTED,LVIS_SELECTED);
	ListView_EnsureVisible(hWnd,index,false);
}

int WinUtil::ListViewUtil::getItemImageIndent(HWND hWnd,int index)
{
	LVITEM item = {0};
	item.mask = LVIF_INDENT;
	item.iItem = index;

	if ( ListView_GetItem(hWnd,&item) ) {
		return item.iIndent;
	}
	else {
		return -1;
	}
}

int WinUtil::ListViewUtil::getItemImageIndex(HWND hWnd,int index)
{
	LVITEM item = {0};

	item.mask = LVIF_IMAGE;
	item.iItem = index;

	if ( ListView_GetItem(hWnd,&item) ) {
		return item.iImage;
	}
	else {
		return -1;
	}
}

std::string WinUtil::ListViewUtil::getItemText(HWND hWnd,int index,int subIndex)
{
	LVITEM item = {0};

	char text[256];

	item.cchTextMax = 255;
	item.iItem = 0;
	item.iSubItem = subIndex;
	item.pszText = text;

	SendMessage(hWnd,LVM_GETITEMTEXT,index,(LPARAM)&item);

	return text;
}

LPARAM WinUtil::ListViewUtil::getItemParam(HWND hWnd,int index)
{
	LVITEM item = {0};
	item.mask = LVIF_PARAM;
	item.iItem = index;

	if ( ListView_GetItem(hWnd,&item) ) {
		return item.lParam;
	}
	else {
		return NULL;
	}
}

void WinUtil::ListViewUtil::setItemImageIndex(HWND hWnd,int index,int imageIndex)
{
	LVITEM item = {0};
	item.mask = LVIF_IMAGE;
	item.iItem = index;
	item.iImage = imageIndex,

	ListView_SetItem(hWnd,&item);
}

std::string WinUtil::ResourceUtil::loadString(UINT uID)
{
	std::string s;

	TCHAR szBuf[1024];
	if ( LoadString(NULL,uID,szBuf,sizeof(szBuf)/sizeof(TCHAR)) ) {
		s = szBuf;
	}

	return s;
}

void WinUtil::TabControlUtil::insertItem(HWND hWnd,int index,std::string text,LPARAM lParam)
{
	TCITEM item = {0};
	item.mask = TCIF_TEXT | TCIF_PARAM;
	item.pszText = (LPSTR)text.c_str();
	item.lParam = lParam;

	TabCtrl_InsertItem(hWnd,index,&item);
}

LPARAM WinUtil::TabControlUtil::getItemParam(HWND hWnd,int index)
{
	TCITEM item = {0};
	item.mask = TCIF_PARAM;

	if ( TabCtrl_GetItem(hWnd,index,&item) ) {
		return item.lParam;
	}
	else {
		return NULL;
	}
}

HTREEITEM WinUtil::TreeViewUtil::insertItem(HWND hWnd,HTREEITEM hParentItem,std::string text,LPARAM lParam)
{
	TV_INSERTSTRUCT insertStruct = {0};

	if ( hParentItem==NULL ) {
		hParentItem = TVI_ROOT;	
	}

	insertStruct.hParent=hParentItem;
	insertStruct.item.mask=TVIF_TEXT | LVIF_PARAM;
	insertStruct.item.lParam = lParam;
	insertStruct.item.pszText=(LPSTR)text.c_str();

	return TreeView_InsertItem(hWnd,&insertStruct);
}

LPARAM WinUtil::TreeViewUtil::getItemParam(HWND hWnd,HTREEITEM hItem)
{
	TVITEM item = {0};
	item.mask = LVIF_PARAM;
	item.hItem = hItem;

	if ( TreeView_GetItem(hWnd,&item) ) {
		return item.lParam;
	}
	else {
		return NULL;
	}

}

void WinUtil::WindowUtil::center(HWND hWnd)
{
	HWND hWndDesktop = GetDesktopWindow();

	RECT desktopRect;
	RECT windowRect;

	GetWindowRect(hWndDesktop,&desktopRect);
	GetWindowRect(hWnd,&windowRect);

	SetWindowPos(hWnd,HWND_TOP,(desktopRect.right/2)-((windowRect.right-windowRect.left)/2),
				 (desktopRect.bottom/2)-((windowRect.bottom-windowRect.top)/2),0,0,1);
}

void WinUtil::WindowUtil::centerParent(HWND hWnd)
{
	HWND hWndParent = GetParent(hWnd);
	RECT rcParent,rcDialog,rc;

	GetWindowRect(hWndParent,&rcParent); 
	GetWindowRect(hWnd,&rcDialog);
	CopyRect(&rc,&rcParent);

	OffsetRect(&rcDialog,-rcDialog.left,-rcDialog.top); 
	OffsetRect(&rc,-rc.left,-rc.top); 
	OffsetRect(&rc,-rcDialog.right,-rcDialog.bottom);

	SetWindowPos(hWnd,HWND_TOP,rcParent.left+(rc.right/2),rcParent.top+(rc.bottom/2),0,0,SWP_NOSIZE); 
}

std::string WinUtil::WindowUtil::getWindowText(HWND hWnd)
{
	char text[256];

	GetWindowText(hWnd,text,255);

	return text;
}
