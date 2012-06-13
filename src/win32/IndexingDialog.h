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

#ifndef guard_indexingdialog_h
#define guard_indexingdialog_h

#include "../server/indexer.h"

#include "dialogwindow.h"

class IndexingDialog : public DialogWindow,
					   public IndexerListener
{
public:
	IndexingDialog() {
		Indexer::getInstance()->addListener(this);
	}

	~IndexingDialog() {
		Indexer::getInstance()->removeListener(this);
	}

	virtual INT CALLBACK dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual void dialogInit(HWND hWnd,LPARAM lParam);

	virtual void on(IndexerListener::JobCompleted);
	virtual void on(IndexerListener::JobQueued,const IndexerJob &job) {

	}

	virtual void on(IndexerListener::JobStarted);

private:
	void updateDialog();

	HWND m_hButtonAbort;
	HWND m_hProgress;
	HWND m_hStaticValueAction;
	HWND m_hStaticValueMessage;
	HWND m_hStaticValuePath;
	HWND m_hStaticValueTimeElapsed;
};

#endif
