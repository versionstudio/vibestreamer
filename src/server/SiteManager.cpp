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
#include "sitemanager.h"

#define LOGGER_CLASSNAME "SiteManager"

#include "../tinyxml/tinyxml.h"

#include "logmanager.h"
#include "usermanager.h"

int SiteManager::load()
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Loading");

	m_sites.clear();

	TiXmlDocument document;
	document.LoadFile("conf\\sites.xml");
	TiXmlNode *sitesNode = document.FirstChildElement("sites");
	if ( sitesNode!=NULL )
	{
		// create all site instances
		TiXmlNode *siteNode = sitesNode->FirstChildElement("site");
		while ( siteNode!=NULL )
		{
			Site site;

			TiXmlNode *node = NULL;

			node = siteNode->FirstChild("name");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				site.setName(node->FirstChild()->Value());
			}

			node = siteNode->FirstChild("root");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				site.setRoot(node->FirstChild()->Value());
			}

			node = siteNode->FirstChild("path");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				site.setPath(node->FirstChild()->Value());
			}

			node = siteNode->FirstChild("anonymousAccess");
			if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
				site.setAnonymousAccess(true);
			}

			node = siteNode->FirstChild("anonymousUserGuid");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				site.setAnonymousUserGuid(node->FirstChild()->Value());
			}

			node = siteNode->FirstChild("authForm");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				site.setAuthForm(node->FirstChild()->Value());
			}

			node = siteNode->FirstChild("authRealm");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				site.setAuthRealm(node->FirstChild()->Value());
			}

			node = siteNode->FirstChild("authType");
			if ( node!=NULL && node->FirstChild()!=NULL ) {
				site.setAuthType(node->FirstChild()->Value());
			}

			TiXmlNode *accessLoggerNode = siteNode->FirstChildElement("accessLogger");
			if ( accessLoggerNode!=NULL )
			{
				TiXmlNode *node = NULL;

				std::string path;

				node = accessLoggerNode->FirstChild("path");
				if ( node!=NULL && node->FirstChild()!=NULL ) {
					path = node->FirstChild()->Value();
				}

				site.setAccessLogger(new AccessLogger(path));
			}

			TiXmlNode *permissionsNode = siteNode->FirstChildElement("permissions");
			if ( permissionsNode!=NULL ) 
			{
				std::list<Permission> permissions;

				TiXmlNode *permissionNode = permissionsNode->FirstChildElement("permission");
				while ( permissionNode!=NULL )
				{
					std::string userGuid;
					std::string groupGuid;
					std::string remoteAddress;
					
					bool allowed = false;
					bool validPermission = true;

					node = permissionNode->FirstChild("userGuid");
					if ( node!=NULL && node->FirstChild()!=NULL ) {
						userGuid = node->FirstChild()->Value();
					}

					node = permissionNode->FirstChild("groupGuid");
					if ( node!=NULL && node->FirstChild()!=NULL ) {
						groupGuid = node->FirstChild()->Value();
					}

					node = permissionNode->FirstChild("remoteAddress");
					if ( node!=NULL && node->FirstChild()!=NULL ) {
						remoteAddress = node->FirstChild()->Value();
					}

					node = permissionNode->FirstChild("allowed");
					if ( node!=NULL && node->FirstChild()!=NULL && Util::ConvertUtil::toBool(node->FirstChild()->Value()) ) {
						allowed = true;
					}

					// make sure specified user exists
					if ( !userGuid.empty() && !UserManager::getInstance()->findUserByGuid(userGuid,NULL) ) {
						validPermission = false;
					}

					// make sure specified group exists
					if ( !groupGuid.empty() && !UserManager::getInstance()->findGroupByGuid(groupGuid,NULL) ) {
						validPermission = false;
					}

					if ( validPermission ) {
						permissions.push_back(Permission(userGuid,groupGuid,remoteAddress,allowed));
					}

					permissionNode = permissionsNode->IterateChildren("permission",permissionNode);
				}

				site.setPermissions(permissions);
			}

			TiXmlNode *optionsNode = siteNode->FirstChild("options");
			if ( optionsNode!=NULL )
			{
				std::map<std::string,std::string> options;

				node = optionsNode->FirstChildElement("option");
				while ( node!=NULL )
				{
					const char *name = node->ToElement()->Attribute("name");
					if ( name!=NULL )
					{
						std::string value;
						if ( node->FirstChild()!=NULL ) {
							value = node->FirstChild()->Value();
						}

						options[name] = value;
					}

					node = optionsNode->IterateChildren("option",node);
				}

				site.setOptions(options);
			}

			site.loadConfig();

			site.setManager(this);
			m_sites.push_back(site);

			siteNode = sitesNode->IterateChildren("site",siteNode);
		}
	}

	return 0;
}

int SiteManager::save()
{
	ACE_Write_Guard<ACE_Mutex> guard(m_mutex);

	LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Saving");

	TiXmlDocument document;
	document.LoadFile("conf\\sites.xml");
	TiXmlNode *sitesNode = document.FirstChildElement("sites");
	if ( sitesNode==NULL ) {
		sitesNode = document.InsertEndChild(TiXmlElement("sites"));
	}

	if ( sitesNode!=NULL )
	{
		sitesNode->Clear();

		// save all site instances
		std::list<Site>::iterator iter;
		for ( iter=m_sites.begin(); iter!=m_sites.end(); iter++ ) 
		{
			TiXmlNode *siteNode = sitesNode->InsertEndChild(TiXmlElement("site"));
			if ( siteNode!=NULL )	
			{
				TiXmlNode *node = NULL;

				node = siteNode->InsertEndChild(TiXmlElement("name"));
				node->InsertEndChild(TiXmlText(iter->getName().c_str()));

				node = siteNode->InsertEndChild(TiXmlElement("root"));
				node->InsertEndChild(TiXmlText(iter->getRoot().c_str()));

				node = siteNode->InsertEndChild(TiXmlElement("path"));
				node->InsertEndChild(TiXmlText(iter->getPath().c_str()));

				node = siteNode->InsertEndChild(TiXmlElement("anonymousAccess"));
				node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isAnonymousAccess()).c_str()));

				node = siteNode->InsertEndChild(TiXmlElement("anonymousUserGuid"));
				node->InsertEndChild(TiXmlText(iter->getAnonymousUserGuid().c_str()));

				node = siteNode->InsertEndChild(TiXmlElement("authForm"));
				node->InsertEndChild(TiXmlText(iter->getAuthForm().c_str()));

				node = siteNode->InsertEndChild(TiXmlElement("authRealm"));
				node->InsertEndChild(TiXmlText(iter->getAuthRealm().c_str()));

				node = siteNode->InsertEndChild(TiXmlElement("authType"));
				node->InsertEndChild(TiXmlText(iter->getAuthType().c_str()));

				if ( iter->getAccessLogger()!=NULL )
				{
					TiXmlNode *accessLoggerNode = siteNode->InsertEndChild(TiXmlElement("accessLogger"));
					if ( accessLoggerNode!=NULL )
					{
						node = accessLoggerNode->InsertEndChild(TiXmlElement("path"));
						node->InsertEndChild(TiXmlText(iter->getAccessLogger()->getPath().c_str()));
					}
				}

				TiXmlNode *permissionsNode = siteNode->InsertEndChild(TiXmlElement("permissions"));
				if ( permissionsNode!=NULL )
				{
					std::list<Permission> permissions = iter->getPermissions();
					std::list<Permission>::iterator iter;
					for ( iter=permissions.begin(); iter!=permissions.end(); iter++ ) 
					{
						TiXmlNode *permissionNode = permissionsNode->InsertEndChild(TiXmlElement("permission"));
						if ( permissionNode!=NULL )
						{
							TiXmlNode *node = NULL;

							node = permissionNode->InsertEndChild(TiXmlElement("userGuid"));
							node->InsertEndChild(TiXmlText(iter->getUserGuid().c_str()));

							node = permissionNode->InsertEndChild(TiXmlElement("groupGuid"));
							node->InsertEndChild(TiXmlText(iter->getGroupGuid().c_str()));

							node = permissionNode->InsertEndChild(TiXmlElement("remoteAddress"));
							node->InsertEndChild(TiXmlText(iter->getRemoteAddress().c_str()));

							node = permissionNode->InsertEndChild(TiXmlElement("allowed"));
							node->InsertEndChild(TiXmlText(Util::ConvertUtil::toString(iter->isAllowed()).c_str()));
						}
					}
				}

				TiXmlNode *optionsNode = siteNode->InsertEndChild(TiXmlElement("options"));
				if ( optionsNode!=NULL )
				{
					const std::map<std::string,std::string> &options = iter->getOptions();
					std::map<std::string,std::string>::const_iterator optionIter;
					for ( optionIter=options.begin(); optionIter!=options.end(); optionIter++ ) {
						node = optionsNode->InsertEndChild(TiXmlElement("option"));
						node->ToElement()->SetAttribute("name",optionIter->first.c_str());
						node->InsertEndChild(TiXmlText(optionIter->second.c_str()));
					}
				}
			}
		}
	}

	if ( document.FirstChild()!=NULL && document.FirstChild()->ToDeclaration()==NULL ) {
		document.InsertBeforeChild(document.FirstChild(),TiXmlDeclaration("1.0","iso-8859-1","yes"));
	}

	if ( !document.SaveFile() ) {
		return 1;
	}

	return 0;
}

Site* SiteManager::findSiteByPath(const std::string &path)
{
	Site *site = NULL;

	// append a slash to path for correct matching
	std::string checkPath = path;
	if ( *(checkPath.end()-1)!='/' ) {
		checkPath += "/";
	}

	std::list<Site>::iterator iter;
	for ( iter=m_sites.begin(); iter!=m_sites.end(); iter++ )
	{
		// append a slash to site path for correct matching
		std::string sitePath = iter->getPath();
		if ( *(sitePath.end()-1)!='/' ) {
			sitePath += "/";
		}

		// make sure path isn't longer than site path
		if ( checkPath.length()<sitePath.length() ) {
			continue;
		}

		// make sure path matches site path
		if ( checkPath.substr(0,sitePath.length())!=sitePath ) {
			continue;
		}

		// check if this is a better match than any previous matches
		if ( site==NULL || site->getPath().length()<iter->getPath().length() ) {
			site = &(*iter);
		}
	}

	return site;
}
