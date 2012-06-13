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

#ifndef guard_indexer_h
#define guard_indexer_h

#include <ace/synch.h>
#include <boost/filesystem/path.hpp>

#include "configmanager.h"
#include "databasemanager.h"
#include "eventbroadcaster.h"
#include "metadatareader.h"
#include "sharemanager.h"
#include "singleton.h"
#include "thread.h"

/**
* IndexerJob.
* Represents a job for the indexer.
*/
class IndexerJob
{
public:
	enum State { IDLE, VALIDATING, ANALYZING, INDEXING };

	/** 
	* Default constructor. Creates a new instance with idle state.
	* @return instance
	*/
	IndexerJob() : m_analyzedDirectories(0),
		m_analyzedFiles(0),
		m_indexedDirectories(0),
		m_indexedFiles(0),
		m_fullIndexing(false),
		m_newDirectories(0),
		m_newFiles(0),
		m_newSize(0),
		m_startTime(0)
	{
		m_state = IndexerJob::State::IDLE;
	}

	/**
	* Constructor used for creating a new instance.
	* @param shareId the database id of the share being indexed
	* @param fullIndexing true if the job should represent a full indexing
	* and not just check for new items.
	* @return instance
	*/
	IndexerJob(uint64_t shareId,bool fullIndexing) : m_analyzedDirectories(0),
		m_analyzedFiles(0),
		m_indexedDirectories(0),
		m_indexedFiles(0),
		m_newDirectories(0),
		m_newFiles(0),
		m_newSize(0),
		m_startTime(0)
	{
		m_shareId = shareId;
		m_fullIndexing = fullIndexing;

		m_state = IndexerJob::State::IDLE;
	}

	/**
	* Increase the number of directories analyzed during the process.
	*/
	void increaseAnalyzedDirectories() {
		m_analyzedDirectories++;
	}

	/**
	* Increase the number of files analyzed during the process.
	*/
	void increaseAnalyzedFiles() {
		m_analyzedFiles++;
	}

	/**
	* Increase the number of directories indexed during the process.
	*/
	void increaseIndexedDirectories() {
		m_indexedDirectories++;
	}

	/**
	* Increase the number of files indexed during the process.
	*/
	void increaseIndexedFiles() {
		m_indexedFiles++;
	}

	/**
	* Increase the number of new directories indexed during the process.
	*/
	void increaseNewDirectories() {
		m_newDirectories++;
	}

	/**
	* Increase the number of new files indexed during the process.
	*/
	void increaseNewFiles() {
		m_newFiles++;
	}

	/**
	* Increase the total size of the new files indexed during the process.
	* @param size the size to append
	*/
	void increaseNewSize(uint64_t size) {
		m_newSize+=size;
	}

	/**
	* Get the number of directories analyzed during the process.
	* @return the number of directories analyzed during the process
	*/
	const uint64_t getAnalyzedDirectories() const {
		return m_analyzedDirectories;
	}

	/**
	* Get the number of files analyzed during the process.
	* @return the number of files analyzed during the process
	*/
	const uint64_t getAnalyzedFiles() const {
		return m_analyzedFiles;
	}

	/**
	* Get the path the process is currently working on.
	* @return currentPath the path the process is currently working on
	*/
	const std::string getCurrentPath() const {
		return m_currentPath;
	}

	/**
	* Get the duration of the process up to the current time.
	* @return the duration of the process
	*/
	const int getDuration() const {
		if ( m_startTime>0 ) {
			time_t currentTime = Util::TimeUtil::getCalendarTime();
			return (int)difftime(currentTime,m_startTime);
		}
		else {
			return 0;
		}
	}

	/**
	* Get the number of directories indexed during the process.
	* @return the number of directories indexed during the process
	*/
	const uint64_t getIndexedDirectories() const {
		return m_indexedDirectories;
	}

	/**
	* Get the number of files indexed during the process.
	* @return the number of files indexed during the process
	*/
	const uint64_t getIndexedFiles() const {
		return m_indexedFiles;
	}

	/**
	* Get the number of new directories indexed during the process.
	* @return the number of new directories indexed during the process
	*/
	const uint64_t getNewDirectories() const {
		return m_newDirectories;
	}

	/**
	* Get the number of new files indexed during the process.
	* @return the number of new files indexed during the process
	*/
	const uint64_t getNewFiles() const {
		return m_newFiles;
	}

	/**
	* Get the total size of the new files indexed during the process.
	* @return the total size of the new files indexed during the process.
	*/
	const uint64_t getNewSize() const {
		return m_newSize;
	}

	/**
	* Get the database id of the share used by the process.
	* @return the database id of the share used by the process
	*/
	const uint64_t getShareId() const {
		return m_shareId;
	}

	/**
	* Get the start time of the process.
	* @return the start time of the process
	*/
	const time_t getStartTime() const {
		return m_startTime;
	}

	/**
	* Get the current process state.
	* @return the process state
	*/
	const State getState() const {
		return m_state;
	}

	/**
	* Get whether the job represents a full indexing.
	* @return whether the job represents a full indexing
	*/
	const bool isFullIndexing() const {
		return m_fullIndexing;
	}

	/**
	* Set the path the process is currently working on.
	* @param currentPath the path the process is currently working on.
	*/
	void setCurrentPath(const std::string currentPath) {
		m_currentPath = currentPath;
	}

	/**
	* Set the process state.
	* @param processState the process state
	*/
	void setState(const State &state) {
		m_state = state;
	}

	/**
	* Set the start time of the process.
	* @param startTime the start time of the process
	*/
	void setStartTime(const time_t startTime) {
		m_startTime = startTime;
	}

private:
	State m_state;

	std::string m_currentPath;

	time_t m_startTime;

	uint64_t m_analyzedDirectories;
	uint64_t m_analyzedFiles;
	uint64_t m_indexedDirectories;
	uint64_t m_indexedFiles;
	uint64_t m_newDirectories;
	uint64_t m_newFiles;
	uint64_t m_newSize;
	uint64_t m_shareId;

	bool m_fullIndexing;
};

/**
* IndexerItem.
* Represents an item in a hierarchical structure of files and directories
* currently being indexed by the indexing service.
*/
class IndexerItem
{
public:
	/**
	* Constructor used for creating a new instance representing a file.
	* @param path the path to the file or directory that this item represents.
	* @param directory whether the item is a directory or not
	* @return instance
	*/
	IndexerItem(const boost::filesystem::wpath &path,bool directory) : m_dbId(0),
		m_directories(0),
		m_files(0),
		m_parentItem(NULL)
	{
		m_path = path;
		m_directory = directory;
	}

	/**
	* Add an item as a child of the current item.
	* @param item the item to add as a child item
	* @return a pointer to the newly added child item
	*/
	IndexerItem* addItem(IndexerItem &item)
	{
		item.setParentItem(this);
		if ( item.isDirectory() ) {
			m_directories++;
		}
		else {
			m_files++;
		}

		m_items.push_back(item);
		return &m_items.back();
	}

	/**
	* Get the database id of the current item.
	* @return the database id of the current item
	*/	
	const uint64_t getDbId() const {
		return m_dbId;
	}

	/**
	* Get the number of directories that are children of the current item.
	* @return the number of directories that are children of the current item
	*/
	const uint64_t getDirectories() const {
		return m_directories;
	}

	/**
	* Get the number of files that are children of the current item.
	* @return the number of files that are children of the current item
	*/
	const uint64_t getFiles() const {
		return m_files;
	}

	/**
	* Get all children of the current item.
	* @return a collection of all child items
	*/
	std::list<IndexerItem>& getItems() {
		return m_items;
	}

	/**
	* Get the parent item.
	* @return the parent item
	*/
	const IndexerItem* getParentItem() const {
		return m_parentItem;
	}

	/**
	* Get the path to the file or directory that this item represents.
	* @return the path of the file or directory that this item represents
	*/
	const boost::filesystem::wpath getPath() const {
		return m_path;
	}

	/**
	* Get whether the current item is a directory.
	* @return true if the current item is a directory
	*/
	const bool isDirectory() const {
		return m_directory;
	}

	/**
	* Set the database id of the current item
	* @param dbId the database id of the current item
	*/
	void setDbId(uint64_t dbId) {
		m_dbId = dbId;
	}

private:
	/**
	* Set the parent item.
	* @param set the parent item
	*/
	void setParentItem(IndexerItem *parentItem) {
		m_parentItem = parentItem;
	}

	IndexerItem *m_parentItem;

	std::list<IndexerItem> m_items;

	boost::filesystem::wpath m_path;

	uint64_t m_dbId;
	uint64_t m_directories;
	uint64_t m_files;

	bool m_directory;
};

/**
* IndexerMapping.
* Represents a mapping towards a file pattern and a metadata reader
* that can extract data from the files.
*/
class IndexerMapping
{
public:
	/**
	* Constructor used for creating a new instance.
	* @param filePatternRegex the file pattern regular expression
	* @param metadataReader the name of the metadata reader that can
	* read the given file formats
	* @return instance
	*/
	IndexerMapping(boost::wregex filePatternRegex,MetadataReader *metadataReader) {
		m_filePatternRegex = filePatternRegex;
		m_metadataReader = metadataReader;
	}

	/**
	* Get the file pattern regular expression.
	* @return the file pattern regular expression.
	*/
	const boost::wregex& getFilePatternRegex() const {
		return m_filePatternRegex;
	}

	/**
	* Get the metadata reader.
	* @return the metadata reader
	*/
	MetadataReader *getMetadataReader() {
		return m_metadataReader;
	}

private:
	MetadataReader *m_metadataReader;

	boost::wregex m_filePatternRegex;
};

/**
* IndexerListener.
* Abstract class containing event definitions for the Indexer class.
*/
class IndexerListener
{
public:
	template<int I>	struct X { enum { TYPE = I };  };

	typedef X<0> JobCompleted;
	typedef X<1> JobQueued;
	typedef X<2> JobStarted;

	/**
	* Fired when a job has completed.
	*/
	virtual void on(JobCompleted) = 0;

	/**
	* Fired when a job is added to the queue.
	* @param job the indexer job
	*/
	virtual void on(JobQueued,const IndexerJob &job) = 0;

	/**
	* Fired when a job has started.
	*/
	virtual void on(JobStarted) = 0;
};

/**
* Indexer.
* Singleton class that handles the indexing process.
*/
class Indexer : public Singleton<Indexer>,
				public EventBroadcaster<IndexerListener>,
				public Runnable,
				public ConfigManagerListener,
				public ShareManagerListener
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	Indexer() : m_started(false),
		m_thread(this)
	{
		ConfigManager::getInstance()->addListener(this);
		ShareManager::getInstance()->addListener(this);
	}

	/**
	* Destructor.
	*/
	~Indexer() {
		ConfigManager::getInstance()->removeListener(this);
		ShareManager::getInstance()->removeListener(this);
	}

	/**
	* Initialize and prepare the indexer for usage.
	* @return true if indexer was initialized successfully
	*/
	bool init();

	/**
	* Start the indexing service.
	* @return true if service was started successfully
	*/
	bool start();

	/**
	* Stop the indexing service.
	* Any ongoing indexing will be aborted.
	*/
	void stop();

	/**
	* Abort any ongoing indexing.
	*/
	void abort();

	/**
	* @override
	*/
	virtual void run();

	/**
	* Queue an indexer job.
	* @param job the indexer job
	*/
	void queue(const IndexerJob &job);

	/**
	* Read the metadata from the file at the given path.
	* The file name will be checked against the indexer mappings and
	* the metadata will be extracted by the first matching metadata reader.
	* @param path the path to the file to extract metadata from
	* @param metadata out parameter for any found metadata
	* @param images out parameter for any found metadata images
	*/
	void readMetadata(const std::wstring path,
		std::map<std::string,std::wstring> *metadata,std::list<MetadataImage::Ptr> *images);

	/**
	* Get the current job.
	* @return the current job
	*/
	const IndexerJob getCurrentJob() {
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex);
		return m_currentJob;
	}

	/**
	* Get the indexing queue.
	* @return a collection of all jobs queued for indexing
	*/
	std::list<IndexerJob> getQueue() {
		ACE_Read_Guard<ACE_Mutex> guard(m_mutex);
		return m_queue;
	}

	/**
	* @override
	*/
	virtual void on(ShareManagerListener::ShareAdded,const Share &share);

	/**
	* @override
	*/
	virtual void on(ShareManagerListener::ShareRemoved,const Share &share);

	/**
	* @override
	*/
	virtual void on(ShareManagerListener::ShareUpdated,const Share &share) {
		// not implemented
	}

	/**
	* @override
	*/
	virtual void on(ConfigManagerListener::Load);

	/**
	* @override
	*/
	virtual void on(ConfigManagerListener::Save) {
		// not implemented
	}

private:
	/**
	* Delete the database entry for the given index.
	* This method is the same as cleaning the entire index.
	* @param shareId the database id of the share to delete the index for
	*/
	void deleteDbEntry(uint64_t shareId);

	/**
	* Pop the next job be processed from the queue.
	* @return the job that should be processed or NULL if no job is queued for processing
	*/
	IndexerJob* popQueue();

	/**
	* Index the given job.
	* @param job the job to process
	*/
	void index(IndexerJob *job);

	/**
	* Validate any existing indexed items in the database
	* @param job the indexer job currently being processed
	* @param conn the connection to the index database
	* @return false if the process was interrupted
	*/
	bool validateProcess(IndexerJob *job,DatabaseConnection *conn,
		const boost::wregex &filePatternRegex,bool includeHidden);

	/**
	* Find and analyze all items.
	* @param job the indexer job currently being processed
	* @param item the index item to analyze and append all found items to
	* @param filePatternRegex the file pattern regular expression
	* @param includeHidden whether hidden files should be included in the indexing
	* @return false if the process was interrupted
	*/
	bool analyzeProcess(IndexerJob *job,IndexerItem *item,
		const boost::wregex &filePatternRegex,bool includeHidden);

	/**
	* Index all the found items.
	* @param job the indexer job currently being processed
	* @param item the index item to analyze and append all found items to
	* @param conn the connection to the index database
	* @return false if the process was interrupted
	*/
	bool indexProcess(IndexerJob *job,IndexerItem *item,DatabaseConnection *conn);

	/**
	* Insert the given item into the database.
	* If successfull this method will set the items database id.
	* @param job the indexer job currently being processed
	* @param item the item to insert into the database
	* @param lastWriteTime the last time the item was modified
	* @param size the file size of the item
	* @param conn the connection to the index database
	* @
	* @return true if the item was inserted succesfully
	*/
	bool insertItem(IndexerJob *job,IndexerItem *item,time_t lastWriteTime,
		uint64_t size,DatabaseConnection *conn);

	/**
	* Update the given item in the database.
	* @param job the indexer job currently being processed
	* @param item the item to update in the database
	* @param lastWriteTime the last time the item was modified
	* @param size the file size of the item
	* @param conn the connection to the index database
	* @return true if the item was updated successfully
	*/
	bool updateItem(IndexerJob *job,IndexerItem *item,time_t lastWriteTime,
		uint64_t size,DatabaseConnection *conn);

	/**
	* Update the given item path and file name in the database.
	* @param job the indexer job currently being processed
	* @param item the item to update in the database
	* @param conn the connection to the index database
	* @return true if the item was updated successfully
	*/
	bool updateItemPath(IndexerJob *job,IndexerItem *item,DatabaseConnection *conn);

	/**
	* Set the current job.
	* @param currentJob the current job
	*/
	void setCurrentJob(const IndexerJob &currentJob) {
		ACE_Write_Guard<ACE_Mutex> guard(m_mutex);
		m_currentJob = currentJob;
	}

	ACE_Mutex m_mutex;

	Thread m_thread;

	IndexerJob m_currentJob;

	std::list<IndexerMapping> m_mappings;

	std::list<IndexerJob> m_queue;

	bool m_started;
};

#endif
