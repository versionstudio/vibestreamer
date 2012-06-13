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
#include "jshttpserverrequest.h"

#include "contextprivate.h"
#include "engine.h"
#include "jsuser.h"

JSClass JsHttpServerRequest::m_jsClass = {
	"HttpServerRequest",
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

JSFunctionSpec JsHttpServerRequest::m_jsFunctionSpec[] = {
	{ "getAttribute",JsHttpServerRequest::getAttribute,0,NULL,NULL },
	{ "getMethod",JsHttpServerRequest::getMethod,0,NULL,NULL },
	{ "getParameter",JsHttpServerRequest::getParameter,1,NULL,NULL },
	{ "getParameterNames",JsHttpServerRequest::getParameterNames,0,NULL,NULL },
	{ "getRemoteAddress",JsHttpServerRequest::getRemoteAddress,0,NULL,NULL },
	{ "getUser",JsHttpServerRequest::getUser,0,NULL,NULL },
	{ "removeAttribute",JsHttpServerRequest::removeAttribute,1,NULL,NULL },
	{ "setAttribute",JsHttpServerRequest::setAttribute,2,NULL,NULL },
	{ NULL }
};

JSObject* JsHttpServerRequest::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsHttpServerRequest::m_jsClass,
		NULL,NULL,
		NULL,JsHttpServerRequest::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsHttpServerRequest::jsInstance(JSContext *cx,JSObject *obj)
{
	return JS_NewObject(cx,JsHttpServerRequest::getJsClass(),NULL,obj);
}

JSBool JsHttpServerRequest::getAttribute(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);

	std::string value;
	if ( cxPrivate->getHttpRequest().getAttribute(name,&value) ) {
		JSString *ret = JS_NewStringCopyN(cx,value.c_str(),value.length());		
		*rval = STRING_TO_JSVAL(ret);
	}
	else {
		*rval = JSVAL_NULL;
	}

	return JS_TRUE;
}

JSBool JsHttpServerRequest::getAttributeNames(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);

	JSObject *arr = JS_NewArrayObject(cx,0,NULL);
	if ( arr!=NULL )
	{
		int count=0;

		const std::map<std::string,std::string> &attributes = cxPrivate->getHttpRequest().getAttributes();
		std::map<std::string,std::string>::const_iterator iter;
		for ( iter=attributes.begin(); iter!=attributes.end(); iter++ )
		{
			JSString *str = JS_NewStringCopyN(cx,iter->first.c_str(),iter->first.length());
			jsval element = STRING_TO_JSVAL(str);
			if ( JS_SetElement(cx,arr,count,&element)==JS_TRUE ) {
				count++;
			}
		}

		*rval = OBJECT_TO_JSVAL(arr);
	}

	return JS_TRUE;
}

JSBool JsHttpServerRequest::getMethod(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);
	
	JSString *str = JS_NewStringCopyN(cx,cxPrivate->getHttpRequest().getMethod().c_str(),
		cxPrivate->getHttpRequest().getMethod().length());

	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsHttpServerRequest::getParameter(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);

	std::string param;
	if ( cxPrivate->getHttpRequest().getParameter(name,&param) ) {
		JSString *ret = JS_NewStringCopyN(cx,param.c_str(),param.length());		
		*rval = STRING_TO_JSVAL(ret);
	}
	else {
		*rval = JSVAL_NULL;
	}

	return JS_TRUE;
}

JSBool JsHttpServerRequest::getParameterNames(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);

	JSObject *arr = JS_NewArrayObject(cx,0,NULL);
	if ( arr!=NULL )
	{
		int count=0;

		const std::map<std::string,std::string> &parameters = cxPrivate->getHttpRequest().getParameters();
		std::map<std::string,std::string>::const_iterator iter;
		for ( iter=parameters.begin(); iter!=parameters.end(); iter++ )
		{
			JSString *str = JS_NewStringCopyN(cx,iter->first.c_str(),iter->first.length());
			jsval element = STRING_TO_JSVAL(str);
			if ( JS_SetElement(cx,arr,count,&element)==JS_TRUE ) {
				count++;
			}
		}

		*rval = OBJECT_TO_JSVAL(arr);
	}

	return JS_TRUE;
}

JSBool JsHttpServerRequest::getRemoteAddress(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);
	
	JSString *str = JS_NewStringCopyN(cx,cxPrivate->getHttpRequest().getRemoteAddress().c_str(),
		cxPrivate->getHttpRequest().getRemoteAddress().length());

	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsHttpServerRequest::getUser(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);

	User *user = cxPrivate->getHttpRequest().getUser();
	if ( user!=NULL ) {
		*rval = OBJECT_TO_JSVAL(JsUser::jsInstance(cx,obj,*user));
	}

	return JS_TRUE;
}

JSBool JsHttpServerRequest::removeAttribute(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);
	cxPrivate->getHttpRequest().removeAttribute(name);

	return JS_TRUE;
}

JSBool JsHttpServerRequest::setAttribute(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};
	char *value = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"ss",&name,&value) ) {
		return Engine::throwUsageError(cx,argv);
	}

	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);
	cxPrivate->getHttpRequest().setAttribute(name,std::string(value));

	return JS_TRUE;
}
