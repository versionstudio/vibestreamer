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
#include "sharingtab.h"

#include "../server/sharemanager.h"

#include "sharedialog.h"

INT CALLBACK SharingTab::dialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch ( msg )
	{
		case UWM_INDEX_UPDATED:
		{
			int count = ListView_GetItemCount(m_hListShares);
			for ( int i=0; i<count; i++ ) 
			{
				std::string shareGuid = *(std::string*)WinUtil::ListViewUtil::getItemParam(m_hListShares,i);

				Share share;
				if ( !ShareManager::getInstance()->findShareByGuid(shareGuid,&share) ) {
					continue;
				}

				std::string lastIndexed;
				std::string size = WinUtil::FormatUtil::formatFileSize(share.getSize());

				// check if share is queued for indexing
				std::list<IndexerJob> queue = Indexer::getInstance()->getQueue();
				std::list<IndexerJob>::iterator iter;
				for ( iter=queue.begin(); iter!=queue.end(); iter++ ) {
					if ( iter->getShareId()==share.getDbId() ) {
						lastIndexed = "Queued...";
					}
				}

				// check if share is currently being indexed
				if ( Indexer::getInstance()->getCurrentJob().getShareId()==share.getDbId() ) {
					lastIndexed = "Indexing...";
				}

				// check when share was last indexed
				if ( lastIndexed.empty() )
				{
					time_t lastIndexedTime = share.getLastIndexedTime();
					if ( lastIndexedTime>0 ) 
					{
						tm localTime;
						Util::TimeUtil::getLocalTime(lastIndexedTime,&localTime);
						lastIndexed = Util::TimeUtil::format(localTime,"%Y-%m-%d %H:%M");
					}
					else {
						lastIndexed = "Never";
					}
				}

				ListView_SetItemText(m_hListShares,i,2,(LPSTR)size.c_str());
				ListView_SetItemText(m_hListShares,i,3,(LPSTR)lastIndexed.c_str());
			}
		}
		break;

		case WM_COMMAND:
		{
			switch ( wParam )
			{
				case UID_ADD:
				{
					openShareDialog(NULL);
				}
				break;

				case UID_EDIT:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListShares);
					if ( selectedIndex!=-1 ) {
						std::string *pShareGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListShares,selectedIndex);
						openShareDialog(pShareGuid);
					}
				}
				break;

				case UID_REMOVE:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListShares);
					if ( selectedIndex!=-1 ) 
					{
						std::string *pShareGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListShares,selectedIndex);

						Share share;
						if ( ShareManager::getInstance()->findShareByGuid(*pShareGuid,&share) ) {
							ShareManager::getInstance()->removeShare(share);
							dialogRefresh();
						}
					}
				}
				break;

				case UID_INDEXALL:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListShares);
					if ( selectedIndex!=-1 ) 
					{
						std::string shareGuid = *(std::string*)WinUtil::ListViewUtil::getItemParam(m_hListShares,selectedIndex);

						Share share;
						if ( ShareManager::getInstance()->findShareByGuid(shareGuid,&share) ) {
							Indexer::getInstance()->queue(IndexerJob(share.getDbId(),true));
						}
					}
				}
				break;

				case UID_INDEXNEW:
				{
					int selectedIndex = ListView_GetSelectionMark(m_hListShares);
					if ( selectedIndex!=-1 ) 
					{
						std::string shareGuid = *(std::string*)WinUtil::ListViewUtil::getItemParam(m_hListShares,selectedIndex);

						Share share;
						if ( ShareManager::getInstance()->findShareByGuid(shareGuid,&share) ) {
							Indexer::getInstance()->queue(IndexerJob(share.getDbId(),false));
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
					if ( pNmh->hwndFrom==m_hListShares ) {
						SendMessage(m_hListShares,LVM_SETCOLUMNWIDTH,3,LVSCW_AUTOSIZE_USEHEADER);
					}
				}
				break;

				case NM_DBLCLK:
				{
					if ( pNmh->hwndFrom==m_hListShares )
					{
						LPNMITEMACTIVATE pNmItem = (LPNMITEMACTIVATE)lParam;
						if ( pNmItem->iItem!=-1 ) {
							std::string *pShareGuid = (std::string*)WinUtil::ListViewUtil::getItemParam(m_hListShares,pNmItem->iItem);
							openShareDialog(pShareGuid);
						}
					}
				}
				break;

				case NM_RCLICK:
				{
					if ( pNmh->hwndFrom==m_hListShares )
					{
						HMENU hPopupMenu;
						hPopupMenu = CreatePopupMenu();

						LPNMITEMACTIVATE pNmItem = (LPNMITEMACTIVATE)lParam;
						if ( pNmItem->iItem!=-1 )
						{
							AppendMenu(hPopupMenu,MF_STRING,UID_ADD,WinUtil::ResourceUtil::loadString(IDS_ADD).c_str());
							AppendMenu(hPopupMenu,MF_STRING,UID_EDIT,WinUtil::ResourceUtil::loadString(IDS_EDIT).c_str());
							AppendMenu(hPopupMenu,MF_STRING,UID_REMOVE,WinUtil::ResourceUtil::loadString(IDS_REMOVE).c_str());
							AppendMenu(hPopupMenu,MF_SEPARATOR,NULL,NULL);
							AppendMenu(hPopupMenu,MF_STRING,UID_INDEXALL,WinUtil::ResourceUtil::loadString(IDS_INDEXALL).c_str());
							AppendMenu(hPopupMenu,MF_STRING,UID_INDEXNEW,WinUtil::ResourceUtil::loadString(IDS_INDEXNEW).c_str());
						}
						else
						{
							AppendMenu(hPopupMenu,MF_STRING,UID_ADD,WinUtil::ResourceUtil::loadString(IDS_ADD).c_str());
							AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_EDIT).c_str());
							AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_REMOVE).c_str());
							AppendMenu(hPopupMenu,MF_SEPARATOR,NULL,NULL);
							AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_INDEXALL).c_str());
							AppendMenu(hPopupMenu,MF_STRING | MF_GRAYED,NULL,WinUtil::ResourceUtil::loadString(IDS_INDEXNEW).c_str());
						}

						WinUtil::MenuUtil::showPopupMenu(hWnd,hPopupMenu);
					}
				}
				break;
			}
		}
		break;
	}

	return 0;
}

void SharingTab::dialogInit(HWND hWnd,LPARAM lParam)
{
	m_hListShares = GetDlgItem(hWnd, IDC_LIST_SHARES);

	SendMessage(m_hListShares,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT);
	WinUtil::ListViewUtil::insertColumn(m_hListShares,0,WinUtil::ResourceUtil::loadString(IDS_VIRTUALNAME).c_str(),WinUtil::DpiUtil::scaleX(97),false);
	WinUtil::ListViewUtil::insertColumn(m_hListShares,1,WinUtil::ResourceUtil::loadString(IDS_PATH).c_str(),WinUtil::DpiUtil::scaleX(165),false);
	WinUtil::ListViewUtil::insertColumn(m_hListShares,2,WinUtil::ResourceUtil::loadString(IDS_SIZE).c_str(),WinUtil::DpiUtil::scaleX(80),false);
	WinUtil::ListViewUtil::insertColumn(m_hListShares,3,WinUtil::ResourceUtil::loadString(IDS_LASTINDEXED).c_str(),0,false);

	SetWindowPos(hWnd,HWND_TOP,WinUtil::DpiUtil::scaleX(15),WinUtil::DpiUtil::scaleY(30),-1,-1,SWP_NOSIZE);

	dialogRefresh();
}

void SharingTab::dialogRefresh()
{
	ListView_DeleteAllItems(m_hListShares);
	m_shareGuids.clear();

	std::list<Share> shares = ShareManager::getInstance()->getShares();
	std::list<Share>::iterator iter;
	for ( iter=shares.begin(); iter!=shares.end(); iter++ ) 
	{
		m_shareGuids.push_back(iter->getGuid());
		int index = WinUtil::ListViewUtil::insertItem(m_hListShares,-1,iter->getName(),(LPARAM)&m_shareGuids.back());
		WinUtil::ListViewUtil::insertSubItem(m_hListShares,index,1,iter->getPath());
	}

	PostMessage(this->getHwnd(),UWM_INDEX_UPDATED,NULL,NULL);
}

void SharingTab::openShareDialog(std::string *pShareGuid)
{
	ShareDialog shareDialog;

	if ( pShareGuid!=NULL )
	{
		Share share;
		if ( !ShareManager::getInstance()->findShareByGuid(*pShareGuid,&share) ) {
			MessageBox(this->getHwnd(),"Share no longer exists",APP_NAME,MB_ICONEXCLAMATION);
			return;
		}

		if ( shareDialog.init(this->getHwnd(),MAKEINTRESOURCE(IDD_USER),(LPARAM)&share) ) {
			ShareManager::getInstance()->updateShare(share);
			dialogRefresh();
		}
	}
	else
	{
		Share share;
		if ( shareDialog.init(this->getHwnd(),MAKEINTRESOURCE(IDD_USER),(LPARAM)&share) ) {
			ShareManager::getInstance()->addShare(share);
			dialogRefresh();
		}		
	}
}

void SharingTab::on(IndexerListener::JobCompleted) 
{
	PostMessage(this->getHwnd(),UWM_INDEX_UPDATED,NULL,NULL);
}

void SharingTab::on(IndexerListener::JobQueued,const IndexerJob &job)
{
	PostMessage(this->getHwnd(),UWM_INDEX_UPDATED,NULL,NULL);
}

void SharingTab::on(IndexerListener::JobStarted)
{
	PostMessage(this->getHwnd(),UWM_INDEX_UPDATED,NULL,NULL);
}
