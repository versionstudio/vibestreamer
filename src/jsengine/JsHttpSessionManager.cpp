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
#include "jshttpsessionmanager.h"

#include "../server/httpserver.h"

#include "engine.h"
#include "jshttpsession.h"

JSClass JsHttpSessionManager::m_jsClass = {
	"HttpSessionManager",
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

JSFunctionSpec JsHttpSessionManager::m_jsFunctionSpec[] = {
	{ "findSessionByGuid",JsHttpSessionManager::findSessionByGuid,1,NULL,NULL },
	{ "getSessions",JsHttpSessionManager::getSessions,0,NULL,NULL },
	{ NULL }
};

JSObject* JsHttpSessionManager::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsHttpSessionManager::m_jsClass,
		NULL,NULL,
		NULL,JsHttpSessionManager::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsHttpSessionManager::jsInstance(JSContext *cx,JSObject *obj)
{
	return JS_NewObject(cx,JsHttpSessionManager::getJsClass(),NULL,obj);
}

JSBool JsHttpSessionManager::findSessionByGuid(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *guid = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&guid) ) {
		return Engine::throwUsageError(cx,argv);
	}

	HttpSession::Ptr sessionPtr = HttpServer::getInstance()->getSessionManager().findSessionByGuid(guid);
	if ( sessionPtr!=NULL ) {
		*rval = OBJECT_TO_JSVAL(JsHttpSession::jsInstance(cx,obj,sessionPtr));
	}

	return JS_TRUE;
}

JSBool JsHttpSessionManager::getSessions(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	JSObject *arr = JS_NewArrayObject(cx,0,NULL);
	if ( arr!=NULL )
	{
		int count=0;

		std::vector<HttpSession::Ptr> sessions = HttpServer::getInstance()->getSessionManager().getSessions();
		std::vector<HttpSession::Ptr>::iterator iter;
		for ( iter=sessions.begin(); iter!=sessions.end(); iter++ ) 
		{
			jsval element = OBJECT_TO_JSVAL(JsHttpSession::jsInstance(cx,obj,*iter));
			if ( JS_SetElement(cx,arr,count,&element)==JS_TRUE ) {
				count++;
			}
		}

		*rval = OBJECT_TO_JSVAL(arr);
	}

	return JS_TRUE;
}
