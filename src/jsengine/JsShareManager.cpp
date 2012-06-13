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
#include "jssharemanager.h"

#include "../server/sharemanager.h"
#include "../server/usermanager.h"

#include "engine.h"
#include "jsshare.h"

JSClass JsShareManager::m_jsClass = {
	"ShareManager",
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

JSFunctionSpec JsShareManager::m_jsFunctionSpec[] = {
	{ "findShareByDbId",JsShareManager::findShareByDbId,1,NULL,NULL },
	{ "findShareByGuid",JsShareManager::findShareByGuid,1,NULL,NULL },
	{ "findShareByName",JsShareManager::findShareByName,1,NULL,NULL },
	{ "getShares",JsShareManager::getShares,0,NULL,NULL },
	{ NULL }
};

JSObject* JsShareManager::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsShareManager::m_jsClass,
		NULL,NULL,
		NULL,JsShareManager::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsShareManager::jsInstance(JSContext *cx,JSObject *obj)
{
	return JS_NewObject(cx,JsShareManager::getJsClass(),NULL,obj);
}

JSBool JsShareManager::findShareByDbId(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	uint64_t dbId = 0;

	if ( !JS_ConvertArguments(cx,argc,argv,"u",&dbId) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Share share;
	if ( ShareManager::getInstance()->findShareByDbId(dbId,&share) ) {
		*rval = OBJECT_TO_JSVAL(JsShare::jsInstance(cx,obj,share));
	}

	return JS_TRUE;
}

JSBool JsShareManager::findShareByGuid(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *guid = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&guid) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Share share;
	if ( ShareManager::getInstance()->findShareByGuid(guid,&share) ) {
		*rval = OBJECT_TO_JSVAL(JsShare::jsInstance(cx,obj,share));
	}

	return JS_TRUE;
}

JSBool JsShareManager::findShareByName(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Share share;
	if ( ShareManager::getInstance()->findShareByName(name,&share) ) {
		*rval = OBJECT_TO_JSVAL(JsShare::jsInstance(cx,obj,share));
	}

	return JS_TRUE;
}

JSBool JsShareManager::getShares(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	JSObject *arr = JS_NewArrayObject(cx,0,NULL);
	if ( arr!=NULL )
	{
		int count = 0;

		std::list<Share> shares = ShareManager::getInstance()->getShares();
		std::list<Share>::iterator iter;
		for ( iter=shares.begin(); iter!=shares.end(); iter++ )
		{
			jsval element = OBJECT_TO_JSVAL(JsShare::jsInstance(cx,obj,*iter));
			if ( JS_SetElement(cx,arr,count,&element)==JS_TRUE ) {
				count++;
			}
		}

		*rval = OBJECT_TO_JSVAL(arr);
	}

	return JS_TRUE;
}
