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
#include "jshttpserver.h"

#include "../server/httpserver.h"

#include "engine.h"
#include "jshttpsessionmanager.h"

JSClass JsHttpServer::m_jsClass = {
	"HttpServer",
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

JSFunctionSpec JsHttpServer::m_jsFunctionSpec[] = {
	{ "getSessionManager",JsHttpServer::getSessionManager,0,NULL,NULL },
	{ NULL }
};

JSObject* JsHttpServer::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsHttpServer::m_jsClass,
		NULL,NULL,
		NULL,JsHttpServer::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsHttpServer::jsInstance(JSContext *cx,JSObject *obj)
{
	return JS_NewObject(cx,JsHttpServer::getJsClass(),NULL,obj);
}

JSBool JsHttpServer::getSessionManager(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	*rval = OBJECT_TO_JSVAL(JsHttpSessionManager::jsInstance(cx,obj));

	return JS_TRUE;
}
