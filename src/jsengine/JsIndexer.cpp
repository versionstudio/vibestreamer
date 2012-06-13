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
#include "jsindexer.h"

#include "../server/indexer.h"
#include "../utf8/utf8.h"

#include "engine.h"
#include "jsmetadataimage.h"

JSClass JsIndexer::m_jsClass = {
	"Indexer",
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

JSFunctionSpec JsIndexer::m_jsFunctionSpec[] = {
	{ "readMetadata",JsIndexer::readMetadata,1,NULL,NULL },
	{ "readMetadataImages",JsIndexer::readMetadataImages,1,NULL,NULL },
	{ NULL }
};

JSObject* JsIndexer::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsIndexer::m_jsClass,
		NULL,NULL,
		NULL,JsIndexer::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsIndexer::jsInstance(JSContext *cx,JSObject *obj)
{
	return JS_NewObject(cx,JsIndexer::getJsClass(),NULL,obj);
}

JSBool JsIndexer::readMetadata(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *path = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&path) ) {
		return Engine::throwUsageError(cx,argv);
	}

	// TODO: return a collection of all found metadata

	return JS_TRUE;
}

JSBool JsIndexer::readMetadataImages(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *path = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&path) ) {
		return Engine::throwUsageError(cx,argv);
	}

	std::string utf8Path = path; // the given path should be formatted as utf-8

	// convert utf-8 path to utf-16 since the indexer uses utf-16 internally
	std::wstring utf16Path;
	utf8::utf8to16(utf8Path.begin(),utf8Path.end(),back_inserter(utf16Path));
	
	JSObject *arr = JS_NewArrayObject(cx,0,NULL);
	if ( arr!=NULL )
	{
		int count = 0;

		std::list<MetadataImage::Ptr> images;
		Indexer::getInstance()->readMetadata(utf16Path,NULL,&images);
		for ( std::list<MetadataImage::Ptr>::iterator iter=images.begin(); 
			iter!=images.end(); iter++ )
		{
			jsval element = OBJECT_TO_JSVAL(JsMetadataImage::jsInstance(cx,obj,*iter));
			if ( JS_SetElement(cx,arr,count,&element)==JS_TRUE ) {
				count++;
			}
		}

		*rval = OBJECT_TO_JSVAL(arr);
	}

	return JS_TRUE;
}
