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
#include "jsfile.h"

#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "contextprivate.h"
#include "engine.h"

JSClass JsFile::m_jsClass = {
	"File",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JsFile::jsDestructor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSFunctionSpec JsFile::m_jsFunctionSpec[] = {
	{ "createDirectory",JsFile::createDirectory,0,NULL,NULL },
	{ "exists",JsFile::exists,0,NULL,NULL },
	{ "getFiles",JsFile::getFiles,1,NULL,NULL },
	{ "getName",JsFile::getName,0,NULL,NULL },
	{ "isDirectory",JsFile::isDirectory,0,NULL,NULL },
	{ "isFile",JsFile::isFile,0,NULL,NULL },
    { NULL }
};

JSObject* JsFile::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsFile::m_jsClass,
		JsFile::jsConstructor,NULL,
		NULL,JsFile::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsFile::jsInstance(JSContext *cx,JSObject *obj,std::string &path)
{
	JSObject *instance = JS_NewObject(cx,JsFile::getJsClass(),NULL,obj);
	JS_SetPrivate(cx,instance,new std::string(path));

	return instance;
}

JSBool JsFile::jsConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if ( !JS_IsConstructing(cx) ) {
		return Engine::throwUsageError(cx,argv);
	}

	char *uri = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&uri) ) {
		return Engine::throwUsageError(cx,argv);
	}

	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);
	HttpServerRequest &httpRequest = cxPrivate->getHttpRequest();

	std::string fileUri = uri;
	if ( httpRequest.getSite()!=NULL && Util::UriUtil::isValid(fileUri) )
	{
		if ( !Util::UriUtil::isAbsolute(fileUri) ) {
			fileUri = Util::UriUtil::getParentSegment(httpRequest.getUri()) + "/" + fileUri;
		}

		std::string *filePath = new std::string(httpRequest.getSite()->getRealPath(fileUri));
		JS_SetPrivate(cx,obj,filePath);
	}
	else {
		return Engine::throwUsageError(cx,argv);
	}

	return JS_TRUE;
}

void JsFile::jsDestructor(JSContext *cx,JSObject *obj)
{
	std::string *filePath = (std::string*)JS_GetPrivate(cx,obj);
	if ( filePath!=NULL ) {
		delete filePath;
		JS_SetPrivate(cx,obj,NULL);
	}
}

JSBool JsFile::createDirectory(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	std::string *filePath = (std::string*)JS_GetPrivate(cx,obj);

	try
	{
		boost::filesystem::path boostPath(filePath->c_str(),boost::filesystem::native);
		if ( boost::filesystem::create_directory(boostPath) ) {
			*rval = JSVAL_TRUE;
		}
		else {
			*rval = JSVAL_FALSE;
		}
	}
	catch(boost::filesystem::filesystem_error error) {
		*rval = JSVAL_FALSE;
	}

	return JS_TRUE;
}

JSBool JsFile::exists(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	std::string *filePath = (std::string*)JS_GetPrivate(cx,obj);

	try
	{
		boost::filesystem::path boostPath(filePath->c_str(),boost::filesystem::native);
		if ( boost::filesystem::exists(boostPath) ) {
			*rval = JSVAL_TRUE;
		}
		else {
			*rval = JSVAL_FALSE;
		}
	}
	catch(boost::filesystem::filesystem_error error) {
		*rval = JSVAL_FALSE;
	}

	return JS_TRUE;
}

JSBool JsFile::getFiles(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *filePattern = {0};

	if ( argc==1 && !JS_ConvertArguments(cx,argc,argv,"s",&filePattern) ) {
		return Engine::throwUsageError(cx,argv);
	}

	std::string *filePath = (std::string*)JS_GetPrivate(cx,obj);

	try
	{
		boost::filesystem::path boostPath = boost::filesystem::path(*filePath,boost::filesystem::native);
		if ( boost::filesystem::exists(boostPath) ) 
		{
			JSObject *arr = JS_NewArrayObject(cx,0,NULL);
			if ( arr!=NULL )
			{
				int count = 0;

				boost::filesystem::directory_iterator endIter;
				for ( boost::filesystem::directory_iterator iter(boostPath); iter!=endIter; iter++ )
				{
					if ( filePattern!=NULL )
					{
						boost::regex filePatternRegex(filePattern);
						if ( !boost::regex_search(iter->leaf(),filePatternRegex) ) {
							continue;
						}
					}

					jsval element = OBJECT_TO_JSVAL(JsFile::jsInstance(cx,obj,iter->string()));
					if ( JS_SetElement(cx,arr,count,&element)==JS_TRUE ) {
						count++;
					}
				}

				*rval = OBJECT_TO_JSVAL(arr);
			}
		}
	}
	catch(boost::filesystem::filesystem_error error) {
	
	}

	return JS_TRUE;
}

JSBool JsFile::getName(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	std::string *filePath = (std::string*)JS_GetPrivate(cx,obj);
	boost::filesystem::path boostPath(*filePath,boost::filesystem::native);

	std::string name = boostPath.leaf();
	JSString *str = JS_NewStringCopyN(cx,name.c_str(),name.length());

	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsFile::isDirectory(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	std::string *filePath = (std::string*)JS_GetPrivate(cx,obj);

	try
	{
		boost::filesystem::path boostPath(filePath->c_str(),boost::filesystem::native);
		if ( boost::filesystem::is_directory(boostPath) ) {
			*rval = JSVAL_TRUE;
		}
		else {
			*rval = JSVAL_FALSE;
		}
	}
	catch(boost::filesystem::filesystem_error error) {
		*rval = JSVAL_FALSE;
	}

	return JS_TRUE;
}

JSBool JsFile::isFile(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	std::string *filePath = (std::string*)JS_GetPrivate(cx,obj);

	try
	{
		boost::filesystem::path boostPath(filePath->c_str(),boost::filesystem::native);
		if ( !boost::filesystem::is_directory(boostPath) ) {
			*rval = JSVAL_TRUE;
		}
		else {
			*rval = JSVAL_FALSE;
		}
	}
	catch(boost::filesystem::filesystem_error error) {
		*rval = JSVAL_FALSE;
	}

	return JS_TRUE;
}
