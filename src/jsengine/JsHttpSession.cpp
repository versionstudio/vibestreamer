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
#include "jshttpsession.h"

#include "../server/httpserver.h"

#include "engine.h"

JSClass JsHttpSession::m_jsClass = {
	"Session",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JsHttpSession::jsDestructor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSFunctionSpec JsHttpSession::m_jsFunctionSpec[] = {
	{ "invalidate",JsHttpSession::invalidate,0,NULL,NULL },
	{ "getAttribute",JsHttpSession::getAttribute,0,NULL,NULL },
	{ "getCreationTime",JsHttpSession::getCreationTime,0,NULL,NULL },
	{ "getDbId",JsHttpSession::getDbId,0,NULL,NULL },
	{ "getGuid",JsHttpSession::getGuid,0,NULL,NULL },
	{ "getRemoteAddress",JsHttpSession::getRemoteAddress,0,NULL,NULL },
	{ "getUserGuid",JsHttpSession::getUserGuid,0,NULL,NULL },
	{ "removeAttribute",JsHttpSession::removeAttribute,1,NULL,NULL },
	{ "setAttribute",JsHttpSession::setAttribute,2,NULL,NULL },
	{ NULL }
};

JSObject* JsHttpSession::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsHttpSession::m_jsClass,
		NULL,NULL,
		NULL,JsHttpSession::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsHttpSession::jsInstance(JSContext *cx,JSObject *obj,HttpSession::Ptr sessionPtr)
{
	JSObject *instance = JS_NewObject(cx,JsHttpSession::getJsClass(),NULL,obj);
	JS_SetPrivate(cx,instance,new HttpSession::Ptr(sessionPtr));

	return instance;
}

void JsHttpSession::jsDestructor(JSContext *cx,JSObject *obj)
{
	HttpSession::Ptr *sessionPtr = (HttpSession::Ptr*)JS_GetPrivate(cx,obj);
	if ( sessionPtr!=NULL ) {
		delete sessionPtr;
		JS_SetPrivate(cx,obj,NULL);
	}
}

JSBool JsHttpSession::invalidate(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpSession::Ptr sessionPtr = *(HttpSession::Ptr*)JS_GetPrivate(cx,obj);
	HttpServer::getInstance()->getSessionManager().invalidateSession(sessionPtr);

	return JS_TRUE;
}

JSBool JsHttpSession::getAttribute(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	HttpSession::Ptr sessionPtr = *(HttpSession::Ptr*)JS_GetPrivate(cx,obj);

	std::string value;
	if ( sessionPtr->getAttribute(name,&value) ) {
		JSString *ret = JS_NewStringCopyN(cx,value.c_str(),value.length());		
		*rval = STRING_TO_JSVAL(ret);
	}
	else {
		*rval = JSVAL_NULL;
	}

	return JS_TRUE;
}

JSBool JsHttpSession::getCreationTime(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpSession::Ptr sessionPtr = *(HttpSession::Ptr*)JS_GetPrivate(cx,obj);
	JSObject *dateObj = js_NewDateObjectMsec(cx,sessionPtr->getCreationTime()*1000.0);
	*rval = OBJECT_TO_JSVAL(dateObj);

	return JS_TRUE;
}

JSBool JsHttpSession::getDbId(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpSession::Ptr sessionPtr = *(HttpSession::Ptr*)JS_GetPrivate(cx,obj);
	*rval = INT_TO_JSVAL(sessionPtr->getDbId());

	return JS_TRUE;
}

JSBool JsHttpSession::getGuid(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpSession::Ptr sessionPtr = *(HttpSession::Ptr*)JS_GetPrivate(cx,obj);
	JSString *str = JS_NewStringCopyN(cx,sessionPtr->getGuid().c_str(),sessionPtr->getGuid().length());
	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsHttpSession::getRemoteAddress(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpSession::Ptr sessionPtr = *(HttpSession::Ptr*)JS_GetPrivate(cx,obj);
	JSString *str = JS_NewStringCopyN(cx,sessionPtr->getRemoteAddress().c_str(),sessionPtr->getRemoteAddress().length());
	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsHttpSession::getUserGuid(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	HttpSession::Ptr sessionPtr = *(HttpSession::Ptr*)JS_GetPrivate(cx,obj);
	JSString *str = JS_NewStringCopyN(cx,sessionPtr->getUserGuid().c_str(),sessionPtr->getUserGuid().length());
	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsHttpSession::removeAttribute(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	HttpSession::Ptr sessionPtr = *(HttpSession::Ptr*)JS_GetPrivate(cx,obj);
	sessionPtr->removeAttribute(name);

	return JS_TRUE;
}

JSBool JsHttpSession::setAttribute(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};
	char *value = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"ss",&name,&value) ) {
		return Engine::throwUsageError(cx,argv);
	}

	HttpSession::Ptr sessionPtr = *(HttpSession::Ptr*)JS_GetPrivate(cx,obj);
	sessionPtr->setAttribute(name,std::string(value));

	return JS_TRUE;
}
