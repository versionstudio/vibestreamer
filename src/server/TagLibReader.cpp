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
#include "taglibreader.h"

#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/tag.h>

const int TagLibReader::MAX_IMAGE_SIZE = 1024*1024;

void TagLibReader::extract(const std::wstring path,
	std::map<std::string,std::wstring> *metadata,std::list<MetadataImage::Ptr> *images)
{
	TagLib::FileRef fileRef(path.c_str());
	if ( !fileRef.isNull() ) 
	{
		if ( metadata!=NULL )
		{
			TagLib::Tag *tag = fileRef.tag();
			if ( tag!=NULL && !tag->isEmpty() )
			{	
				if ( !tag->album().isNull() ) {
					(*metadata)["mdAlbum"] = boost::trim_copy(tag->album().toWString());
				}

				if ( !tag->artist().isNull() ) {
					(*metadata)["mdArtist"] = boost::trim_copy(tag->artist().toWString());
				}

				if ( !tag->genre().isNull() ) {
					(*metadata)["mdGenre"] = boost::trim_copy(tag->genre().toWString());
				}

				if ( !tag->title().isNull() ) {
					(*metadata)["mdTitle"] = boost::trim_copy(tag->title().toWString());
				}

				// TODO: comment field is ignored since it caused unexpected crashes in rare cases (taglib bug?)

				(*metadata)["mdTrack"] = Util::ConvertUtil::toWideString(tag->track());
				(*metadata)["mdYear"] = Util::ConvertUtil::toWideString(tag->year());
			}

			TagLib::AudioProperties *audioProperties = fileRef.audioProperties();
			if ( audioProperties!=NULL )
			{
				(*metadata)["mdBitRate"] = Util::ConvertUtil::toWideString(audioProperties->bitrate());
				(*metadata)["mdChannels"] = Util::ConvertUtil::toWideString(audioProperties->channels());
				(*metadata)["mdLength"] = Util::ConvertUtil::toWideString(audioProperties->length());
				(*metadata)["mdSampleRate"] = Util::ConvertUtil::toWideString(audioProperties->sampleRate());
			}
		}

		if ( images!=NULL )
		{
			TagLib::MPEG::File *mpegFile = (TagLib::MPEG::File*)fileRef.file();
			if ( mpegFile->ID3v2Tag()!=NULL )
			{
				TagLib::ID3v2::Tag &id3Tag = *mpegFile->ID3v2Tag();
				TagLib::ID3v2::FrameList frameList = id3Tag.frameListMap()["APIC"];
				for ( TagLib::ID3v2::FrameList::Iterator iter=frameList.begin();
					iter!=frameList.end(); iter++ )
				{
					TagLib::ID3v2::AttachedPictureFrame *ap = (TagLib::ID3v2::AttachedPictureFrame*)*iter;
					const TagLib::ByteVector &iv = ap->picture();
					if ( iv.size()>0 && iv.size()<MAX_IMAGE_SIZE ) 
					{
						MetadataImage::Ptr imagePtr = MetadataImage::Ptr(new MetadataImage(ap->mimeType().toCString(),
							iv.data(),iv.size()));

						images->push_back(imagePtr);
					}
				}
			}
		}
	}
}
