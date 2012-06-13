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
#include "jssite.h"

#include "../server/sitemanager.h"

#include "engine.h"

JSClass JsSite::m_jsClass = {
	"Site",
	JSCLASS_HAS_PRIVATE,
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

JSFunctionSpec JsSite::m_jsFunctionSpec[] = {
	{ "getAttribute",JsSite::getAttribute,1,NULL,NULL },
	{ "getAttributeNames",JsSite::getAttributeNames,0,NULL,NULL },
	{ "getName",JsSite::getName,0,NULL,NULL },
	{ "getOption",JsSite::getOption,1,NULL,NULL },
	{ "getOptionNames",JsSite::getOptionNames,0,NULL,NULL },
	{ "getPath",JsSite::getPath,0,NULL,NULL },
	{ "removeAttribute",JsSite::removeAttribute,1,NULL,NULL },
	{ "removeOption",JsSite::removeOption,1,NULL,NULL },
	{ "setAttribute",JsSite::setAttribute,2,NULL,NULL },
	{ "setOption",JsSite::setOption,2,NULL,NULL },
	{ NULL }
};

JSObject* JsSite::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsSite::m_jsClass,
		NULL,NULL,
		NULL,JsSite::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsSite::jsInstance(JSContext *cx,JSObject *obj,Site *site)
{
	JSObject *instance = JS_NewObject(cx,JsSite::getJsClass(),NULL,obj);
	JS_SetPrivate(cx,instance,site);

	return instance;
}

JSBool JsSite::getAttribute(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Site *site = (Site*)JS_GetPrivate(cx,obj);

	std::string value;
	if ( site->getAttribute(name,&value) ) {
		JSString *ret = JS_NewStringCopyN(cx,value.c_str(),value.length());		
		*rval = STRING_TO_JSVAL(ret);
	}
	else {
		*rval = JSVAL_NULL;
	}

	return JS_TRUE;
}

JSBool JsSite::getAttributeNames(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Site *site = (Site*)JS_GetPrivate(cx,obj);

	JSObject *arr = JS_NewArrayObject(cx,0,NULL);
	if ( arr!=NULL )
	{
		int count=0;

		std::map<std::string,std::string> attributes = site->getAttributes();
		std::map<std::string,std::string>::const_iterator iter;
		for ( iter=attributes.begin(); iter!=attributes.end(); iter++ )
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

JSBool JsSite::getName(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Site *site = (Site*)JS_GetPrivate(cx,obj);
	JSString *str = JS_NewStringCopyN(cx,site->getName().c_str(),site->getName().length());
	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsSite::getOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Site *site = (Site*)JS_GetPrivate(cx,obj);

	std::string value;
	if ( site->getOption(name,&value) ) {
		JSString *ret = JS_NewStringCopyN(cx,value.c_str(),value.length());		
		*rval = STRING_TO_JSVAL(ret);
	}
	else {
		*rval = JSVAL_NULL;
	}

	return JS_TRUE;
}

JSBool JsSite::getOptionNames(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Site *site = (Site*)JS_GetPrivate(cx,obj);

	JSObject *arr = JS_NewArrayObject(cx,0,NULL);
	if ( arr!=NULL )
	{
		int count=0;

		std::map<std::string,std::string> options = site->getOptions();
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

JSBool JsSite::getPath(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	Site *site = (Site*)JS_GetPrivate(cx,obj);
	JSString *str = JS_NewStringCopyN(cx,site->getPath().c_str(),site->getPath().length());
	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsSite::removeAttribute(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Site *site = (Site*)JS_GetPrivate(cx,obj);
	site->removeAttribute(name);

	return JS_TRUE;
}

JSBool JsSite::removeOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&name) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Site *site = (Site*)JS_GetPrivate(cx,obj);
	site->removeOption(name);

	return JS_TRUE;
}

JSBool JsSite::setAttribute(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};
	char *value = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"ss",&name,&value) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Site *site = (Site*)JS_GetPrivate(cx,obj);
	site->setAttribute(name,std::string(value));

	return JS_TRUE;
}

JSBool JsSite::setOption(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *name = {0};
	char *value = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"ss",&name,&value) ) {
		return Engine::throwUsageError(cx,argv);
	}

	Site *site = (Site*)JS_GetPrivate(cx,obj);
	site->setOption(name,std::string(value));

	return JS_TRUE;
}
