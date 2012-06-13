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
#include "jsgroup.h"

#include "../server/usermanager.h"

#include "engine.h"

JSClass JsGroup::m_jsClass = {
	"Group",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JsGroup::jsDestructor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSFunctionSpec JsGroup::m_jsFunctionSpec[] = {
	{ "getDbId",JsGroup::getDbId,0,NULL,NULL },
	{ "getGuid",JsGroup::getGuid,0,NULL,NULL },
	{ "getName",JsGroup::getName,0,NULL,NULL },
	{ "getOption",JsGroup::getOption,1,NULL,NULL },
	{ "getOptionNames",JsGroup::getOptionNames,0,NULL,NULL },
	{ "removeOption",JsGroup::removeOption,1,NULL,NULL },
	{ "setOption",JsGroup::setOption,2,NULL,NULL },
	{ NULL }
};

JSObject* JsGroup::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsGroup::m_jsClass,
		NULL,NULL,
		NULL,JsGroup::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsGroup::jsInstance(JSContext *cx,JSObject *obj,Group &group)
{
	JSObject *instance = JS_NewObject(cx,JsGroup::getJsClass(),NULL,obj);
	JS_SetPrivate(cx,instance,new Group(group));

	return instance;
}

void JsGroup::jsDestructor(JSContext *cx,JSObject *obj)
{
	Group *group = (Group*)JS_GetPrivate(cx,obj);
	if ( group!=NULL ) {
		UserManager::getInstance()->updateGroup(*group);
		delete group;
		JS_SetPrivate(cx,obj,NULL);
	}
}

JSBool JsGroup::getDbId(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Group *group = (Group*)JS_GetPrivate(cx,obj);	
	*rval = INT_TO_JSVAL(group->getDbId());

	return JS_TRUE;
}

JSBool JsGroup::getGuid(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Group *group = (Group*)JS_GetPrivate(cx,obj);
	
	JSString *str = JS_NewStringCopyN(cx,group->getGuid().c_str(),group->getGuid().length());
	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsGroup::getName(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Group *group = (Group*)JS_GetPrivate(cx,obj);

	JSString *str = JS_NewStringCopyN(cx,group->getName().c_str(),group->getName().length());
	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsGroup::getOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Group *group = (Group*)JS_GetPrivate(cx,obj);

	std::string value;
	if ( group->getOption(name,&value) ) {
		JSString *ret = JS_NewStringCopyN(cx,value.c_str(),value.length());		
		*rval = STRING_TO_JSVAL(ret);
	}
	else {
		*rval = JSVAL_NULL;
	}

	return JS_TRUE;
}

JSBool JsGroup::getOptionNames(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Group *group = (Group*)JS_GetPrivate(cx,obj);

	JSObject *arr = JS_NewArrayObject(cx,0,NULL);
	if ( arr!=NULL )
	{
		int count=0;

		const std::map<std::string,std::string> &options = group->getOptions();
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

JSBool JsGroup::removeOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Group *group = (Group*)JS_GetPrivate(cx,obj);
	group->removeOption(name);

	return JS_TRUE;
}

JSBool JsGroup::setOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};
	char *value = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"ss",&name,&value) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Group *group = (Group*)JS_GetPrivate(cx,obj);
	group->setOption(name,std::string(value));

	return JS_TRUE;
}
