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

#ifndef guard_metadatareader_h
#define guard_metadatareader_h

#include "metadataimage.h"

/**
* MetadataReader.
* Abstract base class for all metadata readers.
*/
class MetadataReader
{
public:
	/**
	* Extract metadata from the given file.
	* Subclasses must override this method.
	* @param path the path to the file to extract metadata from
	* @param metadata out parameter for any found metadata
	* @param images out parameter for any found metadata images
	*/
	virtual void extract(const std::wstring path,
		std::map<std::string,std::wstring> *metadata,std::list<MetadataImage::Ptr> *images) = 0;

	/**
	* Get all the name of all fields this metadata reader can extract.
	* @return a collection with the name of all fields this metadata reader can extract
	*/
	const std::list<std::string>& getFieldNames() {
		return m_fieldNames;
	}

protected:
	std::list<std::string> m_fieldNames;
};

#endif
