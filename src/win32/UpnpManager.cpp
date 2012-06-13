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
#include "upnpmanager.h"

#define LOGGER_CLASSNAME "UpnpManager"

#include "../server/logmanager.h"

bool UpnpManager::addMapping(const UpnpMapping& mapping)
{
	bool addedMapping = false;

	// check if mapping already exists
	std::vector<UpnpMapping>::iterator iter;
	for ( iter=m_mappings.begin(); iter!=m_mappings.end(); ) 
	{
		if ( iter->getProtocol()==mapping.getProtocol() 
			&& iter->getPort()==mapping.getPort() )	{
				return true;
		}
	}

	IUPnPNAT *pNatInterface = NULL;
	IStaticPortMappingCollection* pPortMappingCollectionInterface = NULL;

	// create com instance of nat
	if ( SUCCEEDED(CoCreateInstance(CLSID_UPnPNAT,NULL,CLSCTX_ALL,IID_IUPnPNAT,(void **)&pNatInterface)) 
		&& pNatInterface!=NULL )
	{
		// get the collection of forwarded ports
		if ( SUCCEEDED(pNatInterface->get_StaticPortMappingCollection(&pPortMappingCollectionInterface))
			&& pPortMappingCollectionInterface!=NULL )
		{
			BSTR bstrProtocol = NULL;
			BSTR bstrDescription = NULL;
			BSTR bstrIpAddress = NULL;

			WinUtil::ConvertUtil::toBstr(mapping.getProtocol(),&bstrProtocol);
			WinUtil::ConvertUtil::toBstr(mapping.getDescription(),&bstrDescription);
			WinUtil::ConvertUtil::toBstr(mapping.getIpAddress(),&bstrIpAddress);

			// add port mapping
			IStaticPortMapping *pPortMappingInterface = NULL;
			if ( SUCCEEDED(pPortMappingCollectionInterface->Add(mapping.getPort(),bstrProtocol,mapping.getPort(),
				bstrIpAddress,VARIANT_TRUE,bstrDescription,&pPortMappingInterface)) &&
				pPortMappingInterface!=NULL )
			{
				addedMapping = true;
			}
			else
			{
				// try once more due to a strange bug - add port mapping
				if ( SUCCEEDED(pPortMappingCollectionInterface->Add(mapping.getPort(),bstrProtocol,mapping.getPort(),
					bstrIpAddress,VARIANT_TRUE,bstrDescription,&pPortMappingInterface)) && 
					pPortMappingInterface!=NULL )
				{
					addedMapping = true;
				}
			}

			if ( addedMapping )
			{
				m_mappings.push_back(mapping);

				// get external ip address
				BSTR bstrExternalIpAddress = NULL;
				if ( SUCCEEDED(pPortMappingInterface->get_ExternalIPAddress(&bstrExternalIpAddress)) ) {
					m_externalIpAddress = WinUtil::ConvertUtil::toString(bstrExternalIpAddress);
					SysFreeString(bstrExternalIpAddress);
				}
			}

			if ( pPortMappingInterface!=NULL ) {
				pPortMappingInterface->Release();
				pPortMappingInterface = NULL;
			}

			SysFreeString(bstrProtocol);
			SysFreeString(bstrDescription);
			SysFreeString(bstrIpAddress);
		}
	}

	if ( pPortMappingCollectionInterface!=NULL ) {
		pPortMappingCollectionInterface->Release();
		pPortMappingCollectionInterface = NULL;
	}

	if ( pNatInterface!=NULL ) {
		pNatInterface->Release();
		pNatInterface = NULL;
	}

	return addedMapping;
}

bool UpnpManager::removeMapping(const UpnpMapping& mapping)
{
	bool removedMapping = false;

	IUPnPNAT *pNatInterface = NULL;
	IStaticPortMappingCollection* pPortMappingCollectionInterface = NULL;

	// create com instance of nat
	if ( SUCCEEDED(CoCreateInstance(CLSID_UPnPNAT,NULL,CLSCTX_ALL,IID_IUPnPNAT,(void **)&pNatInterface)) 
		&& pNatInterface!=NULL )
	{
		// get the collection of forwarded ports
		if ( SUCCEEDED(pNatInterface->get_StaticPortMappingCollection(&pPortMappingCollectionInterface))
			&& pPortMappingCollectionInterface!=NULL )
		{
			BSTR bstrProtocol = NULL;
			WinUtil::ConvertUtil::toBstr(mapping.getProtocol(),&bstrProtocol);

			if ( SUCCEEDED(pPortMappingCollectionInterface->Remove(mapping.getPort(),bstrProtocol)) )
			{
				// remove mapping from vector
				std::vector<UpnpMapping>::iterator iter;
				for ( iter=m_mappings.begin(); iter!=m_mappings.end(); ) 
				{
					if ( iter->getProtocol()==mapping.getProtocol() 
						&& iter->getPort()==mapping.getPort() )	{
						iter = m_mappings.erase(iter);
						continue;
					}
					
					iter++;
				}

				removedMapping = true;
			}

			if ( pPortMappingCollectionInterface!=NULL ) {
				pPortMappingCollectionInterface->Release();
				pPortMappingCollectionInterface = NULL;
			}

			SysFreeString(bstrProtocol);
		}
	}

	if ( pPortMappingCollectionInterface!=NULL ) {
		pPortMappingCollectionInterface->Release();
		pPortMappingCollectionInterface = NULL;
	}

	if ( pNatInterface!=NULL ) {
		pNatInterface->Release();
		pNatInterface = NULL;
	}

	return removedMapping;
}

bool UpnpManager::clearMappings()
{
	bool mappingsCleared = true;

	std::vector<UpnpMapping>::iterator iter;
	std::vector<UpnpMapping> mappings = m_mappings;
	for ( iter=mappings.begin(); iter!=mappings.end(); iter++ ) {
		if ( !removeMapping(*iter) ) {
			mappingsCleared = false;
		}
	}

	return mappingsCleared;
}

bool UpnpManager::getDeviceInformation(UpnpDeviceInformation &deviceInfo)
{
	bool foundDeviceInfo = false;

	// create com instance of device finder
	IUPnPDeviceFinder *pDeviceFinderInterface = NULL;	
	if ( SUCCEEDED(CoCreateInstance(CLSID_UPnPDeviceFinder,NULL,CLSCTX_ALL,
		IID_IUPnPDeviceFinder,(void**)&pDeviceFinderInterface)) && pDeviceFinderInterface!=NULL )
	{
		BSTR bstrTypeUri = SysAllocString(L"urn:schemas-upnp-org:device:InternetGatewayDevice:1");
		
		// find devices of desired type
		IUPnPDevices *pFoundDevicesInterface = NULL;
		if ( SUCCEEDED(pDeviceFinderInterface->FindByType(bstrTypeUri,0,&pFoundDevicesInterface)) 
			&& pFoundDevicesInterface!=NULL )
		{
			// iterate through all found devices
			IUnknown *pUnknownInterface = NULL;
			if ( SUCCEEDED(pFoundDevicesInterface->get__NewEnum(&pUnknownInterface))
				&& pUnknownInterface!=NULL )
			{
				IEnumVARIANT *pEnumVar = NULL;
				if ( SUCCEEDED(pUnknownInterface->QueryInterface(IID_IEnumVARIANT,(void **)&pEnumVar))
					&& pEnumVar!=NULL )
				{
					VARIANT curDevice;
					VariantInit(&curDevice);
					pEnumVar->Reset();

					while ( pEnumVar->Next(1,&curDevice,NULL)==S_OK )
					{
						IUPnPDevice *pDeviceInterface = NULL;
						IDispatch *pDispatchInterface = V_DISPATCH(&curDevice);

						if ( SUCCEEDED(pDispatchInterface->QueryInterface(IID_IUPnPDevice,(void **)&pDeviceInterface))
							&& pDeviceInterface!=NULL )
						{
							BSTR bstr = NULL;

							// description
							if ( SUCCEEDED(pDeviceInterface->get_Description(&bstr)) ) {
								deviceInfo.setDescription(WinUtil::ConvertUtil::toString(bstr));
								SysFreeString(bstr);
								bstr = NULL;
							}

							// manufacturer name
							if ( SUCCEEDED(pDeviceInterface->get_ManufacturerName(&bstr)) ) {
								deviceInfo.setManufacturerName(WinUtil::ConvertUtil::toString(bstr));
								SysFreeString(bstr);
								bstr = NULL;
							}

							foundDeviceInfo = true;
						}

						if ( pDeviceInterface!=NULL ) {
							pDeviceInterface->Release();
							pDeviceInterface = NULL;
						}

						VariantClear(&curDevice);
					}
				}

				if ( pEnumVar!=NULL ) {
					pEnumVar->Release();
					pEnumVar = NULL;
				}	
			}

			if ( pUnknownInterface!=NULL ) {
				pUnknownInterface->Release();
				pUnknownInterface = NULL;
			}
		}

		SysFreeString(bstrTypeUri);
	}

	if ( pDeviceFinderInterface!=NULL ) {
		pDeviceFinderInterface->Release();
		pDeviceFinderInterface = NULL;
	}

	return foundDeviceInfo;
}
