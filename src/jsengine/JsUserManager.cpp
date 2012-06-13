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
#include "jsusermanager.h"

#include "../server/usermanager.h"

#include "contextprivate.h"
#include "engine.h"
#include "jsgroup.h"
#include "jsuser.h"

JSClass JsUserManager::m_jsClass = {
	"UserManager",
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

JSFunctionSpec JsUserManager::m_jsFunctionSpec[] = {
	{ "findGroupByDbId",JsUserManager::findGroupByDbId,1,NULL,NULL },
	{ "findGroupByGuid",JsUserManager::findGroupByGuid,1,NULL,NULL },
	{ "findGroupByName",JsUserManager::findGroupByName,1,NULL,NULL },
	{ "findUserByDbId",JsUserManager::findUserByDbId,1,NULL,NULL },
	{ "findUserByGuid",JsUserManager::findUserByGuid,1,NULL,NULL },
	{ "findUserByName",JsUserManager::findUserByName,1,NULL,NULL },
	{ "getGroups",JsUserManager::getGroups,0,NULL,NULL },
	{ "getUsers",JsUserManager::getUsers,0,NULL,NULL },
	{ NULL }
};

JSObject* JsUserManager::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsUserManager::m_jsClass,
		NULL,NULL,
		NULL,JsUserManager::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsUserManager::jsInstance(JSContext *cx,JSObject *obj)
{
	return JS_NewObject(cx,JsUserManager::getJsClass(),NULL,obj);
}

JSBool JsUserManager::findGroupByDbId(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	uint64_t dbId = 0;

	if ( !JS_ConvertArguments(cx,argc,argv,"u",&dbId) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Group group;
	if ( UserManager::getInstance()->findGroupByDbId(dbId,&group) ) {
		*rval = OBJECT_TO_JSVAL(JsGroup::jsInstance(cx,obj,group));
	}

	return JS_TRUE;
}

JSBool JsUserManager::findGroupByGuid(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *guid = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&guid) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Group group;
	if ( UserManager::getInstance()->findGroupByGuid(guid,&group) ) {
		*rval = OBJECT_TO_JSVAL(JsGroup::jsInstance(cx,obj,group));
	}

	return JS_TRUE;
}

JSBool JsUserManager::findGroupByName(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Group group;
	if ( UserManager::getInstance()->findGroupByName(name,&group) ) {
		*rval = OBJECT_TO_JSVAL(JsGroup::jsInstance(cx,obj,group));
	}

	return JS_TRUE;
}

JSBool JsUserManager::findUserByDbId(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	uint64_t dbId = 0;

	if ( !JS_ConvertArguments(cx,argc,argv,"u",&dbId) ) {
		return Engine::throwUsageError(cx,argv);
	}

	User user;
	if ( UserManager::getInstance()->findUserByDbId(dbId,&user) ) {
		*rval = OBJECT_TO_JSVAL(JsUser::jsInstance(cx,obj,user));
	}

	return JS_TRUE;
}

JSBool JsUserManager::findUserByGuid(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *guid = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&guid) ) {
		return Engine::throwUsageError(cx,argv);
	}

	User user;
	if ( UserManager::getInstance()->findUserByGuid(guid,&user) ) {
		*rval = OBJECT_TO_JSVAL(JsUser::jsInstance(cx,obj,user));
	}

	return JS_TRUE;
}

JSBool JsUserManager::findUserByName(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	User user;
	if ( UserManager::getInstance()->findUserByName(name,&user) ) {
		*rval = OBJECT_TO_JSVAL(JsUser::jsInstance(cx,obj,user));
	}

	return JS_TRUE;
}

JSBool JsUserManager::getUsers(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	JSObject *arr = JS_NewArrayObject(cx,0,NULL);
	if ( arr!=NULL )
	{
		int count = 0;

		ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);

		std::list<User> users = UserManager::getInstance()->getUsers();
		std::list<User>::iterator iter;
		for ( iter=users.begin(); iter!=users.end(); iter++ )
		{
			jsval element = OBJECT_TO_JSVAL(JsUser::jsInstance(cx,obj,*iter));
			if ( JS_SetElement(cx,arr,count,&element)==JS_TRUE ) {
				count++;
			}
		}

		*rval = OBJECT_TO_JSVAL(arr);
	}

	return JS_TRUE;
}

JSBool JsUserManager::getGroups(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	JSObject *arr = JS_NewArrayObject(cx,0,NULL);
	if ( arr!=NULL )
	{
		int count = 0;

		ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);

		std::list<Group> groups = UserManager::getInstance()->getGroups();
		std::list<Group>::iterator iter;
		for ( iter=groups.begin(); iter!=groups.end(); iter++ )
		{
			jsval element = OBJECT_TO_JSVAL(JsGroup::jsInstance(cx,obj,*iter));
			if ( JS_SetElement(cx,arr,count,&element)==JS_TRUE ) {
				count++;
			}
		}

		*rval = OBJECT_TO_JSVAL(arr);
	}

	return JS_TRUE;
}
