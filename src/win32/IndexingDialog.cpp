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
#include "indexingdialog.h"

#include "../server/sharemanager.h"

#include <iomanip>

INT CALLBACK IndexingDialog::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{	
		case UWM_INDEX_UPDATED: {
			updateDialog();
		}
		break;

		case WM_TIMER: 
		{
			if ( wParam==IDT_INDEXINGPROGRESS ) {
				updateDialog();
			}
		}
		break;

		case WM_COMMAND:
		{
			switch ( wParam )
			{
				case IDC_BUTTON_ABORT:
				{
					EnableWindow(m_hButtonAbort,false);
					Indexer::getInstance()->abort();
				}
				break;
			}
		}
		break;

		case WM_CLOSE: 
		{
			ShowWindow(hWnd,SW_HIDE);
		}
		break;
	}

	return 0;
}

void IndexingDialog::dialogInit(HWND hWnd,LPARAM lParam)
{	
	m_hButtonAbort = GetDlgItem(hWnd,IDC_BUTTON_ABORT);
	m_hProgress = GetDlgItem(hWnd,IDC_PROGRESS);
	m_hStaticValueAction = GetDlgItem(hWnd,IDC_STATIC_VALUE_ACTION);
	m_hStaticValueMessage = GetDlgItem(hWnd,IDC_STATIC_VALUE_MESSAGE);
	m_hStaticValuePath = GetDlgItem(hWnd,IDC_STATIC_VALUE_PATH);
	m_hStaticValueTimeElapsed = GetDlgItem(hWnd,IDC_STATIC_VALUE_TIMEELAPSED);

	WinUtil::WindowUtil::centerParent(hWnd);

	updateDialog();
}

void IndexingDialog::updateDialog()
{
	std::stringstream message;
	IndexerJob currentJob = Indexer::getInstance()->getCurrentJob();

	switch ( currentJob.getState() )
	{
		case IndexerJob::State::IDLE: 
		{
			SetWindowText(m_hStaticValueAction,WinUtil::ResourceUtil::loadString(IDS_IDLE).c_str());
		}
		break;

		case IndexerJob::State::VALIDATING: 
		{
			SetWindowText(m_hStaticValueAction,WinUtil::ResourceUtil::loadString(IDS_VALIDATING).c_str());
			message << currentJob.getCurrentPath();
		}
		break;

		case IndexerJob::State::ANALYZING: 
		{
			SetWindowText(m_hStaticValueAction,WinUtil::ResourceUtil::loadString(IDS_ANALYZING).c_str());
			message << "Found " << currentJob.getAnalyzedFiles()
					<< " files in " << currentJob.getAnalyzedDirectories() << " directories\r\n" 
					<< currentJob.getCurrentPath();
		}
		break;

		case IndexerJob::State::INDEXING: 
		{
			SetWindowText(m_hStaticValueAction,WinUtil::ResourceUtil::loadString(IDS_INDEXING).c_str());
			message << "Indexed " << currentJob.getIndexedFiles() << " of " << currentJob.getAnalyzedFiles() << " files in "
					<< currentJob.getIndexedDirectories() << " of " << currentJob.getAnalyzedDirectories() << " directories\r\n" 
					<< currentJob.getCurrentPath();
		}
		break;
	}

	uint64_t duration = currentJob.getDuration();

	int hours = 0;
	int minutes = 0;
	int seconds = 0;

	hours = duration/3600;
	duration = duration-hours*3600;

	minutes = duration/60;
	duration = duration-minutes*60;
	seconds = duration;

	std::stringstream timeElapsed;	
	timeElapsed.fill('0');
	timeElapsed << std::setw(2) << hours << ":";
	timeElapsed << std::setw(2) << minutes << ":";
	timeElapsed << std::setw(2) << seconds;

	SetWindowText(m_hStaticValueTimeElapsed,timeElapsed.str().c_str());
	SetWindowText(m_hStaticValueMessage,message.str().c_str());

	int progressBarValue = 0;

	if ( currentJob.getIndexedDirectories()>0 || currentJob.getIndexedFiles()>0 ) 
	{
		uint64_t val1 = currentJob.getIndexedDirectories()+currentJob.getIndexedFiles();
		uint64_t val2 = currentJob.getAnalyzedDirectories()+currentJob.getAnalyzedFiles();
		progressBarValue = (val1*100)/val2;
	}

	SendMessage(m_hProgress,PBM_SETRANGE,0,(LPARAM)MAKELPARAM(0,100));
	SendMessage(m_hProgress,PBM_SETPOS,progressBarValue,0);
}

void IndexingDialog::on(IndexerListener::JobCompleted)
{
	KillTimer(this->getHwnd(),IDT_INDEXINGPROGRESS);
	EnableWindow(m_hButtonAbort,false);
	PostMessage(this->getHwnd(),UWM_INDEX_UPDATED,NULL,NULL);
}

void IndexingDialog::on(IndexerListener::JobStarted)
{
	SetTimer(this->getHwnd(),IDT_INDEXINGPROGRESS,250,NULL);
	EnableWindow(m_hButtonAbort,true);
	PostMessage(this->getHwnd(),UWM_INDEX_UPDATED,NULL,NULL);
}
