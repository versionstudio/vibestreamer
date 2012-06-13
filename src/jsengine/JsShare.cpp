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
#include "jsshare.h"

#include "../server/sharemanager.h"

#include "engine.h"
#include "jsuser.h"

JSClass JsShare::m_jsClass = {
	"Share",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JsShare::jsDestructor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSFunctionSpec JsShare::m_jsFunctionSpec[] = {
	{ "checkPermission",JsShare::checkPermission,2,NULL,NULL },
	{ "getDbId",JsShare::getDbId,0,NULL,NULL },
	{ "getDirectories",JsShare::getDirectories,0,NULL,NULL },
	{ "getFiles",JsShare::getFiles,0,NULL,NULL },
	{ "getGuid",JsShare::getGuid,0,NULL,NULL },
	{ "getName",JsShare::getName,0,NULL,NULL },
	{ NULL }
};

JSObject* JsShare::jsInstance(JSContext *cx,JSObject *obj,Share &privateData)
{
	JSObject *instance = JS_NewObject(cx,JsShare::getJsClass(),NULL,obj);
	JS_SetPrivate(cx,instance,new Share(privateData));

	return instance;
}

JSObject* JsShare::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsShare::m_jsClass,
		NULL,NULL,
		NULL,JsShare::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

void JsShare::jsDestructor(JSContext *cx,JSObject *obj)
{
	Share *share = (Share*)JS_GetPrivate(cx,obj);
	if ( share!=NULL ) {
		ShareManager::getInstance()->updateShare(*share);
		delete share;
		JS_SetPrivate(cx,obj,NULL);
	}
}

JSBool JsShare::checkPermission(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	if ( argc!=2 || !JSVAL_IS_OBJECT(argv[0]) || !JSVAL_IS_STRING(argv[1]) ) {
		return Engine::throwUsageError(cx,argv);
	}

	if ( !JS_InstanceOf(cx,JSVAL_TO_OBJECT(argv[0]),JsUser::getJsClass(),NULL) ) {
		return Engine::throwUsageError(cx,argv);
	}

	User *user = (User*)JS_GetPrivate(cx,JSVAL_TO_OBJECT(argv[0]));

	char *remoteAddress = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"/s",&remoteAddress) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Share *share = (Share*)JS_GetPrivate(cx,obj);
	*rval = BOOLEAN_TO_JSVAL(share->checkPermission(*user,remoteAddress));

	return JS_TRUE;
}

JSBool JsShare::getDbId(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Share *share = (Share*)JS_GetPrivate(cx,obj);
	*rval = INT_TO_JSVAL(share->getDbId());

	return JS_TRUE;
}

JSBool JsShare::getDirectories(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Share *share = (Share*)JS_GetPrivate(cx,obj);
	*rval = INT_TO_JSVAL(share->getDirectories());

	return JS_TRUE;
}

JSBool JsShare::getFiles(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Share *share = (Share*)JS_GetPrivate(cx,obj);
	*rval = INT_TO_JSVAL(share->getFiles());

	return JS_TRUE;
}

JSBool JsShare::getGuid(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Share *share = (Share*)JS_GetPrivate(cx,obj);
	JSString *str = JS_NewStringCopyN(cx,share->getGuid().c_str(),share->getGuid().length());
	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsShare::getName(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Share *share = (Share*)JS_GetPrivate(cx,obj);
	JSString *str = JS_NewStringCopyN(cx,share->getName().c_str(),share->getName().length());
	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}
