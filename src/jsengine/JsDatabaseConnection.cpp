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
#include "jsdatabaseconnection.h"

#define LOGGER_CLASSNAME "JsDatabaseConnection"

#include "../server/databasemanager.h"
#include "../server/logmanager.h"

#include "engine.h"

JSClass JsDatabaseConnection::m_jsClass = {
	"DatabaseConnection",
	NULL,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JsDatabaseConnection::jsDestructor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSFunctionSpec JsDatabaseConnection::m_jsFunctionSpec[] = {
	{ "executeJson",JsDatabaseConnection::executeJson,1,NULL,NULL },
	{ "executeNonQuery",JsDatabaseConnection::executeNonQuery,1,NULL,NULL },
	{ "executeString",JsDatabaseConnection::executeString,1,NULL,NULL },
	{ "executeXml",JsDatabaseConnection::executeXml,1,NULL,NULL },
	{ "getInsertId",JsDatabaseConnection::getInsertId,0,NULL,NULL },
	{ NULL }
};

JSObject* JsDatabaseConnection::jsInit(JSContext *cx,JSObject *obj)
{
	JSObject *prototypeObj = JS_InitClass(cx,obj,NULL,&JsDatabaseConnection::m_jsClass,
		NULL,NULL,
		NULL,JsDatabaseConnection::m_jsFunctionSpec,
		NULL,NULL);

	return prototypeObj;
}

JSObject* JsDatabaseConnection::jsInstance(JSContext *cx,JSObject *obj,DatabaseConnection *privateData)
{
	JSObject *instance = JS_NewObject(cx,JsDatabaseConnection::getJsClass(),NULL,obj);
	JS_SetPrivate(cx,instance,privateData);

	return instance;
}

void JsDatabaseConnection::jsDestructor(JSContext *cx,JSObject *obj)
{
	DatabaseConnection *conn = (DatabaseConnection*)JS_GetPrivate(cx,obj);
	if ( conn!=NULL ) {
		DatabaseManager::getInstance()->releaseConnection(conn);
		JS_SetPrivate(cx,obj,NULL);
	}
}

JSBool JsDatabaseConnection::executeJson(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *query = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&query) ) {
		return Engine::throwUsageError(cx,argv);
	}

	DatabaseConnection *conn = (DatabaseConnection*)JS_GetPrivate(cx,obj);
	if ( conn!=NULL ) 
	{
		try
		{
			sqlite3x::sqlite3_command cmd(conn->getSqliteConn(),query);
			sqlite3x::sqlite3_reader reader = cmd.executereader();

			std::stringstream result;
			formatJson(reader,result);
			makeResult(result,cx,rval);
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to execute query [%s]",ex.what());
		}
	}

	return JS_TRUE;
}

JSBool JsDatabaseConnection::executeNonQuery(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *query = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&query) ) {
		return Engine::throwUsageError(cx,argv);
	}

	DatabaseConnection *conn = (DatabaseConnection*)JS_GetPrivate(cx,obj);
	if ( conn!=NULL ) 
	{
		bool success = false;

		try {
			conn->getSqliteConn().executenonquery(query);
			success = true;
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to execute query [%s]",ex.what());
		}

		*rval = BOOLEAN_TO_JSVAL(success);
	}

	return JS_TRUE;
}

JSBool JsDatabaseConnection::executeString(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *query = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&query) ) {
		return Engine::throwUsageError(cx,argv);
	}

	DatabaseConnection *conn = (DatabaseConnection*)JS_GetPrivate(cx,obj);
	if ( conn!=NULL ) 
	{
		try
		{
			sqlite3x::sqlite3_command cmd(conn->getSqliteConn(),query);
			sqlite3x::sqlite3_reader reader = cmd.executereader();

			std::stringstream result;
			formatString(reader,result);
			makeResult(result,cx,rval);
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to execute query [%s]",ex.what());
		}
	}

	return JS_TRUE;
}

JSBool JsDatabaseConnection::executeXml(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	char *query = {0};

	if ( !JS_ConvertArguments(cx,argc,argv,"s",&query) ) {
		return Engine::throwUsageError(cx,argv);
	}

	DatabaseConnection *conn = (DatabaseConnection*)JS_GetPrivate(cx,obj);
	if ( conn!=NULL ) 
	{
		try
		{
			sqlite3x::sqlite3_command cmd(conn->getSqliteConn(),query);
			sqlite3x::sqlite3_reader reader = cmd.executereader();

			std::stringstream result;
			formatXml(reader,result);
			makeResult(result,cx,rval);
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to execute query [%s]",ex.what());
		}
	}

	return JS_TRUE;
}

JSBool JsDatabaseConnection::getInsertId(JSContext *cx,JSObject *obj,uintN argc,jsval *argv,jsval *rval)
{
	if ( argc!=0 ) {
		return Engine::throwUsageError(cx,argv);
	}

	DatabaseConnection *conn = (DatabaseConnection*)JS_GetPrivate(cx,obj);
	if ( conn!=NULL ) 
	{
		try
		{
			uint64_t rowId = conn->getSqliteConn().insertid();
			*rval = INT_TO_JSVAL(rowId);
		}
		catch(exception &ex) {
			LogManager::getInstance()->warning(LOGGER_CLASSNAME,"Failed to retrieve last inserted row id [%s]",ex.what());
		}
	}

	return JS_TRUE;
}

void JsDatabaseConnection::formatJson(sqlite3x::sqlite3_reader &reader,std::stringstream &result)
{
	result << "[";

	int columns = reader.getcolcount();
	bool firstRow = true;

	while ( reader.read() )
	{
		std::string row;
		for ( int i=0; i<columns ;i ++ )
		{
			if ( !row.empty() ) {
				row += ",";
			}

			row += "\"" + reader.getcolname(i) + "\" : \"" + boost::replace_all_copy(reader.getstring(i),"\"","\\\"") + "\"";
		}

		if ( firstRow ) {
			firstRow = false;
		}
		else {
			result << ",\r\n";
		}

		result << "{ " << row << " }";
	}

	result << "]";
}

void JsDatabaseConnection::formatString(sqlite3x::sqlite3_reader &reader,std::stringstream &result)
{
	int columns = reader.getcolcount();
	bool firstRow = true;

	while ( reader.read() )
	{
		for ( int i=0; i<columns ;i ++ ) 
		{
			if ( i>0 ) {
				result << ";";
			}

			result << reader.getstring(i);
		}

		if ( firstRow ) {
			firstRow = false;
		}
		else {
			result << "|";
		}
	}
}

void JsDatabaseConnection::formatXml(sqlite3x::sqlite3_reader &reader,std::stringstream &result)
{
	result << "<result>\r\n";

	int columns = reader.getcolcount();
	while ( reader.read() )
	{
		result << "\t<item";
		for ( int i=0; i<columns ;i ++ ) {
			result << " " << reader.getcolname(i) << "=\"" << reader.getstring(i) << "\"";
		}
		result << " />\r\n";
	}

	result << "</result>\r\n";
}

void JsDatabaseConnection::makeResult(const std::stringstream &result,JSContext *cx,jsval *rval)
{
	size_t size = result.str().size();
	if ( size>0 )
	{
		char *s = (char*)JS_malloc(cx,size+1);
		if ( s!=NULL )
		{
			strcpy(s,result.str().c_str());
			JSString *ret = JS_NewString(cx,s,size);
			if ( ret!=NULL ) {					
				*rval = STRING_TO_JSVAL(ret);
			}
			else {
				JS_free(cx,s);
			}
		}
	}
	else {
		*rval = JSVAL_NULL;
	}
}
