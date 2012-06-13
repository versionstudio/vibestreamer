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

#ifndef guard_jsdatabaseconnection_h
#define guard_jsdatabaseconnection_h

#include <jsapi.h>

#include "../server/database.h"

/**
* JsDatabaseConnection.
* JavaScript representation of a database connection.
* */
class JsDatabaseConnection
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
	* @param privateData the private data for the instance
	* @return the new instance object
	*/
	static JSObject* jsInstance(JSContext *cx,JSObject *obj,DatabaseConnection *privateData);

	/**
	* Destructor callback. Called when an instance is destroyed.
	* @param cx the context where the object is allocated
	* @param obj the context object supplied at runtime
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
	* Execute a query with the returned result formatted as json.
	*/
	static JSBool executeJson(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Execute a query with no expected result.
	*/
	static JSBool executeNonQuery(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Execute a query with the returned result formatted as a string.
	*/
	static JSBool executeString(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Execute a query with the returned result formatted as xml.
	*/
	static JSBool executeXml(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Get the latest inserted row id by this connection.
	*/
	static JSBool getInsertId(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

private:
	/**
	 * Format the result from the reader as json.
	 * @param &reader the sqlite reader
	 * @param &result the stream that the result will be written to
	 */
	static void formatJson(sqlite3x::sqlite3_reader &reader,std::stringstream &result);

	/**
	 * Format the result from the reader as a string.
	 * @param &reader the sqlite reader
	 * @param &result the stream that the result will be written to
	 */
	static void formatString(sqlite3x::sqlite3_reader &reader,std::stringstream &result);

	/**
	 * Format the result from the reader as xml.
	 * @param &reader the sqlite reader
	 * @param &result the stream that the result will be written to
	 */
	static void formatXml(sqlite3x::sqlite3_reader &reader,std::stringstream &result);

	/**
	* Make a query result from the given string stream.
	* @param result the string stream containing the result
	* @param cx the context
	* @param rval the return variable to store the result in
	*/
	static void makeResult(const std::stringstream &result,JSContext *cx,jsval *rval);

	static JSClass m_jsClass;
	static JSFunctionSpec m_jsFunctionSpec[];
};

#endif
