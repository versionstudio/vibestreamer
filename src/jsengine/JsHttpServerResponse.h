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

#ifndef guard_jshttpserverresponse_h
#define guard_jshttpserverresponse_h

#include <jsapi.h>

/**
* JsHttpServerResponse.
* JavaScript representation of the HttpServerResponse class.
*/
class JsHttpServerResponse
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
	* Write binary data as content.
	*/
	static JSBool binaryWrite(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Forces any content in the content body buffer to be written to the client.
	*/
	static JSBool flush(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Send a redirect response, telling the client to redirect to a new url.
	*/
	static JSBool redirect(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Sets the length of the content body in the response.
	* This method sets the HTTP Content-Length header.
	*/
	static JSBool setContentLength(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Sets the content type of the response being sent to the client.
	*/
	static JSBool setContentType(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Write text to the content body buffer.
	*/
	static JSBool write(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

	/**
	* Write text to the content body buffer, appending a line break to the end.
	*/
	static JSBool writeLine(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval);

private:
	static JSClass m_jsClass;
	static JSFunctionSpec m_jsFunctionSpec[];
};

#endif
