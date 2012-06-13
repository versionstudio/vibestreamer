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
#include "jshttpclientresponse.h"

#include "engine.h"

JSClass JsHttpClientResponse::m_jsClass = {
	"HttpClientResponse",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JsHttpClientResponse::jsDestructor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSFunctionSpec JsHttpClientResponse::m_jsFunctionSpec[] = {
	{ "getBody",JsHttpClientResponse::getBody,0,NULL,NULL },
	{ "getHeader",JsHttpClientResponse::getHeader,0,NULL,NULL },
	{ "getStatusCode",JsHttpClientResponse::getStatusCode,0,NULL,NULL },
    { NULL }
};

JSObject* JsHttpClientResponse::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsHttpClientResponse::m_jsClass,
		NULL,NULL,
		NULL,JsHttpClientResponse::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsHttpClientResponse::jsInstance(JSContext *cx,JSObject *obj,HttpClientResponse *privateData)
{
	JSObject *instance = JS_NewObject(cx,JsHttpClientResponse::getJsClass(),NULL,obj);
	JS_SetPrivate(cx,instance,privateData);

	return instance;
}

void JsHttpClientResponse::jsDestructor(JSContext *cx,JSObject *obj)
{
	HttpClientResponse *clientResponse = (HttpClientResponse*)JS_GetPrivate(cx,obj);
	if ( clientResponse!=NULL ) {
		delete clientResponse;
		JS_SetPrivate(cx,obj,NULL);
	}
}

JSBool JsHttpClientResponse::getBody(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpClientResponse *clientResponse = (HttpClientResponse*)JS_GetPrivate(cx,obj);
	
	JSString *str = JS_NewStringCopyN(cx,clientResponse->getBody().c_str(),
		clientResponse->getBody().length());

	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsHttpClientResponse::getHeader(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpClientResponse *clientResponse = (HttpClientResponse*)JS_GetPrivate(cx,obj);
	
	JSString *str = JS_NewStringCopyN(cx,clientResponse->getHeader().c_str(),
		clientResponse->getHeader().length());

	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsHttpClientResponse::getStatusCode(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpClientResponse *clientResponse = (HttpClientResponse*)JS_GetPrivate(cx,obj);
	*rval = INT_TO_JSVAL(clientResponse->getStatusCode());

	return JS_TRUE;
}
