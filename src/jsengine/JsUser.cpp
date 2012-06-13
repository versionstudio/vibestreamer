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
#include "jsuser.h"

#include "../server/usermanager.h"

#include "engine.h"

JSClass JsUser::m_jsClass = {
	"User",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JsUser::jsDestructor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSFunctionSpec JsUser::m_jsFunctionSpec[] = {
	{ "getDbId",JsUser::getDbId,0,NULL,NULL },
	{ "getGuid",JsUser::getGuid,0,NULL,NULL },
	{ "getName",JsUser::getName,0,NULL,NULL },
	{ "getLastKnownIp",JsUser::getLastKnownIp,0,NULL,NULL },
	{ "getLastLoginTime",JsUser::getLastLoginTime,0,NULL,NULL },
	{ "getLogins",JsUser::getLogins,0,NULL,NULL },
	{ "getOption",JsUser::getOption,1,NULL,NULL },
	{ "getOptionNames",JsUser::getOptionNames,0,NULL,NULL },
	{ "getRoleOption",JsUser::getRoleOption,1,NULL,NULL },
	{ "getRoleOptionNames",JsUser::getRoleOptionNames,0,NULL,NULL },
	{ "isRoleAdmin",JsUser::isRoleAdmin,0,NULL,NULL },
	{ "isRoleBrowser",JsUser::isRoleBrowser,0,NULL,NULL },
	{ "removeOption",JsUser::removeOption,1,NULL,NULL },
	{ "setOption",JsUser::setOption,2,NULL,NULL },
	{ NULL }
};

JSObject* JsUser::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsUser::m_jsClass,
		NULL,NULL,
		NULL,JsUser::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsUser::jsInstance(JSContext *cx,JSObject *obj,User &user)
{
	JSObject *instance = JS_NewObject(cx,JsUser::getJsClass(),NULL,obj);
	JS_SetPrivate(cx,instance,new User(user));

	return instance;
}

void JsUser::jsDestructor(JSContext *cx,JSObject *obj)
{
	User *user = (User*)JS_GetPrivate(cx,obj);
	if ( user!=NULL ) {
		UserManager::getInstance()->updateUser(*user);
		delete user;
		JS_SetPrivate(cx,obj,NULL);
	}
}

JSBool JsUser::getDbId(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	User *user = (User*)JS_GetPrivate(cx,obj);	
	*rval = INT_TO_JSVAL(user->getDbId());

	return JS_TRUE;
}

JSBool JsUser::getGuid(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	User *user = (User*)JS_GetPrivate(cx,obj);
	
	JSString *str = JS_NewStringCopyN(cx,user->getGuid().c_str(),user->getGuid().length());
	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsUser::getName(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	User *user = (User*)JS_GetPrivate(cx,obj);

	JSString *str = JS_NewStringCopyN(cx,user->getName().c_str(),user->getName().length());
	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsUser::getLastKnownIp(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	User *user = (User*)JS_GetPrivate(cx,obj);

	JSString *str = JS_NewStringCopyN(cx,user->getLastKnownIp().c_str(),user->getLastKnownIp().length());
	*rval = STRING_TO_JSVAL(str);	

	return JS_TRUE;
}

JSBool JsUser::getLastLoginTime(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	User *user = (User*)JS_GetPrivate(cx,obj);
	
	JSObject *dateObj = js_NewDateObjectMsec(cx,user->getLastLoginTime()*1000.0);
	*rval = OBJECT_TO_JSVAL(dateObj);

	return JS_TRUE;
}

JSBool JsUser::getLogins(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	User *user = (User*)JS_GetPrivate(cx,obj);
	*rval = INT_TO_JSVAL(user->getLogins());

	return JS_TRUE;
}

JSBool JsUser::getOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	User *user = (User*)JS_GetPrivate(cx,obj);

	std::string value;
	if ( user->getOption(name,&value) ) {
		JSString *ret = JS_NewStringCopyN(cx,value.c_str(),value.length());		
		*rval = STRING_TO_JSVAL(ret);
	}
	else {
		*rval = JSVAL_NULL;
	}

	return JS_TRUE;
}

JSBool JsUser::getOptionNames(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	User *user = (User*)JS_GetPrivate(cx,obj);

	JSObject *arr = JS_NewArrayObject(cx,0,NULL);
	if ( arr!=NULL )
	{
		int count=0;

		const std::map<std::string,std::string> &options = user->getOptions();
		std::map<std::string,std::string>::const_iterator iter;
		for ( iter=options.begin(); iter!=options.end(); iter++ )
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

JSBool JsUser::getRoleOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	User *user = (User*)JS_GetPrivate(cx,obj);

	std::string value;
	if ( user->getRoleOption(name,&value) ) {
		JSString *ret = JS_NewStringCopyN(cx,value.c_str(),value.length());		
		*rval = STRING_TO_JSVAL(ret);
	}
	else {
		*rval = JSVAL_NULL;
	}

	return JS_TRUE;
}

JSBool JsUser::getRoleOptionNames(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	User *user = (User*)JS_GetPrivate(cx,obj);

	JSObject *arr = JS_NewArrayObject(cx,0,NULL);
	if ( arr!=NULL )
	{
		int count=0;

		const std::map<std::string,std::string> &options = user->getRoleOptions();
		std::map<std::string,std::string>::const_iterator iter;
		for ( iter=options.begin(); iter!=options.end(); iter++ )
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

JSBool JsUser::isRoleAdmin(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	User *user = (User*)JS_GetPrivate(cx,obj);
	*rval = BOOLEAN_TO_JSVAL(user->isRoleAdmin());

	return JS_TRUE;
}

JSBool JsUser::isRoleBrowser(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	User *user = (User*)JS_GetPrivate(cx,obj);
	*rval = BOOLEAN_TO_JSVAL(user->isRoleBrowser());

	return JS_TRUE;
}

JSBool JsUser::removeOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	User *user = (User*)JS_GetPrivate(cx,obj);
	user->removeOption(name);

	return JS_TRUE;
}

JSBool JsUser::setOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};
	char *value = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"ss",&name,&value) ) {
		return Engine::throwUsageError(cx,argv);
	}

	User *user = (User*)JS_GetPrivate(cx,obj);
	user->setOption(name,std::string(value));

	return JS_TRUE;
}
