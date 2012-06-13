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
#include "indexer.h"

#define LOGGER_CLASSNAME "Indexer"

#include <ace/os.h>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "logmanager.h"
#include "taglibreader.h"
#include "taskrunner.h"

bool Indexer::init()
{
	if ( LogManager::getInstance()->isDebug() ) {
		LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Initializing");
	}

	bool success = false;

	DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_INDEX,true);
	if ( conn!=NULL )
	{
		try
		{
			std::vector<std::string> metadataColumns;

			// get all existing columns
			sqlite3x::sqlite3_command cmd(conn->getSqliteConn(),"PRAGMA table_info([items]);");
			sqlite3x::sqlite3_reader reader = cmd.executereader();
			while ( reader.read() ) {
				metadataColumns.push_back(reader.getstring(1));
			}

			// make sure that a column exists for all metadata fields
			for ( std::list<IndexerMapping>::iterator mappingIter=m_mappings.begin(); 
				mappingIter!=m_mappings.end(); mappingIter++ )
			{
				std::list<std::string> fieldNames = mappingIter->getMetadataReader()->getFieldNames();
				for ( std::list<std::string>::iterator iter=fieldNames.begin(); 
					iter!=fieldNames.end(); iter++ )
				{
					if ( std::find(metadataColumns.begin(),metadataColumns.end(),*iter)==metadataColumns.end() ) {
						conn->getSqliteConn().executenonquery("ALTER TABLE [items] ADD COLUMN [" + *iter + "] TEXT COLLATE NOCASE");
					}
				}
			}

			try
			{
				std::list<Share> shares = ShareManager::getInstance()->getShares();

				// match all indexes with existing shares
				sqlite3x::sqlite3_command cmd(conn->getSqliteConn(),"SELECT DISTINCT shareId FROM [items]");
				sqlite3x::sqlite3_reader reader = cmd.executereader();
				while ( reader.read() )
				{
					uint64_t shareId = reader.getint64(0);
					bool matchedIndex = false;

					for ( std::list<Share>::iterator iter=shares.begin(); iter!=shares.end(); iter++ ) 
					{
						if ( iter->getDbId()==shareId )
						{
							uint64_t directories = conn->getSqliteConn().executeint64("SELECT COUNT(itemId) FROM [items]"
								" WHERE shareId=" + Util::ConvertUtil::toString(shareId) + " AND directory=1");

							uint64_t files = conn->getSqliteConn().executeint64("SELECT COUNT(itemId) FROM [items]"
								" WHERE shareId=" + Util::ConvertUtil::toString(shareId) + " AND directory=0");

							uint64_t size = conn->getSqliteConn().executeint64("SELECT SUM(size) FROM [items]"
								" WHERE shareId=" + Util::ConvertUtil::toString(shareId) + " AND directory=0");

							iter->setDirectories(directories);
							iter->setFiles(files);
							iter->setSize(size);

							ShareManager::getInstance()->updateShare(*iter);
							matchedIndex = true;
							break;
						}
					}

					if ( !matchedIndex ) {
						deleteDbEntry(shareId);
					}
				}

				success = true;
			}
			catch(exception &ex) {
				LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could not load indexes [%s]",ex.what());
			}
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could prepare index table [%s]",ex.what());
		}

		DatabaseManager::getInstance()->releaseConnection(conn);
	}

	return success;
}

bool Indexer::start() 
{
	if ( m_started ) {
		return false;
	}

	if ( !m_thread.start() ) {
		return false;
	}

	m_started = true;

	return true;
}

void Indexer::stop() 
{
	if ( !m_started ) {
		return;
	}

	m_thread.cancel();
	m_thread.join();

	m_started = false;
}

void Indexer::abort() 
{
	m_thread.interrupt();
}

void Indexer::run()
{
	while ( true )
	{
		if ( m_thread.isCancelled() ) {			
			break;
		}

		IndexerJob *job = popQueue();
		if ( job!=NULL ) {
			index(job);
			delete job;
		}
		else
		{
			// check if any shares should be auto indexed
			std::list<Share> shares = ShareManager::getInstance()->getSharesToIndex();
			for ( std::list<Share>::iterator iter=shares.begin(); iter!=shares.end(); iter++ ) {
				queue(IndexerJob(iter->getDbId(),true));
			}
		}

		m_thread.sleep(1000);
	}
}

void Indexer::queue(const IndexerJob &job)
{
	m_mutex.acquire();

	// remove any already existing jobs
	for ( std::list<IndexerJob>::iterator iter=m_queue.begin(); iter!=m_queue.end(); iter++ ) {
		if ( iter->getShareId()==job.getShareId() ) {
			m_queue.erase(iter);
			break;
		}
	}

	// abort if currently being indexed
	if ( m_currentJob.getShareId()==job.getShareId() ) {
		abort();
	}
	
	m_queue.push_back(job);
	m_mutex.release();
	fireEvent(IndexerListener::JobQueued(),job);
}

void Indexer::readMetadata(const std::wstring path,
	std::map<std::string,std::wstring> *metadata,std::list<MetadataImage::Ptr> *images)
{
	boost::filesystem::wpath boostPath(path,boost::filesystem::native);

	for ( std::list<IndexerMapping>::iterator iter=m_mappings.begin();
		iter!=m_mappings.end(); iter++ ) 
	{
		if ( boost::regex_search(boostPath.leaf(),iter->getFilePatternRegex()) ) {
			if ( iter->getMetadataReader()!=NULL ) {
				iter->getMetadataReader()->extract(path,metadata,images);
			}
		}
	}
}

void Indexer::deleteDbEntry(uint64_t shareId)
{
	std::stringstream query;
	query << "DELETE FROM [items] WHERE shareId=" << shareId;

	TaskRunner::getInstance()->schedule(
		new DatabaseTask(DatabaseManager::DATABASE_INDEX,query.str(),true));
}

IndexerJob* Indexer::popQueue()
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	IndexerJob *job = NULL;
	if ( !m_queue.empty() ) {
		job = new IndexerJob(m_queue.front());
		m_queue.pop_front();
	}

	return job;
}

void Indexer::index(IndexerJob *job)
{
	Share share;
	if ( ShareManager::getInstance()->findShareByDbId(job->getShareId(),&share) )
	{
		LogManager::getInstance()->info(LOGGER_CLASSNAME,"Indexing of '%s' started",share.getName().c_str());

		job->setStartTime(Util::TimeUtil::getCalendarTime());
		job->setState(IndexerJob::State::VALIDATING);
		setCurrentJob(*job);

		fireEvent(IndexerListener::JobStarted());

		DatabaseConnection *conn = DatabaseManager::getInstance()->getConnection(DatabaseManager::DATABASE_INDEX,true);
		if ( conn!=NULL )
		{
			std::wstring filePattern = Util::ConvertUtil::toWideString(
				ConfigManager::getInstance()->getString(ConfigManager::INDEXER_FILEPATTERN));

			boost::wregex filePatternRegex(filePattern,boost::regex_constants::icase);

			bool includeHidden = ConfigManager::getInstance()->getBool(ConfigManager::INDEXER_INCLUDEHIDDEN);
			bool interrupted = false;

			sqlite3x::sqlite3_transaction transaction(conn->getSqliteConn(),true);
			if ( validateProcess(job,conn,filePatternRegex,includeHidden) )
			{
				job->setState(IndexerJob::State::ANALYZING);
				setCurrentJob(*job);

				boost::filesystem::wpath boostPath(Util::ConvertUtil::toWideString(share.getPath()),
					boost::filesystem::native);

				IndexerItem rootItem(boostPath,true);
				if ( analyzeProcess(job,&rootItem,filePatternRegex,includeHidden) )
				{
					job->setState(IndexerJob::State::INDEXING);
					setCurrentJob(*job);

					if ( indexProcess(job,&rootItem,conn) )
					{
						if ( job->isFullIndexing() ) 
						{
							share.setDirectories(job->getNewDirectories());
							share.setFiles(job->getNewFiles());
							share.setSize(job->getNewSize());
						}
						else
						{
							share.setDirectories(share.getDirectories()+job->getNewDirectories());
							share.setFiles(share.getFiles()+job->getNewFiles());
							share.setSize(share.getSize()+job->getNewSize());
						}

						transaction.commit();
					}
					else {
						interrupted = true;
					}
				}
				else {
					interrupted = true;
				}
			}
			else {
				interrupted = true;
			}

			share.setLastIndexedTime(Util::TimeUtil::getCalendarTime());

			ShareManager::getInstance()->updateShare(share);

			if ( interrupted ) {
				transaction.rollback();
			}

			DatabaseManager::getInstance()->releaseConnection(conn);
			
			if ( interrupted ) {
				LogManager::getInstance()->info(LOGGER_CLASSNAME,"Indexing of '%s' was interrupted after %d seconds",
					share.getName().c_str(),job->getDuration());
			}
			else {
				LogManager::getInstance()->info(LOGGER_CLASSNAME,"Indexing of '%s' completed in %d seconds",
					share.getName().c_str(),job->getDuration());
			}
		}
		else {
			LogManager::getInstance()->info(LOGGER_CLASSNAME,
				"Indexing failed. Could not retrieve database connection");
		}
	}
	else {
		LogManager::getInstance()->info(LOGGER_CLASSNAME,"Indexing failed. Share could not be found");
	}

	setCurrentJob(IndexerJob());

	fireEvent(IndexerListener::JobCompleted());
}

bool Indexer::validateProcess(IndexerJob *job,DatabaseConnection *conn,
	const boost::wregex &filePatternRegex,bool includeHidden)
{
	if ( !job->isFullIndexing() ) {
		return true;
	}

	bool interrupted = false;

	try
	{
		std::wstringstream query;
		query << "SELECT itemId,name,path FROM [items] WHERE shareId=" << job->getShareId();

		sqlite3x::sqlite3_command cmd(conn->getSqliteConn(),query.str());
		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while ( reader.read() )
		{
			if ( m_thread.isInterrupted() ) {
				interrupted = true;
				break;
			}
			
			uint64_t dbId = reader.getint64(0);

			std::wstring fileName = reader.getstring16(1);
			std::wstring filePath = reader.getstring16(2);

			bool validItem = false;

			if ( LogManager::getInstance()->isDebug() ) {
				LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Validating '%ls'",
					filePath.c_str());
			}

#ifdef WIN32
			DWORD fileAttributes;

			// make sure path still exists
			fileAttributes = GetFileAttributesW(filePath.c_str());
			if ( fileAttributes!=INVALID_FILE_ATTRIBUTES ) 
			{
				if ( (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
					boost::regex_search(fileName,filePatternRegex) )
				{
					validItem = true;
				}
			}
#else
			try
			{
				// make sure path still exists
				boost::filesystem::wpath boostPath(filePath,boost::filesystem::native);
				if ( boost::filesystem::exists(boostPath) )
				{
					if ( boost::filesystem::is_directory(boostPath) ||
						boost::regex_search(fileName,filePatternRegex) ) 
					{
						validItem = true;
					}
				}
			}
			catch(boost::filesystem::filesystem_error error) {
			
			}
#endif

			if ( !validItem ) 
			{
				std::stringstream query;
				query << "DELETE FROM [items] WHERE shareId=" 
					  << job->getShareId() << " AND itemId=" << dbId;

				conn->getSqliteConn().executenonquery(query.str());
			}

			job->setCurrentPath(Util::ConvertUtil::toString(filePath));
			setCurrentJob(*job);
		}
	}
	catch(exception &ex) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Error validating share [%s]",ex.what());
	}

	return !interrupted;
}

bool Indexer::analyzeProcess(IndexerJob *job,IndexerItem *item,
	const boost::wregex &filePatternRegex,bool includeHidden)
{
	bool interrupted = false;

#ifdef WIN32
	WIN32_FIND_DATAW fileData;

	HANDLE file = FindFirstFileW(std::wstring(item->getPath().string()+L"\\*.*").c_str(),&fileData);
	if ( file!=INVALID_HANDLE_VALUE )
	{
		while ( FindNextFileW(file,&fileData)!=0 )
		{
			if ( m_thread.isInterrupted() ) {
				interrupted = true;
				break;
			}
			
			std::wstring fileName = fileData.cFileName;
			std::wstring filePath = item->getPath().string() + L"\\" + fileName;

			if ( fileName==L"." || fileName==L".." ) {
				continue;
			}

			if ( !includeHidden && (fileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ) {
				continue;
			}

			if ( LogManager::getInstance()->isDebug() ) {
				LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Analyzing '%ls'",
					filePath.c_str());
			}

			if ( fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				boost::filesystem::wpath boostPath(filePath,boost::filesystem::native);

				job->increaseAnalyzedDirectories();
				IndexerItem *directoryItem =item->addItem(IndexerItem(boostPath,true));
				if ( !analyzeProcess(job,directoryItem,filePatternRegex,includeHidden) ) {
					interrupted = true;
					break;
				}
			}
			else
			{
				if ( boost::regex_search(fileName,filePatternRegex) ) 
				{
					boost::filesystem::wpath boostPath(filePath,boost::filesystem::native);

					job->increaseAnalyzedFiles();
					item->addItem(IndexerItem(boostPath,false));
				}
			}

			job->setCurrentPath(Util::ConvertUtil::toString(filePath));
			setCurrentJob(*job);
		}
	}
	else
	{
		DWORD errorCode = GetLastError();
		if ( errorCode!=ERROR_NO_MORE_FILES ) 
		{
			if ( LogManager::getInstance()->isDebug() ) {
				LogManager::getInstance()->debug(LOGGER_CLASSNAME,
					"Could not analyze path '%ls'",item->getPath().string().c_str());
			}
		}
	}

	FindClose(file);
#else
	try
	{
		if ( boost::filesystem::exists(item->getPath()) ) 
		{
			boost::filesystem::wdirectory_iterator endIter;
			boost::filesystem::wdirectory_iterator iter(item->getPath());
			for ( iter; iter!=endIter; iter++ )
			{
				if ( m_thread.isInterrupted() ) {
					interrupted = true;
					break;
				}

				std::wstring filePath = iter->string();
				std::wstring fileName = iter->leaf();

				if ( !includeHidden && boost::starts_with(fileName,L".") ) {
					continue;
				}

				if ( LogManager::getInstance()->isDebug() ) {
					LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Analyzing '%ls'",
						filePath.c_str());
				}

				try
				{
					if ( boost::filesystem::is_directory(*iter) )
					{
						job->increaseAnalyzedDirectories();
						IndexerItem *directoryItem = item->addItem(IndexerItem(*iter,true));
						if ( !analyzeProcess(job,directoryItem,filePatternRegex,includeHidden) ) {
							interrupted = true;
							break;
						}
					}
					else
					{
						if ( boost::regex_search(fileName,filePatternRegex) ) {
							job->increaseAnalyzedFiles();
							item->addItem(IndexerItem(*iter,false));
						}
					}

					job->setCurrentPath(Util::ConvertUtil::toString(filePath));
					setCurrentJob(*job);
				}
				catch(boost::filesystem::filesystem_error error) {
					
				}
			}
		}
	}
	catch(boost::filesystem::filesystem_error error) 
	{
		if ( LogManager::getInstance()->isDebug() ) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,
				"Could not analyze path '%ls'",item->getPath().string().c_str());
		}
	}

#endif

	return !interrupted;
}

bool Indexer::indexProcess(IndexerJob *job,IndexerItem *item,DatabaseConnection *conn)
{
	bool interrupted = false;

	std::list<IndexerItem> &items = item->getItems();
	for ( std::list<IndexerItem>::iterator iter=items.begin(); 
		iter!=items.end(); iter++ )
	{
		if ( m_thread.isInterrupted() ) {
			interrupted = true;
			break;
		}

		std::wstring filePath = iter->getPath().string();
		std::wstring fileName = iter->getPath().leaf();

		if ( LogManager::getInstance()->isDebug() ) {
			LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Indexing '%ls'",
				filePath.c_str());
		}

		try
		{
			uint64_t existingId = 0;
			uint64_t existingLastWriteTime = 0;

			std::wstring existingName;
			std::wstring existingPath;

			std::wstringstream query;
			query << "SELECT itemId,name,path,lastWriteTime FROM [items]"
				  << " WHERE shareId=" << job->getShareId()
				  << " AND path='" << conn->quote(filePath) << "' LIMIT 1";

			// check if the item exists in the database
			sqlite3x::sqlite3_command cmd(conn->getSqliteConn(),query.str());
			sqlite3x::sqlite3_reader reader = cmd.executereader();
			while ( reader.read() )
			{
				existingId = reader.getint64(0);
				existingName = reader.getstring16(1);
				existingPath = reader.getstring16(2);
				existingLastWriteTime = reader.getint64(3);
			}

			if ( existingId>0 )
			{
				iter->setDbId(existingId);

				if ( job->isFullIndexing() )
				{
					try
					{
						uint64_t fileSize = 0;
						time_t lastWriteTime = boost::filesystem::last_write_time(iter->getPath());
						if ( !iter->isDirectory() ) {
							fileSize = boost::filesystem::file_size(iter->getPath());
						}

						if ( existingLastWriteTime!=lastWriteTime ) {
							updateItem(job,&*iter,lastWriteTime,fileSize,conn);
						}
						else if ( existingPath!=filePath ) {
							updateItemPath(job,&*iter,conn);
						}

						if ( iter->isDirectory() ) {
							job->increaseNewDirectories();
						}
						else {
							job->increaseNewFiles();
							job->increaseNewSize(fileSize);
						}
					}
					catch(boost::filesystem::filesystem_error error) {
						
					}
				}
			}
			else
			{
				try	
				{
					uint64_t fileSize = 0;
					time_t lastWriteTime = boost::filesystem::last_write_time(iter->getPath());
					if ( !iter->isDirectory() ) {
						fileSize = boost::filesystem::file_size(iter->getPath());
					}

					if ( insertItem(job,&*iter,lastWriteTime,fileSize,conn) )
					{
						if ( iter->isDirectory() ) {
							job->increaseNewDirectories();
						}
						else {
							job->increaseNewFiles();
							job->increaseNewSize(fileSize);
						}
					}
				}
				catch(boost::filesystem::filesystem_error error) {
						
				}
			}

			if ( iter->isDirectory() )
			{
				job->increaseIndexedDirectories();
				if ( !indexProcess(job,&*iter,conn) ) {
					interrupted = true;
					break;
				}
			}
			else {
				job->increaseIndexedFiles();
			}
		}
		catch(exception &ex) 
		{
			if ( LogManager::getInstance()->isDebug() ) {
				LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Error while indexing item [%s]",ex.what());
			}
		}

		job->setCurrentPath(Util::ConvertUtil::toString(filePath));
		setCurrentJob(*job);
	}

	return !interrupted;
}

bool Indexer::insertItem(IndexerJob *job,IndexerItem *item,time_t lastWriteTime,
		uint64_t size,DatabaseConnection *conn)
{
	uint64_t parentItemId = 0;
	if ( item->getParentItem()!=NULL ) {
		parentItemId = item->getParentItem()->getDbId();
	}

	std::string hash = Util::ConvertUtil::toString(boost::to_lower_copy(item->getPath().string()));
	hash = Util::CryptoUtil::md5Encode(hash.c_str(),hash.length());

	std::wstring metadataColumns;
	std::wstring metadataValues;
	std::map<std::string,std::wstring> metadata;
	Indexer::getInstance()->readMetadata(item->getPath().string(),&metadata,NULL);
	for ( std::map<std::string,std::wstring>::iterator iter=metadata.begin(); iter!=metadata.end(); iter++ ) {
		metadataColumns += L"," + Util::ConvertUtil::toWideString(iter->first);
		metadataValues += L",'" + conn->quote(iter->second) + L"'";
	}

	std::wstringstream query;
	query << "INSERT INTO [items]"
		  << " (shareId,parentItemId,name,hash,path,directory,directories,files,size,lastWriteTime" << metadataColumns << ")"
		  << " VALUES ("
		  << job->getShareId() << ","
		  << parentItemId << ","
		  << "'" << conn->quote(item->getPath().leaf()) << "',"
		  << "'" << Util::ConvertUtil::toWideString(hash) << "',"
		  << "'" << conn->quote(item->getPath().string()) << "',"
		  << item->isDirectory() << ","
		  << item->getDirectories() << ","
		  << item->getFiles() << ","
		  << size << ","
		  << lastWriteTime
		  << metadataValues << ")";

	try  {
		conn->getSqliteConn().executenonquery(query.str());
		item->setDbId(conn->getSqliteConn().insertid());
	}
	catch(exception &ex) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to insert index item [%s]",ex.what());
		return false;
	}

	return true;
}

bool Indexer::updateItem(IndexerJob *job,IndexerItem *item,time_t lastWriteTime,
		uint64_t size,DatabaseConnection *conn)
{
	std::wstringstream query;
	query << "UPDATE [items] SET "
		  << "name='" << conn->quote(item->getPath().leaf()) << "',"
		  << "path='" << conn->quote(item->getPath().string()) << "',"
		  << "directory=" << item->isDirectory() << ","
		  << "directories=" << item->getDirectories() << ","
		  << "files=" << item->getFiles() << ","
		  << "size='" << size << "',"
		  << "lastWriteTime='" << lastWriteTime << "'";

	std::map<std::string,std::wstring> metadata;
	Indexer::getInstance()->readMetadata(item->getPath().string(),&metadata,NULL);
	for ( std::map<std::string,std::wstring>::iterator iter=metadata.begin(); iter!=metadata.end(); iter++ ) {
		query << L"," << Util::ConvertUtil::toWideString(iter->first) << L"='" << conn->quote(iter->second) << L"'";
	}

	query << " WHERE shareId=" << job->getShareId() << " AND itemId=" << item->getDbId();

	try {
		conn->getSqliteConn().executenonquery(query.str());
	}
	catch(exception &ex) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to update index item [%s]",ex.what());
		return false;
	}

	return true;
}

bool Indexer::updateItemPath(IndexerJob *job,IndexerItem *item,DatabaseConnection *conn)
{
	std::wstringstream query;
	query << "UPDATE [items] SET "
		<< "name='" << conn->quote(item->getPath().leaf()) << "',"
		<< "path='" << conn->quote(item->getPath().string()) << "' "
		<< "WHERE shareId=" << job->getShareId() << " AND itemId=" << item->getDbId();

	try {
		conn->getSqliteConn().executenonquery(query.str());
	}
	catch(exception &ex) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to update index item path [%s]",ex.what());
		return false;
	}

	return true;
}

void Indexer::on(ShareManagerListener::ShareAdded,const Share &share)
{
	queue(IndexerJob(share.getDbId(),true));
}

void Indexer::on(ShareManagerListener::ShareRemoved,const Share &share)
{
	ACE_Read_Guard<ACE_Mutex> guard(m_mutex);

	// abort if currently being indexed
	if ( m_currentJob.getShareId()==share.getDbId() ) {
		abort();
	}
	
	deleteDbEntry(share.getDbId());
}

void Indexer::on(ConfigManagerListener::Load)
{
	TiXmlElement mappingsElement = ConfigManager::getInstance()->getElement(ConfigManager::INDEXER_MAPPINGS);
	TiXmlNode *mappingNode = mappingsElement.FirstChildElement("mapping");
	while ( mappingNode!=NULL )
	{
		MetadataReader *metadataReader = NULL;

		std::string filePattern;
		std::string metadataReaderName;

		TiXmlNode *node = NULL;

		node = mappingNode->FirstChild("filePattern");
		if ( node!=NULL && node->FirstChild()!=NULL ) {
			filePattern = node->FirstChild()->Value();
		}

		node = mappingNode->FirstChild("metadataReader");
		if ( node!=NULL && node->FirstChild()!=NULL ) {
			metadataReaderName = node->FirstChild()->Value();
		}

		boost::wregex filePatternRegex(Util::ConvertUtil::toWideString(filePattern),
			boost::regex_constants::icase);

		if ( metadataReaderName=="TagLibReader" ) {
			metadataReader = new TagLibReader();
		}

		m_mappings.push_back(IndexerMapping(filePatternRegex,metadataReader));

		mappingNode = mappingsElement.IterateChildren("mapping",mappingNode);
	}
}
