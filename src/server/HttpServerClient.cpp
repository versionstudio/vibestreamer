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
#include "httpserverclient.h"

ssize_t HttpServerClient::recv(char *buffer,size_t bufferSize,bool nonBlocking)
{
	const ACE_Time_Value *timeout = NULL;
	if ( nonBlocking ) {
		timeout = &ACE_Time_Value::zero;
	}
	else {
		timeout = &m_timeout;
	}

	if ( m_sslPeer!=NULL ) {
		return m_sslPeer->recv(buffer,bufferSize,0,timeout);
	}
	else if ( m_peer!=NULL ) {
		return m_peer->recv(buffer,bufferSize,0,timeout);
	}

	return -1;	
}

ssize_t HttpServerClient::send(char *buffer,size_t length,bool nonBlocking)
{
	const ACE_Time_Value *timeout = NULL;
	if ( nonBlocking ) {
		timeout = &ACE_Time_Value::zero;
	}
	else {
		timeout = &m_timeout;
	}

	if ( m_sslPeer!=NULL ) {
		return m_sslPeer->send(buffer,length,timeout);
	}
	else if ( m_peer!=NULL ) {
		return m_peer->send(buffer,length,timeout);
	}

	return -1;
}
