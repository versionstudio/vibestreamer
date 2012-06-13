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
#include "statustab.h"

#define LOGGER_CLASSNAME "StatusTab"

#include "../server/version.h"

#include "upnpmanager.h"

#include <richedit.h>

INT CALLBACK StatusTab::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case UWM_INSERT_LOGENTRY:
		{
			std::string *pFormattedTime = (std::string*)wParam;
			std::string *pMessage = (std::string*)lParam;
			std::string messageLine = *pFormattedTime + " " + *pMessage;
			if ( m_logEntries!=0 ) {
				messageLine = "\r\n" + messageLine;
			}

			// append message to rich edit
			SendMessage(m_hRichEditServerLog,EM_SETSEL,-1,-1);
			SendMessage(m_hRichEditServerLog,EM_REPLACESEL,0,(LPARAM)messageLine.c_str());
			SendMessage(m_hRichEditServerLog,WM_VSCROLL,SB_BOTTOM,0);

			if ( ++m_logEntries>512 )
			{
				// limit visible log entries
				LRESULT lineIndex = SendMessage(m_hRichEditServerLog,EM_LINEINDEX,1,0);
				if ( lineIndex>-1 ) {
					SendMessage(m_hRichEditServerLog,EM_SETSEL,0,lineIndex);
					SendMessage(m_hRichEditServerLog,EM_REPLACESEL,0,NULL);
				}
			}

			delete pFormattedTime;
			delete pMessage;
		}
		break;

		case UWM_SESSIONS_UPDATED:
		{
			std::vector<HttpSession::Ptr> sessions = HttpServer::getInstance()->getSessionManager().getSessions();
			std::vector<int> removeIndexes;

			int count = ListView_GetItemCount(m_hListSessions);
			for ( int i=0; i<count; i++ )
			{
				bool foundSession = false;
				std::string *pGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListSessions,i);

				std::vector<HttpSession::Ptr>::iterator iter;
				for ( iter=sessions.begin(); iter!=sessions.end(); iter++ )	
				{
					if ( (*iter)->getGuid()==*pGuid )
					{
						foundSession = true;

						// check if username has changed
						std::string presentationName = (*iter)->getPresentationName();
						if ( WinUtil::ListViewUtil::getItemText(m_hListSessions,i,0)!=presentationName ) {
							ListView_SetItemText(m_hListSessions,i,0,(LPSTR)presentationName.c_str());
						}
					}
				}

				if ( !foundSession ) {
					removeIndexes.push_back(i);
				}
			}

			// loop sessions that should be removed from list
			std::vector<int>::iterator iterIndex;
			for ( iterIndex=removeIndexes.begin(); iterIndex!=removeIndexes.end(); iterIndex++ ) {
				std::string *pGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListSessions,*iterIndex);
				ListView_DeleteItem(m_hListSessions,*iterIndex);
				delete pGuid;
			}

			// loop connected sessions to see if any sessions should be added
			std::vector<HttpSession::Ptr>::iterator iter;
			for ( iter=sessions.begin(); iter!=sessions.end(); iter++ )
			{
				bool foundSession = false;

				int count = ListView_GetItemCount(m_hListSessions);
				for ( int i=0; i<count; i++ ) {
					std::string *pGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListSessions,i);
					if ( *pGuid==(*iter)->getGuid() ) {
						foundSession = true;
						break;
					}
				}

				if ( !foundSession ) 
				{
					std::string presentationName = (*iter)->getPresentationName();

					tm localTime;
					Util::TimeUtil::getLocalTime((*iter)->getCreationTime(),&localTime);
					std::string connected = Util::TimeUtil::format(localTime,"%Y-%m-%d %H:%M");

					WinUtil::ListViewUtil::insertItem(m_hListSessions,0,presentationName,(LPARAM)new std::string((*iter)->getGuid()));
					WinUtil::ListViewUtil::insertSubItem(m_hListSessions,0,1,(*iter)->getRemoteAddress().c_str());
					WinUtil::ListViewUtil::insertSubItem(m_hListSessions,0,2,connected.c_str());
				}
			}
		}
		break;

		case WM_COMMAND:
		{
			switch ( wParam )
			{
				case IDC_BUTTON_STARTSTOP:
				{
					if ( !HttpServer::getInstance()->isStarted() ) {
						startServer();
					}
					else {
						stopServer();
					}
				}
				break;

				case UID_CLEAR:
				{
					SetWindowText(m_hRichEditServerLog,NULL);
					m_logEntries = 0;
				}
				break;

				case UID_KICKSESSION:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListSessions);
					if ( selectedIndex!=-1 ) {
						std::string *pGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListSessions,selectedIndex);
						HttpSession::Ptr sessionPtr = HttpServer::getInstance()->getSessionManager().findSessionByGuid(*pGuid);
						if ( sessionPtr!=NULL ) {
							HttpServer::getInstance()->getSessionManager().kickSession(sessionPtr);
						}
					}
				}
				break;
			}
		}
		break;

		case WM_NOTIFY:
		{
			LPNMHDR pNmh = (LPNMHDR)lParam;
			switch ( pNmh->code )
			{
				case LVN_DELETEITEM:
				case LVN_INSERTITEM:
				case NM_CUSTOMDRAW:
				{
					if ( pNmh->hwndFrom==m_hListSessions ) {
						SendMessage(m_hListSessions,LVM_SETCOLUMNWIDTH,2,LVSCW_AUTOSIZE_USEHEADER);
					}
				}
				break;

				case EN_MSGFILTER:
				{
					const MSGFILTER *pFilter = (MSGFILTER*)lParam;
					if ( pFilter->nmhdr.hwndFrom==m_hRichEditServerLog
						&& pFilter->msg==WM_RBUTTONDOWN )
					{
						HMENU hPopupMenu = CreatePopupMenu();
						if ( hPopupMenu!=NULL )
						{
							if ( Edit_GetLineCount(m_hRichEditServerLog)>0 ) {								
								AppendMenu(hPopupMenu,MF_STRING,UID_CLEAR,WinUtil::ResourceUtil::loadString(IDS_CLEAR).c_str());
							}
							else {
								AppendMenu(hPopupMenu,MF_STRING|MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_CLEAR).c_str());
							}

							WinUtil::MenuUtil::showPopupMenu(hWnd,hPopupMenu);
						}
					}
				}
				break;

				case NM_RCLICK:
				{
					if ( pNmh->hwndFrom==m_hListSessions )
					{
						HMENU hPopupMenu = CreatePopupMenu();
						if ( hPopupMenu!=NULL )
						{
							LPNMITEMACTIVATE pNmItem = (LPNMITEMACTIVATE)lParam;
							if ( pNmItem->iItem!=-1 ) {
								AppendMenu(hPopupMenu,MF_STRING,UID_KICKSESSION,WinUtil::ResourceUtil::loadString(IDS_KICKSESSION).c_str());
							}
							else {
								AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,UID_KICKSESSION,WinUtil::ResourceUtil::loadString(IDS_KICKSESSION).c_str());
							}

							WinUtil::MenuUtil::showPopupMenu(hWnd,hPopupMenu);
						}
					}
				}
				break;
			}
		}
		break;
	}

	return 0;
}

void StatusTab::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hButtonStartStop = GetDlgItem(hWnd,IDC_BUTTON_STARTSTOP);
	m_hRichEditServerLog = GetDlgItem(hWnd,IDC_EDIT_SERVERLOG);
	m_hListSessions = GetDlgItem(hWnd,IDC_LIST_SESSIONS);

	SendMessage(m_hRichEditServerLog,EM_SETEVENTMASK,0,ENM_MOUSEEVENTS);
	SendMessage(m_hListSessions,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT);
	WinUtil::ListViewUtil::insertColumn(m_hListSessions,0,WinUtil::ResourceUtil::loadString(IDS_NAME).c_str(),WinUtil::DpiUtil::scaleX(248),false);
	WinUtil::ListViewUtil::insertColumn(m_hListSessions,1,WinUtil::ResourceUtil::loadString(IDS_IPADDRESS).c_str(),WinUtil::DpiUtil::scaleX(100),false);
	WinUtil::ListViewUtil::insertColumn(m_hListSessions,2,WinUtil::ResourceUtil::loadString(IDS_CONNECTED).c_str(),0,false);

	SetWindowText(m_hButtonStartStop,WinUtil::ResourceUtil::loadString(IDS_START).c_str());
	SetWindowPos(hWnd,HWND_TOP,WinUtil::DpiUtil::scaleX(15),WinUtil::DpiUtil::scaleY(30),-1,-1,SWP_NOSIZE);
}

void StatusTab::startServer()
{
	EnableWindow(m_hButtonStartStop,false);
	if ( HttpServer::getInstance()->start() ) 
	{
		SetWindowText(m_hButtonStartStop,WinUtil::ResourceUtil::loadString(IDS_STOP).c_str());
		if ( SettingsManager::getInstance()->getBool("upnpEnabled") )
		{
			std::string ipAddress = ConfigManager::getInstance()->getString(ConfigManager::HTTPSERVER_CONNECTOR_SERVERADDRESS);
			int port = ConfigManager::getInstance()->getInt(ConfigManager::HTTPSERVER_CONNECTOR_SERVERPORT);
			if ( UpnpManager::getInstance()->addMapping(UpnpMapping("TCP",APP_NAME,ipAddress,port)) ) 
			{
				LogManager::getInstance()->info(LOGGER_CLASSNAME,"Configured UPnP port mapping for %d",port);
				if ( !UpnpManager::getInstance()->getExternalIpAddress().empty() ) {
					LogManager::getInstance()->info(LOGGER_CLASSNAME,"External IP address: %s",UpnpManager::getInstance()->getExternalIpAddress().c_str());
				}
			}
			else
			{
				LogManager::getInstance()->info(LOGGER_CLASSNAME,"Failed to create UPnP port mapping for %d",port);
			}
		}
	}

	EnableWindow(m_hButtonStartStop,true);
}

void StatusTab::stopServer()
{
	EnableWindow(m_hButtonStartStop,false);
	if ( HttpServer::getInstance()->stop() ) 
	{
		SetWindowText(m_hButtonStartStop,WinUtil::ResourceUtil::loadString(IDS_START).c_str());
		if ( UpnpManager::getInstance()->hasMappings() ) 
		{
			if ( UpnpManager::getInstance()->clearMappings() ) {
				LogManager::getInstance()->info(LOGGER_CLASSNAME,"Removed port mapping");
			}
			else {
				LogManager::getInstance()->info(LOGGER_CLASSNAME,"Failed to remove port mapping");
			}
		}
	}

	EnableWindow(m_hButtonStartStop,true);
}

void StatusTab::on(HttpSessionManagerListener::SessionAdded,HttpSession::Ptr sessionPtr)
{
	PostMessage(this->getHwnd(),UWM_SESSIONS_UPDATED,NULL,NULL);
}

void StatusTab::on(HttpSessionManagerListener::SessionRemoved,HttpSession::Ptr sessionPtr,
	HttpSessionManagerListener::SessionRemovedEventArgs e)
{
	PostMessage(this->getHwnd(),UWM_SESSIONS_UPDATED,NULL,NULL);
}

void StatusTab::on(UserManagerListener::UserUpdated,const User &user)
{
	PostMessage(this->getHwnd(),UWM_SESSIONS_UPDATED,NULL,NULL);
}

void StatusTab::on(LogManagerListener::Log,const LogEntry &logEntry)
{
	tm localTime;
	Util::TimeUtil::getLocalTime(logEntry.getCreationTime(),&localTime);
	std::string formattedTimeStamp = Util::TimeUtil::format(localTime,"%Y-%m-%d %H:%M:%S");

	if ( logEntry.getLevel()==LogManager::LogLevel::WARNING ) {
		PostMessage(this->getHwnd(),UWM_INSERT_LOGENTRY,(WPARAM)new std::string(formattedTimeStamp),
			(LPARAM)new std::string("An error occured. Please check the server log file for details."));
	}
	else if ( logEntry.getLevel()==LogManager::LogLevel::INFO ) 
	{
		PostMessage(this->getHwnd(),UWM_INSERT_LOGENTRY,(WPARAM)new std::string(formattedTimeStamp),
			(LPARAM)new std::string(logEntry.getMessage()));
	}
}
