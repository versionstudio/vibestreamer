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

#ifndef guard_jsfile_h
#define guard_jsfile_h

#include <jsapi.h>

/**
* JsFile.
* Represents a file or directory on disk within the site root.
* An instance of this class can be used for file operations.
*/
class JsFile
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
	* @param path the path to the file
	* @return the new instance object
	*/
	static JSObject* jsInstance(JSContext *cx,JSObject *obj,std::string &path);

	/**
	* Constructor callback. Called when an instance is constructed.
	* @param cx the context in which execution is taking place
	* @param obj the "this" object supplied to the function at execution time
	* @param argc the actual number of arguments supplied to the function
	* @param argv the arguments provided to the function
	* @param rval out parameter for the return value
	* @return JS_TRUE if call was successful
	*/
	static JSBool jsConstructor(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

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
	* Create a directory from the file path.
	* Any nonexistent parent directories will be created.
	*/
	static JSBool createDirectory(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Check whether the file exists.
	*/
	static JSBool exists(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get all files and directories that are children to the file.
	*/
	static JSBool getFiles(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get the name of the file.
	*/
	static JSBool getName(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get whether the file is a directory.
	*/
	static JSBool isDirectory(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get whether the file is a file.
	*/
	static JSBool isFile(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

private:
	static JSClass m_jsClass;
	static JSFunctionSpec m_jsFunctionSpec[];
};

#endif
