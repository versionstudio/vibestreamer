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

#ifndef guard_jsuser_h
#define guard_jsuser_h

#include <jsapi.h>

#include "../server/user.h"

/**
* JsUser.
* JavaScript representation of the User class.
*/
class JsUser
{
public:
	/**
	* Initialize the js class and makes it accessible in the context.
	* @param cx the context from which to derive runtime information
	* @param obj the global object to use for initializing the class
	* @return the object that is the prototype for the newly initialized class
	*/
	static JSObject* jsInit(JSContext *cx,JSObject *obj);

	/**
	* Create a new instance of the class.
	* @param cx the context where the instance should be created
	* @param obj the context object supplied at runtime
	* @param user the user instance
	* @return the new instance object
	*/
	static JSObject* jsInstance(JSContext *cx,JSObject *obj,User &user);

	/**
	* Destructor callback. Called when an instance is destroyed.
	* @param cx the context from which to derive runtime information
	* @param obj the global object to use for initializing the class
	*/
	static void jsDestructor(JSContext *cx,JSObject *obj);
	
	/**
	* Get the js class descriptor.
	* @return the js class descriptor
	*/
	static JSClass *getJsClass() { 
		return &m_jsClass; 
	}

	/**
	* Get the database id for the user.
	*/
	static JSBool getDbId(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get the guid.
	*/
	static JSBool getGuid(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get the username.
	*/
	static JSBool getName(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get the last known ip this user connected from.
	*/
	static JSBool getLastKnownIp(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get the last time the user logged on.
	*/
	static JSBool getLastLoginTime(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get the total number of times this user has logged on.
	*/
	static JSBool getLogins(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get the option with the given name.
	*/
	static JSBool getOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get the names of all available options.
	*/
	static JSBool getOptionNames(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get the option with the given name.
	* This also includes any group memberships.
	*/
	static JSBool getRoleOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get the names of all available options.
	* This also includes any group memberships.
	*/
	static JSBool getRoleOptionNames(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get whether the user has admin privileges
	* This also includes any group memberships.
	*/
	static JSBool isRoleAdmin(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get whether the user only is allowed to browse shares and not download.
	* This also includes any group memberships.
	*/
	static JSBool isRoleBrowser(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Remove an option by name.
	*/
	static JSBool removeOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Set an option by name.
	*/
	static JSBool setOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

private:
	static JSClass m_jsClass;
	static JSFunctionSpec m_jsFunctionSpec[];
};

#endif
