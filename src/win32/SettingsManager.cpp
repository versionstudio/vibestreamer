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
#include "settingsmanager.h"

#include "../server/version.h"
#include "../tinyxml/tinyxml.h"

const std::string SettingsManager::AUTOSTART = "autoStart";
const std::string SettingsManager::CHECKFORUPDATES = "checkForUpdates";
const std::string SettingsManager::CONFIRMEXIT = "confirmExit";
const std::string SettingsManager::MINIMIZETOTRAY = "minimizeToTray";
const std::string SettingsManager::UPNPENABLED = "upnpEnabled";

void SettingsManager::load()
{
	TiXmlDocument document;
	document.LoadFile("settings.xml");
	TiXmlNode *pSettingsNode = document.FirstChildElement("settings");
	if ( pSettingsNode!=NULL )
	{
		// load all settings
		TiXmlNode *pNode = pSettingsNode->FirstChild();
		while ( pNode!=NULL )
		{
			if ( pNode->FirstChild()!=NULL ) {
				setString(pNode->ToElement()->Value(),pNode->ToElement()->FirstChild()->Value());
			}

			pNode = pSettingsNode->IterateChildren(pNode);
		}
	}

	loadDefaults();
}

void SettingsManager::save()
{
	TiXmlDocument document;
	document.LoadFile("settings.xml");
	TiXmlNode *pSettingsNode = document.FirstChildElement("settings");
	if ( pSettingsNode==NULL ) {
		pSettingsNode = document.InsertEndChild(TiXmlElement("settings"));		
	}

	if ( pSettingsNode!=NULL )
	{
		pSettingsNode->Clear();

		// save all settings
		std::map<std::string,std::string>::iterator iter;
		for ( iter=m_settings.begin(); iter!=m_settings.end(); iter++ ) {
			TiXmlNode *pNode = pSettingsNode->InsertEndChild(TiXmlElement(iter->first.c_str()));		
			pNode->InsertEndChild(TiXmlText(iter->second.c_str()));
		}
	}

	if ( document.FirstChild()!=NULL && document.FirstChild()->ToDeclaration()==NULL ) {
		document.InsertBeforeChild(document.FirstChild(),TiXmlDeclaration("1.0","iso-8859-1","yes"));
	}

	document.SaveFile();
}

void SettingsManager::restoreDefaults()
{
	m_settings.clear();
	loadDefaults();
}

void SettingsManager::loadDefaults()
{
	setDefaultBool(AUTOSTART,false);
	setDefaultBool(CHECKFORUPDATES,true);
	setDefaultBool(CONFIRMEXIT,false);
	setDefaultBool(MINIMIZETOTRAY,true);
	setDefaultBool(UPNPENABLED,false);
}
