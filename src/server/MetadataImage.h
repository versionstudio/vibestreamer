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

#ifndef guard_metadataimage_h
#define guard_metadataimage_h

#include <boost/shared_ptr.hpp>

/**
* MetadataImage.
* Class representing a metadata image.
*/
class MetadataImage
{
public:
	/**
	* Constructor.
	* @param mimeType the mime type of the image
	* @param data the image data
	* @param size the size of the image data
	* @return instance
	*/
	MetadataImage(std::string mimeType,const char *data,uint64_t size)
	{
		m_mimeType = mimeType;
		m_size = size;
		if ( size>0 ) {
			m_data = new char[size];
			memcpy(m_data,data,size);
		}
	}

	typedef boost::shared_ptr<MetadataImage> Ptr;

	/**
	* Destructor.
	*/
	~MetadataImage() 
	{
		if ( m_data!=NULL ) {
			delete m_data;
		}
	}

	/**
	* Get the image data.
	* @return the image data
	*/
	const char* getData() const {
		return m_data;
	}

	/**
	* Get the mime type of the image.
	* @return the mime type of the image
	*/
	const std::string &getMimeType() const {
		return m_mimeType;
	}

	/**
	* Get the image size.
	* @return image size in bytes
	*/
	const uint64_t getSize() const {
		return m_size;
	}

private:
	/**
	* Private copy constructor since instances of this class should
	* not be copied. Use the shared_ptr typedef "Ptr" instead for
	* memory efficient handling of metadata images.
	* @return instance
	*/
	MetadataImage(const MetadataImage &image) : m_data(NULL),
		m_size(0) 
	{

	}

	std::string m_mimeType;

	char *m_data;

	uint64_t m_size;
};

#endif
