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
#include "jsdatabasemanager.h"

#include "../server/databasemanager.h"

#include "engine.h"
#include "jsdatabaseconnection.h"

JSClass JsDatabaseManager::m_jsClass = {
	"DatabaseManager",
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

JSFunctionSpec JsDatabaseManager::m_jsFunctionSpec[] = {
	{ "getConnection",JsDatabaseManager::getConnection,2,NULL,NULL },
	{ "releaseConnection",JsDatabaseManager::releaseConnection,1,NULL,NULL },
	{ NULL }
};

JSObject* JsDatabaseManager::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsDatabaseManager::m_jsClass,
		NULL,NULL,
		NULL,JsDatabaseManager::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsDatabaseManager::jsInstance(JSContext *cx,JSObject *obj)
{
	return JS_NewObject(cx,JsDatabaseManager::getJsClass(),NULL,obj);
}

JSBool JsDatabaseManager::getConnection(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};
	JSBool writeLock = JS_FALSE;

	if ( !JS_ConvertArguments(cx,argc,argv,"sb",&name,&writeLock) ) {
		return Engine::throwUsageError(cx,argv);
	}

	DatabaseConnection *connection = DatabaseManager::getInstance()->getConnection(name,writeLock);
	if ( connection!=NULL ) {
		*rval = OBJECT_TO_JSVAL(JsDatabaseConnection::jsInstance(cx,obj,connection));
	}

	return JS_TRUE;
}

JSBool JsDatabaseManager::releaseConnection(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	if ( argc!=1 || !JSVAL_IS_OBJECT(argv[0]) ) {
		return Engine::throwUsageError(cx,argv);
	}

	if ( !JS_InstanceOf(cx,JSVAL_TO_OBJECT(argv[0]),JsDatabaseConnection::getJsClass(),NULL) ) {
		return Engine::throwUsageError(cx,argv);
	}

	JsDatabaseConnection::jsDestructor(cx,JSVAL_TO_OBJECT(argv[0]));

	return JS_TRUE;
}
