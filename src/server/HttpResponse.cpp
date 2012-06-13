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
#include "httpresponse.h"

#include "httpserverclient.h"

const int HttpClientResponse::MAX_BODY_SIZE = 4096;
const int HttpClientResponse::MAX_HEADER_SIZE = 2097152;

bool HttpResponse::getCookie(const std::string &name,std::string &value) 
{
	std::map<std::string,std::string>::iterator iter;
	for ( iter=m_cookies.begin(); iter!=m_cookies.end(); iter++ ) {
		if ( boost::iequals(name,iter->first) ) {
			value = iter->second;
			return true;
		}
	}

	return false;
}

bool HttpResponse::getHeader(const std::string &name,std::string &value) 
{
	std::map<std::string,std::string>::iterator iter;
	for ( iter=m_headers.begin(); iter!=m_headers.end(); iter++ ) {
		if ( boost::iequals(name,iter->first) ) {
			value = iter->second;
			return true;
		}
	}

	return false;
}

void HttpServerResponse::flush()
{
	if ( !m_commited )
	{
		std::stringstream header;
		header << "HTTP/1.1 " << Util::ConvertUtil::toString(m_statusCode) << "\r\n";

		for ( std::map<std::string,std::string>::const_iterator iter=m_headers.begin(); 
			iter!=m_headers.end(); iter++ ) {
			header << iter->first << ": " << iter->second << "\r\n";
		}

		for ( std::map<std::string,std::string>::const_iterator iter=m_cookies.begin(); 
			iter!=m_cookies.end(); iter++ ) {
			header << "Set-Cookie: " << iter->first << "=" << iter->second << ";path=/\r\n";
		}

		header << "\r\n";
		m_commited = true;
		m_client->send((char*)header.str().c_str(),header.str().length());
	}

	if ( m_bufferLength>0 ) {
		m_client->send(m_buffer,m_bufferLength);
		memset(m_buffer,0,m_bufferSize);
		m_bufferLength = 0;
	}
}

void HttpServerResponse::write(const char *buffer,size_t length,bool autoFlush)
{
	if ( m_buffer==NULL ) {
		m_buffer = new char[m_bufferSize+1];
		memset(m_buffer,0,m_bufferSize+1);
	}

	size_t pos = 0;
	while ( pos<length )
	{
		int available = m_bufferSize-m_bufferLength;
		if ( available==0 ) {
			flush();
			available = m_bufferSize;
		}

		int bytes = length-pos;
		if ( bytes>available ) {
			bytes = available;
		}

		memcpy(m_buffer+m_bufferLength,buffer+pos,bytes);
		m_bufferLength += bytes;
		pos += bytes;
	}

	if ( autoFlush ) {
		flush();
	}
}

bool HttpClientResponse::attachBuffer(std::string buffer)
{
	try
	{
		if ( !m_parsedHeader )
		{
			m_header += buffer;
			buffer.clear();

			// check if header is complete
			size_t endpos = m_header.find("\r\n\r\n");
			if ( endpos!=std::string::npos ) {
				if ( endpos<m_header.length() ) {
					buffer = m_header.substr(endpos+4);
					m_header = m_header.substr(0,endpos+4);
				}
			}

			// make sure header size doesn't overflow
			if ( m_header.length()>MAX_HEADER_SIZE ) {
				throw 1;
			}

			if ( endpos!=std::string::npos ) {
				parseHeader();
			}
		}

		// check if any data was sent
		if ( !buffer.empty() )
		{
			// make sure body size doesn't overflow
			if ( m_body.length()+buffer.length()>MAX_BODY_SIZE ) {
				throw 1;
			}

			// check is data is chunked
			if ( m_chunked ) {
				m_body += buffer; // TODO: this is a temporary solution, chunk data support should be implemented.
			}
			else {
				m_body += buffer;
			}
		}
	}
	catch(int) {
		return false;
	}

	return true;
}

void HttpClientResponse::parseHeader()
{
	size_t start = 0;
	size_t pos = 0;
	size_t findIndex = 0;

	m_parsedHeader = true;

	pos = m_header.find("\r\n",0);
	if ( pos==std::string::npos ) {
		throw 1;
	}
	
	std::string firstLine = m_header.substr(0,pos);
	if ( firstLine.length()<5 || firstLine.substr(0,5)!="HTTP/" ) {
		throw 1;
	}

	std::string status;
	pos = firstLine.find(' ',0);
	if ( pos==std::string::npos ) {
		throw 1;
	}
	else
	{
		start = pos + 1;
		pos = firstLine.find(' ',start);
		if ( pos==std::string::npos ) {
			status = firstLine.substr(start);
		}
		else {
			status = firstLine.substr(start,pos-start);
		}
	}

	m_statusCode = Util::ConvertUtil::toInt(status);
	if ( m_statusCode<1 ) {
		throw 1;
	}

	start = firstLine.length()+2;
	while ( (pos=m_header.find("\r\n",start))!=std::string::npos )
	{
		std::string line = m_header.substr(start,pos-start);
		size_t delimiterPos = line.find(":");
		if ( delimiterPos!=std::string::npos ) 
		{
			std::string key = line.substr(0,delimiterPos);
			std::string value = line.substr(delimiterPos+2);
			
			m_headers[key] = value;

			if ( boost::iequals(key,"transfer-encoding") && boost::iequals(value,"chunked") ) {
				m_chunked = true;
			}	
		}

		start = pos+2;
	}
}
