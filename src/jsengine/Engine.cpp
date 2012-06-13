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
#include "engine.h"

#define LOGGER_CLASSNAME "Engine"

#include "../server/logmanager.h"

#include "contextprivate.h"
#include "jsdatabaseconnection.h"
#include "jsdatabasemanager.h"
#include "jsfile.h"
#include "jsgroup.h"
#include "jshttpserver.h"
#include "jshttpclientresponse.h"
#include "jshttpclientrequest.h"
#include "jshttpserverresponse.h"
#include "jshttpserverrequest.h"
#include "jshttpsession.h"
#include "jshttpsessionmanager.h"
#include "jsindexer.h"
#include "jslogmanager.h"
#include "jsmetadataimage.h"
#include "jsserver.h"
#include "jsshare.h"
#include "jssharemanager.h"
#include "jssite.h"
#include "jsuser.h"
#include "jsusermanager.h"

const int Engine::IO_BUFFER_SIZE = 8192;

JSClass globalClass = {
  "Global",JSCLASS_GLOBAL_FLAGS,
  JS_PropertyStub,  JS_PropertyStub,JS_PropertyStub, JS_PropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub,  JS_FinalizeStub
};

bool Engine::init()
{
	m_runtime = JS_NewRuntime(8L*1024L*1024L);
	if ( m_runtime==NULL ) {
		return false;
	}

	return true;
}

void Engine::cleanup()
{
	while ( !m_contexts.empty() )
	{
		JSContext *cx = m_contexts.top();
		m_contexts.pop();

		JS_ClearContextThread(cx);
		JS_DestroyContext(cx);
	}

	if ( m_runtime!=NULL ) {
		JS_DestroyRuntime(m_runtime);
		m_runtime = NULL;
	}

	JS_ShutDown();
}

bool Engine::executeFile(FILE *file,HttpServerRequest &httpRequest,HttpServerResponse &httpResponse)
{
	JSContext *cx = NULL;
	JSObject *obj = NULL;

	m_mutex.acquire_write();

	// pop any available context
	if ( !m_contexts.empty() ) {
		cx = m_contexts.top();
		m_contexts.pop();
	}

	m_mutex.release();

	if ( cx==NULL )
	{
		// create new context
		cx = JS_NewContext(m_runtime,8192);
		if ( cx==NULL ) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could not create context. Engine is unavailable.");
			return false;
		}

		JS_SetErrorReporter(cx,reportError);

		if ( LogManager::getInstance()->isDebug() ) {
			LogManager::getInstance()->debug(LOGGER_CLASSNAME,"Created new context");
		}
	}

	bool result = false;

	// begin request (requires js_threadsafe compilation)
	JS_SetContextThread(cx);
	JS_BeginRequest(cx);

	// create global object
	obj = JS_NewObject(cx,&globalClass,0,0);
	if ( obj==NULL ) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could not create global object");
	}
	else
	{
		// initialize standard classes on global object
		if ( JS_InitStandardClasses(cx,obj)==JS_FALSE ) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could not init standard classes");
		}
		else
		{
			// initialize custom classes on global object
			if ( !initCustomClasses(cx,obj) ) {
				LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Could not init custom classes");
			}
			else
			{
				// create predefined objects on global object
				createPredefinedObjects(cx,obj,httpRequest);

				// create context private
				ContextPrivate *cxPrivate = new ContextPrivate(this,httpRequest,httpResponse);
				JS_SetContextPrivate(cx,cxPrivate);

				// execute script in the context
				result = executeFile(file,cx,obj);

				// cleanup predefined objects on context object
				// note: this one might not be needed, remove later if ok
				cleanupPredefinedObjects(cx,obj);

				delete cxPrivate;
			}
		}
	}
	
	JS_GC(cx); // force garbage collection

	// end request (requires js_threadsafe)
	JS_EndRequest(cx);
	JS_ClearContextThread(cx);

	// return context to js engine
	m_mutex.acquire_write();
	m_contexts.push(cx);
	m_mutex.release();

	return result;
}

bool Engine::executeFile(FILE *file,JSContext *cx,JSObject *obj)
{
	jsval rval;
	uintN lineno = 0;

	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);

	std::string script;
	if ( !parseFile(file,cxPrivate->getHttpRequest(),script) ) {
		throwParseError(cx);
		return false;
	}

	JSBool result = JS_EvaluateScript(cx,obj,script.c_str(),(uintN)script.length(),
		Util::UriUtil::getLastSegment(cxPrivate->getHttpRequest().getUri()).c_str(),1,&rval);

	return result==JS_TRUE;
}

void Engine::throwParseError(JSContext *cx)
{
	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);

	std::string errorMsg = "Execution error on script parsing. Check for invalid directives.";
	cxPrivate->getHttpResponse().write(errorMsg.c_str(),errorMsg.length());
}

JSBool Engine::throwUsageError(JSContext* cx,jsval *argv)
{
	JSObject *functionObject = JSVAL_TO_OBJECT(argv[-2]);
	JSFunction *function = (JSFunction*)JS_GetPrivate(cx,functionObject);
	JSString *str = JS_GetFunctionId(function);
	
	char* functionName = JS_GetStringBytes(str);

	JS_ReportError(cx,"Invalid usage of %s",functionName);

	return JS_FALSE;
}

bool Engine::initCustomClasses(JSContext *cx,JSObject *obj)
{
	if ( JsDatabaseConnection::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsDatabaseManager::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsFile::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsGroup::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsHttpServer::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsHttpClientResponse::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsHttpClientRequest::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsHttpServerResponse::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsHttpServerRequest::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsHttpSession::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsHttpSessionManager::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsIndexer::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsLogManager::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsMetadataImage::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsServer::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsShare::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsShareManager::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsSite::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsUser::jsInit(cx,obj)==NULL ) {
		return false;
	}

	if ( JsUserManager::jsInit(cx,obj)==NULL ) {
		return false;
	}

	return true;
}

void Engine::createPredefinedObjects(JSContext *cx,JSObject *obj,HttpServerRequest &httpRequest)
{
	JSObject *predefinedObj = NULL;

	predefinedObj = JS_DefineObject(cx,obj,"response",JsHttpServerResponse::getJsClass(),NULL,JSPROP_PERMANENT|JSPROP_READONLY|JSPROP_ENUMERATE);
	if ( predefinedObj==NULL ) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to create predefined \"response\" object");
	}

	predefinedObj = JS_DefineObject(cx,obj,"request",JsHttpServerRequest::getJsClass(),NULL,JSPROP_PERMANENT|JSPROP_READONLY|JSPROP_ENUMERATE);
	if ( predefinedObj==NULL ) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to create predefined \"request\" object");
	}

	predefinedObj = JS_DefineObject(cx,obj,"server",JsServer::getJsClass(),NULL,JSPROP_PERMANENT|JSPROP_READONLY|JSPROP_ENUMERATE);
	if ( predefinedObj==NULL ) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to create predefined \"server\" object");
	}

	if ( httpRequest.getSession()!=NULL )
	{
		predefinedObj = JS_DefineObject(cx,obj,"session",JsHttpSession::getJsClass(),NULL,JSPROP_PERMANENT|JSPROP_READONLY|JSPROP_ENUMERATE);
		if ( predefinedObj==NULL ) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to create predefined \"session\" object");
		}
		else {
			JS_SetPrivate(cx,predefinedObj,new HttpSession::Ptr(httpRequest.getSession()));
		}
	}
	else
	{
		if ( JS_DefineProperty(cx,obj,"session",JSVAL_NULL,NULL,NULL,JSPROP_PERMANENT|JSPROP_READONLY|JSPROP_ENUMERATE)==JS_FALSE ) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to create predefined \"session\" object");
		}
	}

	if ( httpRequest.getSite()!=NULL )
	{
		predefinedObj = JS_DefineObject(cx,obj,"site",JsSite::getJsClass(),NULL,JSPROP_PERMANENT|JSPROP_READONLY|JSPROP_ENUMERATE);
		if ( predefinedObj==NULL ) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to create predefined \"site\" object");
		}
		else {
			JS_SetPrivate(cx,predefinedObj,httpRequest.getSite());
		}
	}
	else
	{
		if ( JS_DefineProperty(cx,obj,"site",JSVAL_NULL,NULL,NULL,JSPROP_PERMANENT|JSPROP_READONLY|JSPROP_ENUMERATE)==JS_FALSE ) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to create predefined \"site\" object");
		}
	}
}

void Engine::cleanupPredefinedObjects(JSContext *cx,JSObject *obj)
{
	if ( JS_DeleteProperty(cx,obj,"response")==JS_FALSE ) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to cleanup predefined \"response\" object");
	}

	if ( JS_DeleteProperty(cx,obj,"request")==JS_FALSE ) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to cleanup predefined \"request\" object");
	}

	if ( JS_DeleteProperty(cx,obj,"server")==JS_FALSE ) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to cleanup predefined \"server\" object");
	}

	if ( JS_DeleteProperty(cx,obj,"session")==JS_FALSE ) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to cleanup predefined \"session\" object");
	}

	if ( JS_DeleteProperty(cx,obj,"site")==JS_FALSE ) {
		LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to cleanup predefined \"site\" object");
	}
}

bool Engine::executeDirective(std::string directive,HttpServerRequest &httpRequest,
	std::string &output)
{
	std::string name;

	std::map<std::string,std::string> attributes;

	if ( !parseDirective(directive,name,attributes) ) {
		return false;
	}

	if ( name=="include" )
	{
		std::string fileUri = attributes["file"];

		if ( httpRequest.getSite()!=NULL && Util::UriUtil::isValid(fileUri) )
		{
			if ( !Util::UriUtil::isAbsolute(fileUri) ) {
				fileUri = Util::UriUtil::getParentSegment(httpRequest.getUri()) + "/" + fileUri;
			}

			std::string filePath = httpRequest.getSite()->getRealPath(fileUri);
			FILE *file = fopen(filePath.c_str(),"rb");
			if ( file!=NULL )
			{
				std::string script;
				if ( parseFile(file,httpRequest,script) ) {
					output = script;
				}

				fclose(file);
			}

			return true;
		}
	}

	return false;
}

bool Engine::parseFile(FILE *file,HttpServerRequest &httpRequest,std::string &script)
{
	std::string content;

	char *buf = new char[IO_BUFFER_SIZE+1];
	memset(buf,0,IO_BUFFER_SIZE);

	size_t bytesRead = 0;
	while ( (bytesRead = fread(buf,1,IO_BUFFER_SIZE,file))>0 ) {
		content.append(std::string(buf,bytesRead));
		memset(buf,0,IO_BUFFER_SIZE);
	}

	delete[] buf;

	std::string directive;
	std::string text;

	size_t pos = 0;
	size_t start = 0;
	size_t end = 0;

	int lineCount = 0;

	std::string::const_iterator iter = content.begin();
	while ( iter!=content.end() )
	{
		if ( *iter=='<' )
		{
			if ( *++iter=='?' )
			{
				if ( *++iter=='@' ) {
					directive.push_back(*++iter);
				}

				if ( !text.empty() )
				{
					script.append(makeResponse(text));
					for ( int i=0; i<lineCount; i++ ) {
						script.append("\r\n");
					}

					text.clear();
					lineCount = 0;
				}

				while ( iter!=content.end() )
				{
					if ( *iter=='?' )
					{
						if ( *++iter=='>' )
						{
							if ( !directive.empty() ) 
							{
								std::string output;
								if ( executeDirective(directive,httpRequest,output) ) {
									script.append(output);
									directive.clear();
								}
								else {
									return false;
								}
							}

							iter++;
							break;
						}
						else 
						{
							if ( !directive.empty() ) {
								directive.push_back('?');
							}
							else {
								script.push_back('?');
							}
						}
					}

					if ( !directive.empty() ) {
						directive.push_back(*iter++);
					}
					else {
						script.push_back(*iter++);
					}
				}

				if ( iter==content.end() ) {
					break;
				}

				continue;
			}
			else {
				iter--;
			}
		}

		if ( *iter=='\n' ) {
			lineCount++;
		}

		text.push_back(*iter++);
	}

	if ( !text.empty() ) {
		script.append(makeResponse(text));
	}
	else if ( !directive.empty() ) 
	{
		std::string output;
		if ( executeDirective(directive,httpRequest,output) ) {
			script.append(output);
		}
		else {
			return false;
		}
	}

	return true;
}

bool Engine::parseDirective(const std::string &directive,std::string &name,
	std::map<std::string,std::string> &attributes)
{
	std::string dir = boost::trim_copy(directive);

	size_t pos = dir.find(" ");
	if ( pos!=std::string::npos ) 
	{
		name = dir.substr(0,pos);
		
		size_t start = pos+1;
		while ( (pos=dir.find("=\"",start))!=std::string::npos ) 
		{
			std::string attribute = dir.substr(start,pos-start);
			
			start = pos+2;
			pos = dir.find('\"',start);
			if ( pos!=std::string::npos ) {
				attributes[attribute] = dir.substr(start,pos-start);
				start = pos+1;
			}
			else {
				return false;
			}
		}
	}
	else {
		name = dir;
	}

	return true;
}

std::string Engine::makeResponse(const std::string &msg)
{
	std::string escapedMsg = msg;

	boost::replace_all(escapedMsg,"\\","\\\\");
	boost::replace_all(escapedMsg,"\"","\\\"");
	boost::replace_all(escapedMsg,"\r","\\r");
	boost::replace_all(escapedMsg,"\n","\\n");
	
	return "response.write(\"" + escapedMsg + "\");";
}

void Engine::reportError(JSContext *cx,const char *msg,JSErrorReport *errorReport)
{
	ContextPrivate *cxPrivate = (ContextPrivate*)JS_GetContextPrivate(cx);

	char errorMsg[1024] = {0};
	sprintf(errorMsg,"Script Error in %s at line %u: '%s'",errorReport->filename,errorReport->lineno,msg);
	cxPrivate->getHttpResponse().write(errorMsg,strlen(errorMsg));
}
