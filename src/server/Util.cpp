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
#include "util.h"

#include <ace/os.h>
#include <ace/uuid.h>
#include <boost/lexical_cast.hpp>
#include <openssl/md5.h>

bool Util::ConvertUtil::toBool(const std::string &value) 
{
	return value=="true";
}

bool Util::ConvertUtil::toBool(const std::wstring &value) 
{
	return value==L"true";
}

int Util::ConvertUtil::toInt(const std::string &value)
{
	try {
		return boost::lexical_cast<int>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return 0;
	}
}

int Util::ConvertUtil::toInt(const std::wstring &value)
{
	try {
		return boost::lexical_cast<int>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return 0;
	}
}

int64_t Util::ConvertUtil::toInt64(const std::string &value)
{
	try {
		return boost::lexical_cast<int64_t>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return 0;
	}
}

int64_t Util::ConvertUtil::toInt64(const std::wstring &value)
{
	try {
		return boost::lexical_cast<int64_t>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return 0;
	}
}

unsigned int Util::ConvertUtil::toUnsignedInt(const std::string &value)
{
	try {
		return boost::lexical_cast<unsigned int>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return 0;
	}
}

unsigned int Util::ConvertUtil::toUnsignedInt(const std::wstring &value)
{
	try {
		return boost::lexical_cast<unsigned int>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return 0;
	}
}

uint64_t Util::ConvertUtil::toUnsignedInt64(const std::string &value)
{
	try {
		return boost::lexical_cast<uint64_t>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return 0;
	}
}

uint64_t Util::ConvertUtil::toUnsignedInt64(const std::wstring &value)
{
	try {
		return boost::lexical_cast<uint64_t>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return 0;
	}
}

std::string Util::ConvertUtil::toString(const bool value)
{
	if ( value ) {
		return "true";
	}
	else {
		return "false";
	}
}

std::string Util::ConvertUtil::toString(const int value)
{
	try {
		return boost::lexical_cast<std::string>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return std::string();
	}
}

std::string Util::ConvertUtil::toString(const int64_t value)
{
	try {
		return boost::lexical_cast<std::string>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return std::string();
	}
}

std::string Util::ConvertUtil::toString(const unsigned int value)
{
	try {
		return boost::lexical_cast<std::string>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return std::string();
	}
}

std::string Util::ConvertUtil::toString(const uint64_t value)
{
	try {
		return boost::lexical_cast<std::string>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return std::string();
	}
}

std::string Util::ConvertUtil::toString(const std::wstring& value)
{
	std::string s;
	std::locale loc;

	for ( unsigned int i=0; i<value.size(); ++i )	{
		s += std::use_facet<std::ctype<wchar_t> >(loc).narrow(value[i]);
	}

	return s;
}

std::wstring Util::ConvertUtil::toWideString(const bool value)
{
	if ( value ) {
		return L"true";
	}
	else {
		return L"false";
	}
}

std::wstring Util::ConvertUtil::toWideString(const int value)
{
	try {
		return boost::lexical_cast<std::wstring>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return std::wstring();
	}
}

std::wstring Util::ConvertUtil::toWideString(const int64_t value)
{
	try {
		return boost::lexical_cast<std::wstring>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return std::wstring();
	}
}

std::wstring Util::ConvertUtil::toWideString(const unsigned int value)
{
	try {
		return boost::lexical_cast<std::wstring>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return std::wstring();
	}
}

std::wstring Util::ConvertUtil::toWideString(const uint64_t value)
{
	try {
		return boost::lexical_cast<std::wstring>(value);
	}
	catch(boost::bad_lexical_cast&) {
		return std::wstring();
	}
}

std::wstring Util::ConvertUtil::toWideString(const std::string& value)
{
	std::wstring s;
	std::locale loc;

	for ( unsigned int i=0; i<value.size(); ++i ) {
		s += std::use_facet<std::ctype<wchar_t> >(loc).widen(value[i]);
	}

	return s;
}

std::string Util::CryptoUtil::generateGuid()
{
	ACE_Utils::UUID *uuid = ACE_Utils::UUID_GENERATOR::instance()->generate_UUID();
	ACE_CString uuid_str(uuid->to_string()->c_str());

	return std::string(uuid_str.c_str());
}

std::string Util::CryptoUtil::md5Encode(const char *s,size_t len)
{
	unsigned char digest[16] = {0};

	MD5_CTX context;

	MD5_Init(&context);
	MD5_Update(&context,s,len);
	MD5_Final(digest,&context);

	std::string md5sum;

	static const char *hex = "0123456789abcdef";
	for ( int i=0; i<sizeof(digest); i++ ) {
		md5sum += hex[(digest[i] & 0xf0) >> 4];
		md5sum += hex[(digest[i] & 0x0f)];
	}

	return md5sum;
}

std::string Util::CryptoUtil::urlEncode(const std::string &s)
{
	std::string urlEncoded;
	
	size_t length = s.length();
	for ( size_t i=0; i<length; i++)
	{
		if ( isalnum(s[i]) )
			urlEncoded += s[i];
		else
		{
			if ( isspace(s[i]) && s[i]!='\n' )
				urlEncoded += '+';
			else
			{
				urlEncoded += '%';
				urlEncoded += Util::CryptoUtil::toHex(s[i]>>4);
				urlEncoded += Util::CryptoUtil::toHex(s[i]%16);
			}
		}
	}

	return urlEncoded;
}

std::string Util::CryptoUtil::urlDecode(const std::string &s)
{
	std::string urlEncoded = boost::replace_all_copy(s,"+"," ");
	std::string urlDecoded;

	char chdec;
	int state = 0;

	std::string::const_iterator iter;
	for ( iter=urlEncoded.begin(); iter!=urlEncoded.end(); iter++ )
	{
		switch (state) 
		{
			case 0:
			{
				if (*iter=='%') {
					chdec = 0;
					state = 1;
				} 
				else {
					urlDecoded += *iter;
				}
			}
			break;
			
			case 1:
			{
				chdec += Util::CryptoUtil::toHexInt(*iter) * 16;
				state = 2;
			}
			break;

			case 2:
			{
				chdec += Util::CryptoUtil::toHexInt(*iter);
				urlDecoded += chdec;
				state = 0;
			}
			break;
		};
	};

	return urlDecoded;
}

unsigned char Util::CryptoUtil::toHex(const unsigned char &value)
{
	return value>9 ? value+55 : value+48;
}

int Util::CryptoUtil::toHexInt(const unsigned char &value)
{
	if ( value>='0' && value<='9' ) {
		return value-'0';
	}
	else if ( value>='a' && value<='f' ) {
		return value-'a'+10;
	}
	else if ( value>='A' && value<='F' ) {
		return value-'A'+10;
	}
	else {
		return 0;
	}
}

bool Util::StringUtil::isAlpha(const std::string &s)
{
	if ( s.empty() ) {
		return false;
	}

	size_t length = s.length();
	for ( size_t i=0; i<length; i++ ) 
	{
		if ( !isalpha(s[i]) ) {
			return false;
		}
	}

	return true;
}

bool Util::StringUtil::isAlphaNumeric(const std::string &s)
{
	if ( s.empty() ) {
		return false;
	}

	size_t length = s.length();
	for ( size_t i=0; i<length; i++ ) 
	{
		if ( !isalnum(s[i]) ) {
			return false;
		}
	}

	return true;
}

bool Util::StringUtil::isIpAddress(const std::string &ipAddress,bool wildcards)
{
	if ( ipAddress.empty() ) {
		return false;
	}

	boost::regex ipAddressRegex;

	if ( wildcards ) {
		ipAddressRegex = boost::regex("[0-9*]{1,3}\\.[0-9*]{1,3}\\.[0-9*]{1,3}\\.[0-9*]{1,3}");
	}
	else {
		ipAddressRegex = boost::regex("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");
	}

	return boost::regex_search(ipAddress,ipAddressRegex);
}

bool Util::StringUtil::isMatchingIpAddress(const std::string &ipAddress,const std::string &pattern)
{
	if ( !isIpAddress(ipAddress) || !isIpAddress(pattern,true) ) {
		return false;
	}

	boost::regex patternRegex(boost::replace_all_copy(pattern,"*","[0-9]{1,3}"));

	return boost::regex_search(ipAddress,patternRegex);
}

bool Util::StringUtil::isNumeric(const std::string &s)
{
	if ( s.empty() ) {
		return false;
	}

	size_t length = s.length();
	for ( size_t i=0; i<length; i++ ) 
	{
		if ( !isdigit(s[i]) ) {
			return false;
		}
	}

	return true;
}

std::string Util::StringUtil::format(const char *fmt,...)
{
	std::string formatted;

	if ( fmt!=NULL )
	{
		va_list args;
		va_start(args,fmt);
		formatted = formatArgs(fmt,args);
		va_end(args);
	}

	return formatted;
}

std::string Util::StringUtil::formatArgs(const char *fmt,va_list args)
{
	std::string formatted;

	if ( fmt!=NULL )
	{
		int result = -1;
		int length = 256;

		char *buffer = NULL;
		while ( result==-1 )
		{
			if ( buffer!=NULL ) {
				delete[] buffer;
			}

			buffer = new char[length];
			memset(buffer,0,length);
			result = _vsnprintf(buffer,length,fmt,args);
			length *= 2;
		}

		formatted = buffer;
		delete[] buffer;
	}

	return formatted;
}

std::string Util::TimeUtil::format(const tm &localTime,const char *fmt)
{
	char formattedTime[255] = {0};
	ACE_OS::strftime(formattedTime,sizeof(formattedTime),fmt,&localTime);

	return formattedTime;
}

time_t Util::TimeUtil::getCalendarTime()
{
	time_t calendarTime;
	time(&calendarTime);

	return calendarTime;
}

bool Util::TimeUtil::getLocalTime(tm *localTime)
{
	time_t currentTime = time(NULL);

	return ACE_OS::localtime_r(&currentTime,localTime)!=NULL;
}

bool Util::TimeUtil::getLocalTime(const time_t& calendarTime,tm *localTime)
{
	return ACE_OS::localtime_r(&calendarTime,localTime)!=NULL;
}

std::string Util::UriUtil::getLastSegment(const std::string &uri)
{
	std::string lastSegment;

	size_t pos = uri.find_last_of("/");
	if ( pos!=std::string::npos ) {
		lastSegment = uri.substr(pos+1);
	}
	else {
		lastSegment = uri;
	}

	return lastSegment;
}

std::string Util::UriUtil::getParentSegment(const std::string &uri)
{
	std::string parentSegment;

	size_t pos = uri.find_last_of("/");
	if ( pos!=std::string::npos ) {
		parentSegment = uri.substr(0,pos);
	}

	return parentSegment;
}

bool Util::UriUtil::isAbsolute(const std::string &uri)
{
	return !uri.empty() && uri.at(0)=='/';
}

bool Util::UriUtil::isValid(const std::string &uri)
{
	return !uri.empty() && uri.find("\\")==std::string::npos
		&& uri.find("../")==std::string::npos;
}
