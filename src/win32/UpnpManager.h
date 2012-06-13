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

#ifndef guard_upnpmanager_h
#define guard_upnpmanager_h

#include <natupnp.h>
#include <upnp.h>

#include "singleton.h"

class UpnpDeviceInformation
{
public:
	UpnpDeviceInformation() {

	}

	const std::string& getDescription() const {
		return m_description;
	}

	const std::string& getManufacturerName() const {
		return m_manufacturerName;
	}

	void setDescription(const std::string description) {
		m_description = description;
	}

	void setManufacturerName(const std::string manufacturerName) {
		m_manufacturerName = manufacturerName;
	}

private:
	std::string m_description;
	std::string m_manufacturerName;
};

class UpnpMapping
{
public:
	UpnpMapping(std::string protocol,std::string description,
		std::string ipAddress,int port)
	{
		m_protocol = protocol;
		m_description = description;
		m_ipAddress = ipAddress;
		m_port = port;
	}

	const std::string& getDescription() const {
		return m_description;
	}

	const std::string& getIpAddress() const {
		return m_ipAddress;
	}

	const int getPort() const {
		return m_port;
	}

	const std::string& getProtocol() const {
		return m_protocol;
	}

private:
	std::string m_protocol;
	std::string m_description;
	std::string m_ipAddress;

	int m_port;
};

class UpnpManager : public Singleton<UpnpManager>
{
public:
	bool addMapping(const UpnpMapping& mapping);
	bool removeMapping(const UpnpMapping& mapping);
	bool clearMappings();

	bool getDeviceInformation(UpnpDeviceInformation &deviceInfo);

	bool hasMappings() {
		return !m_mappings.empty();
	}
	
	const std::string getExternalIpAddress() {
		return m_externalIpAddress;
	}

private:
	std::vector<UpnpMapping> m_mappings;

	std::string m_externalIpAddress;
};

#endif
