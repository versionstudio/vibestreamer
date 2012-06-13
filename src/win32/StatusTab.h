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

#ifndef guard_statustab_h
#define guard_statustab_h

#include "../server/logmanager.h"
#include "../server/httpserver.h"
#include "../server/usermanager.h"

#include "dialogwindow.h"

class StatusTab : public DialogWindow,
				  public HttpSessionManagerListener,
				  public UserManagerListener,
				  public LogManagerListener
{
public:
	StatusTab(HWND hStatusBar) : m_logEntries(0) 
	{
		HttpServer::getInstance()->getSessionManager().addListener(this);
		UserManager::getInstance()->addListener(this);
		LogManager::getInstance()->addListener(this);

		m_hStatusBar = hStatusBar;
	}

	~StatusTab() {
		HttpServer::getInstance()->getSessionManager().removeListener(this);
		UserManager::getInstance()->removeListener(this);
		LogManager::getInstance()->removeListener(this);
	}

	virtual INT CALLBACK dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual void dialogInit(HWND hWnd,LPARAM lParam);

	virtual void on(HttpSessionManagerListener::SessionAdded,HttpSession::Ptr sessionPtr);

	virtual void on(HttpSessionManagerListener::SessionRemoved,
		HttpSession::Ptr sessionPtr,HttpSessionManagerListener::SessionRemovedEventArgs e);

	virtual void on(UserManagerListener::GroupAdded,const Group &group) {

	}

	virtual void on(UserManagerListener::GroupRemoved,const Group &group) {

	}

	virtual void on(UserManagerListener::GroupUpdated,const Group &group) {

	}

	virtual void on(UserManagerListener::UserAdded,const User &user) {

	}

	virtual void on(UserManagerListener::UserRemoved,const User &user) {

	}

	virtual void on(UserManagerListener::UserUpdated,const User &user);

	virtual void on(LogManagerListener::Log,const LogEntry &logEntry);

	void startServer();
	void stopServer();

private:
	HWND m_hStatusBar;

	HWND m_hButtonStartStop;
	HWND m_hListSessions;
	HWND m_hRichEditServerLog;

	int m_logEntries;
};

#endif
