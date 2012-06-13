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
#include "jshttpserverresponse.h"

#include "contextprivate.h"
#include "engine.h"

JSClass JsHttpServerResponse::m_jsClass = {
	"HttpServerResponse",
	NULL,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSFunctionSpec JsHttpServerResponse::m_jsFunctionSpec[] = {
	{ "binaryWrite",JsHttpServerResponse::binaryWrite,1,NULL,NULL },
	{ "flush",JsHttpServerResponse::flush,0,NULL,NULL },
	{ "redirect",JsHttpServerResponse::redirect,1,NULL,NULL },
	{ "setContentLength",JsHttpServerResponse::setContentLength,1,NULL,NULL },
	{ "setContentType",JsHttpServerResponse::setContentType,1,NULL,NULL },
	{ "write",JsHttpServerResponse::write,1,NULL,NULL },
	{ "writeLine",JsHttpServerResponse::writeLine,1,NULL,NULL },
    { NULL }
};

JSObject* JsHttpServerResponse::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsHttpServerResponse::m_jsClass,
		NULL,NULL,
		NULL,JsHttpServerResponse::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsHttpServerResponse::jsInstance(JSContext *cx,JSObject *obj)
{
	return JS_NewObject(cx,JsHttpServerResponse::getJsClass(),NULL,obj);
}

JSBool JsHttpServerResponse::binaryWrite(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	if ( !JSVAL_IS_STRING(argv[0]) ) {
		return Engine::throwUsageError(cx,argv);
	}

	JSString *data = JSVAL_TO_STRING(argv[0]);
	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);
	cxPrivate->getHttpResponse().write(JS_GetStringBytes(data),JS_GetStringLength(data));

	return JS_TRUE;
}

JSBool JsHttpServerResponse::flush(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);
	cxPrivate->getHttpResponse().flush();

	return JS_TRUE;
}

JSBool JsHttpServerResponse::write(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *output = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&output) ) {
		return Engine::throwUsageError(cx,argv);
	}

	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);
	cxPrivate->getHttpResponse().write(output,strlen(output));

	return JS_TRUE;
}

JSBool JsHttpServerResponse::writeLine(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	if ( argc!=1 ) {
		return Engine::throwUsageError(cx,argv);
	}

	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);
	JsHttpServerResponse::write(cx,obj,argc,argv,rval);
	cxPrivate->getHttpResponse().write("\n",1);

	return JS_TRUE;
}

JSBool JsHttpServerResponse::redirect(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *location = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&location) ) {
		return Engine::throwUsageError(cx,argv);
	}

	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);
	cxPrivate->getHttpResponse().redirect(location);

	return JS_TRUE;
}

JSBool JsHttpServerResponse::setContentLength(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	int contentLength = 0;

	if ( !JS_ConvertArguments(cx,argc,argv,"i",&contentLength) ) {
		return Engine::throwUsageError(cx,argv);
	}

	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);
	cxPrivate->getHttpResponse().setContentLength(contentLength);

	return JS_TRUE;
}

JSBool JsHttpServerResponse::setContentType(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *contentType = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&contentType) ) {
		return Engine::throwUsageError(cx,argv);
	}

	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);
	cxPrivate->getHttpResponse().setContentType(contentType);

	return JS_TRUE;
}
