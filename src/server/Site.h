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

#ifndef guard_site_h
#define guard_site_h

#include "../tinyxml/tinyxml.h"

#include "accesslogger.h"
#include "permission.h"

class SiteManager; // forward declaration

/**
* ErrorPage.
* Represents an error page for a site.
*/
class ErrorPage
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	ErrorPage() {

	}

	/**
	* Constructor used for creating a new instance.
	* @param errorCode the error code for this error page.
	* The error code optionally supports a sub error code that is seperated with a dot.
	* For example: 403.1
	* @param message the error message
	* @param location the location of the error page to use instead of error message
	* @return instance
	*/
	ErrorPage(std::string errorCode,std::string message,std::string location) {
		m_errorCode = errorCode;
		m_message = message;
		m_location = location;
	}

	/**
	* Get the error code.
	* @return the error code
	*/
	const std::string& getErrorCode() const {
		return m_errorCode;
	}

	/**
	* Get the error message.
	* @return the error message
	*/
	const std::string& getMessage() const { 
		return m_message; 
	}

	/**
	* Get the location of the error page
	* @return the location of the error page
	*/
	const std::string& getLocation() const { 
		return m_location; 
	}

private:
	std::string m_errorCode;
	std::string m_message;
	std::string m_location;
};

/**
* Site.
* Represents a site context.
*/
class Site
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	Site() : m_anonymousAccess(false),
		m_accessLogger(NULL),
		m_authType("basic"),
		m_manager(NULL)
	{
	
	}

	/**
	* Load the site configuration.
	*/
	void loadConfig();

	/**
	* Check if a connected user has permission to this site.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @param user the connected user to check
	* @param remoteAddress the remote ip address the user is connected as
	* @return true if the user has permission
	*/
	bool checkPermission(const User &user,const std::string &remoteAddress);

	/**
	* Add a permission to the site.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @param permission the permission to add
	*/
	void addPermission(const Permission &permission);

	/**
	* Remove the attribute with the given name.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @param name the attribute to remove
	*/
	void removeAttribute(std::string name);

	/**
	* Remove the option with the given name.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @param name the option to remove
	*/
	void removeOption(std::string name);

	/**
	* Remove a permission to the site.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @param permission the permission to remove
	*/
	void removePermission(const Permission &permission);

	/**
	* Get the access logger instance.
	* @return the access logger instance or NULL if none exists
	*/
	AccessLogger* getAccessLogger() {
		return m_accessLogger;
	}

	/**
	* Get the guid of the user to be used for anonymous login.
	* @return the guid of the user to be used for anonymous login
	*/
	const std::string& getAnonymousUserGuid() const {
		return m_anonymousUserGuid; 
	}

	/**
	* Get the attribute with the given name.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @param name the attribute to retrieve
	* @param value out parameter for the found attribute value
	* @return true if the attribute was found
	*/	
	bool getAttribute(std::string name,std::string *value);

	/**
	* Get all attributes.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @return all options
	*/
	std::map<std::string,std::string> getAttributes();

	/**
	* Get the location of the authentication form page.
	* @return the location of the authentication form page
	*/
	const std::string& getAuthForm() const {
		return m_authForm; 
	}

	/**
	* Get the authentication realm.
	* @return the authentication realm
	*/
	const std::string& getAuthRealm() const {
		return m_authRealm; 
	}

	/**
	* Get the authentication type.
	* @return the authentication type
	*/
	const std::string& getAuthType() const {
		return m_authType; 
	}

	/**
	* Get all default documents.
	* @return a collection of all default documents
	*/
	const std::list<std::string>& getDefaultDocuments() const {
		return m_defaultDocuments;
	}

	/**
	* Get all error pages.
	* @return a collection of all error pages
	*/
	const std::list<ErrorPage>& getErrorPages() const {
		return m_errorPages;
	}

	/**
	* Get the mime mapping for the given extension.
	* @return the mime mapping for the given extension.
	* An empty string will be returned if no mime mapping was found
	*/
	const std::string getMimeMapping(const std::string &extension) const;

	/**
	* Get all mime mappings.
	* @return a collection of all mime mappings
	*/
	const std::map<std::string,std::string>& getMimeMappings() const {
		return m_mimeMappings;
	}

    /**
     * Get the manager within which this site is valid.
	 * @return the manager within which this site is valid
     */
	SiteManager* getManager() {
		return m_manager;
	}
	
	/**
	* Get the site name.
	* @return the site name
	*/
	const std::string& getName() const {
		return m_name;
	}

	/**
	* Get the option with the given name.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @param name the option to retrieve
	* @param value out parameter for the found option value
	* @return true if the option was found
	*/
	bool getOption(std::string name,std::string *value);

	/**
	* Get all options.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @return all options
	*/
	std::map<std::string,std::string> getOptions();

	/**
	* Get the site path.
	* The site path represents if the site context is mapped against a
	* path instead of the host root.
	* @return the site path, relative to the host root
	*/
	const std::string getPath() const {
		return m_path;
	}

	/**
	* Get all permission entries for the site.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @return a collection of all permission entries
	*/
	std::list<Permission> getPermissions();

	/**
	* Get the real path from a host path.
	* @param path the path relative to the host root
	* @return the real path
	*/
	const std::string getRealPath(const std::string &path) const;

	/**
	* Get the site root.
	* @return the site root
	*/
	const std::string& getRoot() const {
		return m_root;
	}

	/**
	* Get whether anonymous access is enabled.
	* @return true if anonymous access is enabled
	*/
	const bool isAnonymousAccess() const { 
		return m_anonymousAccess; 
	}

	/**
	* Set the access logger instance.
	* @param accessLogger the access logger instance
	*/
	void setAccessLogger(AccessLogger *accessLogger) {
		m_accessLogger = accessLogger;
	}

	/**
	* Set whether anonymous access should be enabled.
	* @param anonymousAccess true if anonymous access should be enabled
	*/
	void setAnonymousAccess(bool anonymousAccess) {
		m_anonymousAccess = anonymousAccess;
	}

	/**
	* Set the guid of the user to be used for anonymous login.
	* @param anonymousUserGuid the guid of the user to be used for anonymous login
	*/
	void setAnonymousUserGuid(std::string anonymousUserGuid) {
		m_anonymousUserGuid = anonymousUserGuid;
	}

	/**
	* Set the attribute with the given name.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @param the header name
	* @param the header value
	*/
	void setAttribute(std::string name,std::string value);

	/**
	* Set the location of the authentication form page.
	* @param authForm the location of the authentication form page
	*/
	void setAuthForm(std::string authForm) {
		m_authForm = authForm;
	}

	/**
	* Set the authentication realm.
	* @param authRealm the authentication realm
	*/
	void setAuthRealm(std::string authRealm) {
		m_authRealm = authRealm;
	}

	/**
	* Set the authentication type.
	* @param authType the authentication type
	*/
	void setAuthType(std::string authType) {
		m_authType = authType;
	}

	/**
	* Set the manager within which this site is valid.
	* @param manager the manager within which this site is valid
	*/
	void setManager(SiteManager *manager) {
		m_manager = manager;
	}
	
	/**
	* Set the site name.
	* @param name the site name
	*/
	void setName(std::string name) {
		m_name = name;
	}

	/**
	* Set the option with the given name.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @param name the option to set
	* @param value the value of the option
	*/
	void setOption(std::string name,std::string value);

	/**
	* Set all options.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @param options the options
	*/
	void setOptions(const std::map<std::string,std::string>& options);

	/**
	* Set the site path.
	* The site path represents if the site context should be mapped against a
	* path instead of the host root.
	* @param path the site path, relative to the host root
	*/
	void setPath(std::string path);

	/**
	* Set all permission entries for the site.
	* This method temporary locks the site using the
	* mutex pool owned by the manager.
	* @param permissions a collection of all permission entries
	*/
	void setPermissions(const std::list<Permission> &permissions);

	/**
	* Set the site root.
	* @param root the site root
	*/
	void setRoot(std::string root);

private:
	/**
	* Load a site configuration file.
	* @param path the path to the configuration file
	*/
	void loadConfigFile(const std::string &path);

	AccessLogger *m_accessLogger;

	SiteManager *m_manager;

	std::map<std::string,std::string> m_attributes;
	std::map<std::string,std::string> m_mimeMappings;
	std::map<std::string,std::string> m_options;

	std::list<Permission> m_permissions;

	std::list<ErrorPage> m_errorPages;

	std::list<std::string> m_defaultDocuments;

	std::string m_anonymousUserGuid;
	std::string m_authForm;
	std::string m_authRealm;
	std::string m_authType;
	std::string m_name;
	std::string m_path;
	std::string m_root;

	bool m_anonymousAccess;
};

#endif
