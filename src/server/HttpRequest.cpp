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
#include "httprequest.h"

#include <ace/inet_addr.h>
#include <ace/sock_connector.h>
#include <ace/time_value.h>

#include "httpserverclient.h"

bool HttpRequest::getCookie(const std::string &name,std::string *value) 
{
	std::map<std::string,std::string>::iterator iter;
	for ( iter=m_cookies.begin(); iter!=m_cookies.end(); iter++ ) 
	{
		if ( boost::iequals(name,iter->first) )
		{
			if ( value!=NULL ) {
				*value = iter->second;
			}

			return true;
		}
	}

	return false;
}

bool HttpRequest::getHeader(const std::string &name,std::string *value) 
{
	std::map<std::string,std::string>::iterator iter;
	for ( iter=m_headers.begin(); iter!=m_headers.end(); iter++ ) 
	{
		if ( boost::iequals(name,iter->first) ) 
		{
			if ( value!=NULL ) {
				*value = iter->second;
			}

			return true;
		}
	}

	return false;
}

bool HttpRequest::getParameter(const std::string &name,std::string *value)
{
	std::map<std::string,std::string>::iterator iter;
	for ( iter=m_parameters.begin(); iter!=m_parameters.end(); iter++ ) 
	{
		if ( boost::iequals(name,iter->first) )
		{
			if ( value!=NULL ) {
				*value = iter->second;
			}

			return true;
		}
	}

	return false;
}

void HttpServerRequest::removeAttribute(std::string name)
{
	std::map<std::string,std::string>::iterator iter = m_attributes.find(name);
	if ( iter!=m_attributes.end() ) {
		m_attributes.erase(iter);
	}
}

bool HttpServerRequest::getAttribute(std::string name,std::string *value) 
{
	std::map<std::string,std::string>::iterator iter = m_attributes.find(name);
	if ( iter!=m_attributes.end() ) 
	{
		if ( value!=NULL ) {
			*value = iter->second;
		}

		return true;
	}

	return false;
}

std::string HttpServerRequest::getPath()
{
	std::string path;

	if ( m_site!=NULL ) {
		path = m_uri.substr(m_site->getPath().length());
	}

	return path;
}

std::string HttpServerRequest::getRealPath()
{
	std::string realPath;

	if ( m_site!=NULL ) {
		realPath = m_site->getRealPath(getPath());
	}

	return realPath;
}

std::string HttpServerRequest::getRemoteAddress() 
{ 
	return m_client->getRemoteAddress().get_host_addr(); 
}

std::string HttpServerRequest::getRemoteHost() 
{ 
	return m_client->getRemoteAddress().get_host_name();
}

bool HttpClientRequest::execute(HttpClientResponse &response)
{
	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream peer;
	ACE_INET_Addr addr;

	ACE_Time_Value tv(0,m_timeout*1000);
	ACE_Time_Value *timeout = NULL;

	if ( m_timeout>0 ) {
		timeout = &tv;
	}

	if ( addr.set(m_port,m_host.c_str())==-1 ) {
		return false;
	}

	if ( connector.connect(peer,addr,timeout)==-1 ) {
		return false;
	}

	std::stringstream postData;
	for ( std::map<std::string,std::string>::iterator iter=m_parameters.begin(); 
		iter!=m_parameters.end(); iter++ )
	{
		if ( iter!=m_parameters.begin() ) {
			postData << "&";
		}

		postData << iter->first << "=" << iter->second;
	}
	
	std::stringstream header;
	header << m_method << " " << m_uri;	

	if ( !m_queryString.empty() ) {
		header << "?" << m_queryString;
	}

	header << " HTTP/" << m_version << "\r\n";
	header << "Host: " << m_host << "\r\n";
	header << "Connection: close\r\n";

	for ( std::map<std::string,std::string>::iterator iter=m_headers.begin(); 
		iter!=m_headers.end(); iter++ ) {
		header << iter->first << ":" << iter->second << "\r\n";
	}

	if ( !postData.str().empty() ) {
		header << "Content-Length: " << postData.str().length() << "\r\n";
		header << "Content-Type: application/x-www-form-urlencoded\r\n";
	}

	header << "\r\n";

	if ( !postData.str().empty() ) {
		header << postData.str().c_str();
	}

	peer.send(header.str().c_str(),header.str().length(),timeout);

	char *buffer = new char[m_bufferSize];
	memset(buffer,0,m_bufferSize);

	while ( peer.recv(buffer,m_bufferSize-1,timeout)>0 )
	{
		if ( !response.attachBuffer(buffer) ) {
			break;
		}

		memset(buffer,0,m_bufferSize);
	}

	delete[] buffer;
	peer.close();

	return true;
}
