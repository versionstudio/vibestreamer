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

#ifndef guard_taglibreader_h
#define guard_taglibreader_h

#include "metadatareader.h"

/**
* TagLibReader.
* Metadata reader that uses the TagLib library for reading id3 metadata.
*/
class TagLibReader : public MetadataReader
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	TagLibReader()
	{
		m_fieldNames.push_back("mdAlbum");
		m_fieldNames.push_back("mdArtist");
		m_fieldNames.push_back("mdBitRate");
		m_fieldNames.push_back("mdChannels");
		m_fieldNames.push_back("mdGenre");
		m_fieldNames.push_back("mdLength");
		m_fieldNames.push_back("mdSampleRate");
		m_fieldNames.push_back("mdTitle");
		m_fieldNames.push_back("mdTrack");
		m_fieldNames.push_back("mdYear");
	}

	static const int MAX_IMAGE_SIZE;

	/**
	* @override
	*/
	virtual void extract(const std::wstring path,
		std::map<std::string,std::wstring> *metadata,std::list<MetadataImage::Ptr> *images);
};

#endif
