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

#ifndef guard_jssharemanager_h
#define guard_jssharemanager_h

#include <jsapi.h>

/**
* JsShareManager.
* JavaScript representation of the ShareManager class.
* */
class JsShareManager
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
	* @return the new instance object
	*/
	static JSObject* jsInstance(JSContext *cx,JSObject *obj);
	
	/**
	* Get the js class descriptor.
	* @return the js class descriptor
	*/
	static JSClass *getJsClass() { 
		return &m_jsClass; 
	}

	/**
	* Find a share by database id
	*/
	static JSBool findShareByDbId(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Find a share by guid
	*/
	static JSBool findShareByGuid(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Find a share by name
	*/
	static JSBool findShareByName(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get a collection of all shares.
	*/
	static JSBool getShares(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

private:
	static JSClass m_jsClass;
	static JSFunctionSpec m_jsFunctionSpec[];
};

#endif
