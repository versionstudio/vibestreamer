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
#include "site.h"

#include "sitemanager.h"

void Site::loadConfig()
{
	// clear loaded config
	m_defaultDocuments.clear();
	m_errorPages.clear();
	m_mimeMappings.clear();

	// load the global config
	loadConfigFile("site.xml");

	// load the local config
	loadConfigFile(m_root + "\\_private\\site.xml");
}

void Site::loadConfigFile(const std::string &path)
{
	TiXmlDocument document;

	if ( document.LoadFile(path.c_str()) )
	{
		TiXmlNode *siteNode = document.FirstChildElement("site");
		if ( siteNode!=NULL )
		{
			TiXmlNode *defaultDocumentsNode = siteNode->FirstChildElement("defaultDocuments");
			if ( defaultDocumentsNode!=NULL )
			{
				TiXmlNode *defaultDocumentNode = defaultDocumentsNode->FirstChildElement("defaultDocument");
				while ( defaultDocumentNode!=NULL ) 
				{
					if ( defaultDocumentNode->FirstChild()!=NULL ) 
					{
						std::string name = defaultDocumentNode->FirstChild()->Value();
						if ( !name.empty() )
						{
							// remove any existing default document with this name
							std::list<std::string>::iterator iter;
							for ( iter=m_defaultDocuments.begin(); iter!=m_defaultDocuments.end(); iter++ ) {
								if ( *iter==name ) {
									m_defaultDocuments.erase(iter);
									break;
								}
							}

							m_defaultDocuments.push_back(name);
						}
					}
					
					defaultDocumentNode = defaultDocumentsNode->IterateChildren("defaultDocument",defaultDocumentNode);
				}
			}

			TiXmlNode *errorPagesNode = siteNode->FirstChildElement("errorPages");
			if ( errorPagesNode!=NULL )
			{
				TiXmlNode *errorPageNode = errorPagesNode->FirstChildElement("errorPage");
				while ( errorPageNode!=NULL ) 
				{
					std::string errorCode;
					std::string message;
					std::string location;

					if ( errorPageNode->FirstChildElement("errorCode") && errorPageNode->FirstChildElement("errorCode")->FirstChild() ) {
						errorCode = errorPageNode->FirstChildElement("errorCode")->FirstChild()->Value();
					}

					if ( errorPageNode->FirstChildElement("message") && errorPageNode->FirstChildElement("message")->FirstChild() ) {
						message = errorPageNode->FirstChildElement("message")->FirstChild()->Value();
					}

					if ( errorPageNode->FirstChildElement("location") && errorPageNode->FirstChildElement("location")->FirstChild() ) {
						location = errorPageNode->FirstChildElement("location")->FirstChild()->Value();
					}

					if ( !errorCode.empty() ) 
					{
						// remove any existing error page with this error code
						std::list<ErrorPage>::iterator iter;
						for ( iter=m_errorPages.begin(); iter!=m_errorPages.end(); iter++ ) {
							if ( iter->getErrorCode()==errorCode ) {
								m_errorPages.erase(iter);
								break;
							}
						}

						m_errorPages.push_back(ErrorPage(errorCode,message,location));
					}

					errorPageNode = errorPagesNode->IterateChildren("errorPage",errorPageNode);
				}
			}

			TiXmlNode *mimeMappingsNode = siteNode->FirstChildElement("mimeMappings");
			if ( mimeMappingsNode!=NULL )
			{
				TiXmlNode *mimeMappingNode = mimeMappingsNode->FirstChildElement("mimeMapping");
				while ( mimeMappingNode!=NULL ) 
				{
					std::string extension;
					std::string mimeType;

					if ( mimeMappingNode->FirstChildElement("extension") && mimeMappingNode->FirstChildElement("extension")->FirstChild() ) {
						extension = mimeMappingNode->FirstChildElement("extension")->FirstChild()->Value();
					}

					if ( mimeMappingNode->FirstChildElement("mimeType") && mimeMappingNode->FirstChildElement("mimeType")->FirstChild() ) {
						mimeType = mimeMappingNode->FirstChildElement("mimeType")->FirstChild()->Value();
					}

					m_mimeMappings[extension] = mimeType;

					mimeMappingNode = mimeMappingsNode->IterateChildren("mimeMapping",mimeMappingNode);
				}
			}
		}
	}
}

bool Site::checkPermission(const User &user,const std::string &remoteAddress) 
{
	bool hasPermission = false;

	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	hasPermission = Permission::checkPermission(m_permissions,user,remoteAddress);

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}

	return hasPermission;
}

void Site::addPermission(const Permission &permission) 
{
	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	m_permissions.push_back(permission);

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}
}

void Site::removeAttribute(std::string name)
{
	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	std::map<std::string,std::string>::iterator iter = m_attributes.find(name);
	if ( iter!=m_attributes.end() ) {
		m_attributes.erase(iter);
	}

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}
}

void Site::removeOption(std::string name)
{
	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	std::map<std::string,std::string>::iterator iter = m_options.find(name);
	if ( iter!=m_options.end() ) {
		m_options.erase(iter);
	}

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}
}

void Site::removePermission(const Permission &permission) 
{
	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	std::list<Permission>::iterator iter;
	for ( iter=m_permissions.begin(); iter!=m_permissions.end(); iter++ ) 
	{
		if ( iter->equals(permission) ) {
			m_permissions.erase(iter);
			break;
		}
	}

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}
}

bool Site::getAttribute(std::string name,std::string *value)
{
	bool foundAttribute = false;

	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	std::map<std::string,std::string>::iterator iter = m_attributes.find(name);
	if ( iter!=m_attributes.end() ) 
	{
		if ( value!=NULL ) {
			*value = iter->second;
		}

		foundAttribute = true;
	}

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}
	
	return foundAttribute;
}

std::map<std::string,std::string> Site::getAttributes()
{
	std::map<std::string,std::string> attributes;

	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	attributes = m_attributes;

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}

	return attributes;
}

const std::string Site::getMimeMapping(const std::string &extension) const
{
	std::map<std::string,std::string>::const_iterator iter;
	for ( iter=m_mimeMappings.begin(); iter!=m_mimeMappings.end(); iter++ ) {
		if ( boost::iequals(extension,iter->first) ) {
			return iter->second;
		}
	}

	return "text/plain"; // default mime type
}

bool Site::getOption(std::string name,std::string *value)
{
	bool foundOption = false;

	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	std::map<std::string,std::string>::iterator iter;
	iter = m_options.find(name);
	if ( iter!=m_options.end() ) 
	{
		if ( value!=NULL ) {
			*value = iter->second;
		}
		
		foundOption = true;
	}

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}

	return foundOption;
}

std::map<std::string,std::string> Site::getOptions()
{
	std::map<std::string,std::string> options;

	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	options = m_options;

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}

	return options;
}

std::list<Permission> Site::getPermissions() 
{
	std::list<Permission> permissions;

	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	permissions = m_permissions;

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}

	return permissions;
}

const std::string Site::getRealPath(const std::string &path) const
{
	return boost::replace_all_copy(m_root+path,"/","\\");
}

void Site::setAttribute(std::string name,std::string value) 
{
	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	m_attributes[name] = value;

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}
}

void Site::setOption(std::string name,std::string value) 
{
	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	m_options[name] = value;

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}
}

void Site::setOptions(const std::map<std::string,std::string>& options)
{
	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	m_options = options;

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}
}

void Site::setPath(std::string path)
{
	if ( !path.empty() )
	{
		// make sure path begins with a slash
		if ( *path.begin()!='/' ) {
			path = "/" + path;
		}

		// make sure path does not end with a slash
		if ( *(path.end()-1)=='/' && (path.end()-1)!=path.begin() ) {
			path = path.substr(0,path.length()-1);
		}
	}

	m_path = path;
}

void Site::setPermissions(const std::list<Permission> &permissions) 
{
	if ( m_manager!=NULL ) {
		m_manager->lockSite(this);
	}

	m_permissions = permissions;

	if ( m_manager!=NULL ) {
		m_manager->releaseSite(this);
	}
}

void Site::setRoot(std::string root)
{
	// make sure root does not end with a slash
	if ( !root.empty() ) {
		if ( *(root.end()-1)=='\\' && (root.end()-1)!=root.begin() ) {
			root = root.substr(0,root.length()-1);
		}
	}

	m_root = root;
}
