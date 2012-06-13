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
#include "jsserver.h"

#include "contextprivate.h"
#include "engine.h"
#include "jsdatabasemanager.h"
#include "jshttpserver.h"
#include "jsindexer.h"
#include "jslogmanager.h"
#include "jssharemanager.h"
#include "jsusermanager.h"

JSClass JsServer::m_jsClass = {
	"Server",
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

JSFunctionSpec JsServer::m_jsFunctionSpec[] = {
	{ "execute",JsServer::execute,1,NULL,NULL },
	{ "getDatabaseManager",JsServer::getDatabaseManager,0,NULL,NULL },
	{ "getHttpServer",JsServer::getHttpServer,0,NULL,NULL },
	{ "getIndexer",JsServer::getIndexer,0,NULL,NULL },
	{ "getLogManager",JsServer::getLogManager,0,NULL,NULL },
	{ "getShareManager",JsServer::getShareManager,0,NULL,NULL },
	{ "getUserManager",JsServer::getUserManager,0,NULL,NULL },
    { NULL }
};

JSObject* JsServer::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsServer::m_jsClass,
		NULL,NULL,
		NULL,JsServer::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsServer::jsInstance(JSContext *cx,JSObject *obj)
{
	return JS_NewObject(cx,JsServer::getJsClass(),NULL,obj);
}

JSBool JsServer::execute(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
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

		std::string filePath = httpRequest.getSite()->getRealPath(fileUri);
		FILE *file = fopen(filePath.c_str(),"rb");
		if ( file!=NULL ) {
			cxPrivate->getEngine()->executeFile(file,cx,obj);
			fclose(file);
			*rval = JSVAL_TRUE;
		}
		else {
			*rval = JSVAL_FALSE;
		}
	}
	else {
		*rval = JSVAL_FALSE;
	}

	return JS_TRUE;
}

JSBool JsServer::getDatabaseManager(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	*rval = OBJECT_TO_JSVAL(JsDatabaseManager::jsInstance(cx,obj));

	return JS_TRUE;
}

JSBool JsServer::getHttpServer(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	*rval = OBJECT_TO_JSVAL(JsHttpServer::jsInstance(cx,obj));

	return JS_TRUE;
}

JSBool JsServer::getIndexer(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	*rval = OBJECT_TO_JSVAL(JsIndexer::jsInstance(cx,obj));

	return JS_TRUE;
}

JSBool JsServer::getLogManager(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	*rval = OBJECT_TO_JSVAL(JsLogManager::jsInstance(cx,obj));

	return JS_TRUE;
}

JSBool JsServer::getShareManager(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	*rval = OBJECT_TO_JSVAL(JsShareManager::jsInstance(cx,obj));

	return JS_TRUE;
}

JSBool JsServer::getUserManager(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	*rval = OBJECT_TO_JSVAL(JsUserManager::jsInstance(cx,obj));

	return JS_TRUE;
}
