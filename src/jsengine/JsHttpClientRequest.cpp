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
#include "jshttpclientrequest.h"

#include "../server/httpresponse.h"
#include "../server/httprequest.h"

#include "engine.h"
#include "jshttpclientresponse.h"

JSClass JsHttpClientRequest::m_jsClass = {
	"HttpClientRequest",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JsHttpClientRequest::jsDestructor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSFunctionSpec JsHttpClientRequest::m_jsFunctionSpec[] = {
	{ "execute",JsHttpClientRequest::execute,0,NULL,NULL },
	{ "getHost",JsHttpClientRequest::getHost,0,NULL,NULL },
	{ "getPort",JsHttpClientRequest::getPort,0,NULL,NULL },
	{ "getUri",JsHttpClientRequest::getUri,0,NULL,NULL },
	{ "setMethod",JsHttpClientRequest::setMethod,1,NULL,NULL },
	{ "setParameter",JsHttpClientRequest::setParameter,2,NULL,NULL },
	{ "setQueryString",JsHttpClientRequest::setQueryString,1,NULL,NULL },
    { NULL }
};

JSObject* JsHttpClientRequest::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsHttpClientRequest::m_jsClass,
		JsHttpClientRequest::jsConstructor,NULL,
		NULL,JsHttpClientRequest::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSBool JsHttpClientRequest::jsConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if ( !JS_IsConstructing(cx) ) {
		return Engine::throwUsageError(cx,argv);
	}

	char *host = {0};
	char *uri = {0};
	int port = 0;

	if ( !JS_ConvertArguments(cx,argc,argv,"ssi",&host,&uri,&port) ) {
		return Engine::throwUsageError(cx,argv);
	}

	HttpClientRequest *clientRequest = new HttpClientRequest(host,uri,port);
	JS_SetPrivate(cx,obj,clientRequest);

	return JS_TRUE;
}

void JsHttpClientRequest::jsDestructor(JSContext *cx,JSObject *obj)
{
	HttpClientRequest *clientRequest = (HttpClientRequest*)JS_GetPrivate(cx,obj);
	if ( clientRequest!=NULL ) {
		delete clientRequest;
		JS_SetPrivate(cx,obj,NULL);
	}
}

JSBool JsHttpClientRequest::execute(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpClientRequest *clientRequest = (HttpClientRequest*)JS_GetPrivate(cx,obj);
	HttpClientResponse *clientResponse = new HttpClientResponse();

	// make sure to suspend request on any action
	// that can cause a thread lock
	jsrefcount saveDepth = JS_SuspendRequest(cx);
	bool success = clientRequest->execute(*clientResponse);
	JS_ResumeRequest(cx,saveDepth);

	if ( success ) {
		*rval = OBJECT_TO_JSVAL(JsHttpClientResponse::jsInstance(cx,obj,clientResponse));
	}
	else {
		delete clientResponse;
	}

	return JS_TRUE;
}

JSBool JsHttpClientRequest::getHost(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpClientRequest *clientRequest = (HttpClientRequest*)JS_GetPrivate(cx,obj);
	
	JSString *str = JS_NewStringCopyN(cx,clientRequest->getHost().c_str(),
		clientRequest->getHost().length());

	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsHttpClientRequest::getPort(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpClientRequest *clientRequest = (HttpClientRequest*)JS_GetPrivate(cx,obj);
	*rval = INT_TO_JSVAL(clientRequest->getPort());

	return JS_TRUE;
}

JSBool JsHttpClientRequest::getUri(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpClientRequest *clientRequest = (HttpClientRequest*)JS_GetPrivate(cx,obj);
	
	JSString *str = JS_NewStringCopyN(cx,clientRequest->getUri().c_str(),
		clientRequest->getUri().length());

	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsHttpClientRequest::setMethod(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *method = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&method) ) {
		return Engine::throwUsageError(cx,argv);
	}
	
	HttpClientRequest *clientRequest = (HttpClientRequest*)JS_GetPrivate(cx,obj);
	clientRequest->setMethod(method);

	return JS_TRUE;
}

JSBool JsHttpClientRequest::setParameter(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};
	char *value = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"ss",&name,&value) ) {
		return Engine::throwUsageError(cx,argv);
	}

	HttpClientRequest *clientRequest = (HttpClientRequest*)JS_GetPrivate(cx,obj);
	clientRequest->setParameter(name,value);

	return JS_TRUE;
}

JSBool JsHttpClientRequest::setQueryString(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *queryString = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&queryString) ) {
		return Engine::throwUsageError(cx,argv);
	}

	HttpClientRequest *clientRequest = (HttpClientRequest*)JS_GetPrivate(cx,obj);
	clientRequest->setQueryString(queryString);

	return JS_TRUE;
}
