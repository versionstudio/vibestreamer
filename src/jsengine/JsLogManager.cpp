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
#include "jslogmanager.h"

#define LOGGER_CLASSNAME "JsLogManager"

#include "../server/logmanager.h"

#include "engine.h"

JSClass JsLogManager::m_jsClass = {
	"LogManager",
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

JSFunctionSpec JsLogManager::m_jsFunctionSpec[] = {
	{ "debug",JsLogManager::debug,1,NULL,NULL },
	{ "info",JsLogManager::info,1,NULL,NULL },
	{ "warning",JsLogManager::warning,1,NULL,NULL },
	{ "isDebug",JsLogManager::isDebug,0,NULL,NULL },
	{ NULL }
};

JSObject* JsLogManager::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsLogManager::m_jsClass,
		NULL,NULL,
		NULL,JsLogManager::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsLogManager::jsInstance(JSContext *cx,JSObject *obj)
{
	return JS_NewObject(cx,JsLogManager::getJsClass(),NULL,obj);
}

JSBool JsLogManager::debug(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *message = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&message) ) {
		return Engine::throwUsageError(cx,argv);
	}

	LogManager::getInstance()->debug(LOGGER_CLASSNAME,message);

	return JS_TRUE;
}

JSBool JsLogManager::info(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *message = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&message) ) {
		return Engine::throwUsageError(cx,argv);
	}

	LogManager::getInstance()->info(LOGGER_CLASSNAME,message);

	return JS_TRUE;
}

JSBool JsLogManager::warning(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *message = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&message) ) {
		return Engine::throwUsageError(cx,argv);
	}

	LogManager::getInstance()->warning(LOGGER_CLASSNAME,message);

	return JS_TRUE;
}

JSBool JsLogManager::isDebug(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	*rval = BOOLEAN_TO_JSVAL(LogManager::getInstance()->isDebug());

	return JS_TRUE;
}
