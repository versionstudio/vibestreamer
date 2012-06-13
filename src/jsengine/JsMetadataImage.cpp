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
#include "jsmetadataimage.h"

#include "contextprivate.h"
#include "engine.h"

JSClass JsMetadataImage::m_jsClass = {
	"MetadataImage",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JsMetadataImage::jsDestructor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSFunctionSpec JsMetadataImage::m_jsFunctionSpec[] = {
	{ "getData",JsMetadataImage::getData,0,NULL,NULL },
	{ "getMimeType",JsMetadataImage::getMimeType,0,NULL,NULL },
	{ "getSize",JsMetadataImage::getSize,0,NULL,NULL },
	{ NULL }
};

JSObject* JsMetadataImage::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsMetadataImage::m_jsClass,
		NULL,NULL,
		NULL,JsMetadataImage::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsMetadataImage::jsInstance(JSContext *cx,JSObject *obj,MetadataImage::Ptr imagePtr)
{
	JSObject *instance = JS_NewObject(cx,JsMetadataImage::getJsClass(),NULL,obj);
	JS_SetPrivate(cx,instance,new MetadataImage::Ptr(imagePtr));

	return instance;
}

void JsMetadataImage::jsDestructor(JSContext *cx,JSObject *obj)
{
	MetadataImage::Ptr *imagePtr = (MetadataImage::Ptr*)JS_GetPrivate(cx,obj);
	if ( imagePtr!=NULL ) {
		delete imagePtr;
		JS_SetPrivate(cx,obj,NULL);
	}
}

JSBool JsMetadataImage::getData(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	MetadataImage::Ptr imagePtr = *(MetadataImage::Ptr*)JS_GetPrivate(cx,obj);

	if ( imagePtr->getSize()>0 )
	{
		char *data = (char*)JS_malloc(cx,imagePtr->getSize()+1);
		if ( data!=NULL )
		{
			memcpy(data,imagePtr->getData(),imagePtr->getSize());
			JSString *ret = JS_NewString(cx,data,imagePtr->getSize());
			if ( ret!=NULL ) {					
				*rval = STRING_TO_JSVAL(ret);
			}
			else {
				JS_free(cx,data);
			}
		}
	}
	else {
		*rval = JSVAL_NULL;
	}

	return JS_TRUE;
}

JSBool JsMetadataImage::getMimeType(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	MetadataImage::Ptr imagePtr = *(MetadataImage::Ptr*)JS_GetPrivate(cx,obj);
	JSString *str = JS_NewStringCopyN(cx,imagePtr->getMimeType().c_str(),imagePtr->getMimeType().length());
	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool JsMetadataImage::getSize(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	MetadataImage::Ptr imagePtr = *(MetadataImage::Ptr*)JS_GetPrivate(cx,obj);
	*rval = INT_TO_JSVAL(imagePtr->getSize());

	return JS_TRUE;
}
