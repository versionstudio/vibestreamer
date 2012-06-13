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
#include "base64.h"

std::string Base64::encode(const std::string &s)
{
	std::string ret;

	if (s.length() == 0) {
		return ret;
	}

	for (size_t i=0; i<s.length(); i+=3 ) {
		
		unsigned char by1=0,by2=0,by3=0;

		by1 = s[i];
		if (i+1<s.length()) {
			by2 = s[i+1];
		}
		
		if (i+2<s.length()) {
			by3 = s[i+2];
		}

		unsigned char by4=0,by5=0,by6=0,by7=0;
		
		by4 = by1>>2;
		by5 = ((by1&0x3)<<4)|(by2>>4);
		by6 = ((by2&0xf)<<2)|(by3>>6);
		by7 = by3&0x3f;

		ret += encodeChar(by4);
		ret += encodeChar(by5);

		if (i+1<s.length())	{
			ret += encodeChar(by6);
		}
		else {
			ret += "=";
		}

		if (i+2<s.length()) {
			ret += encodeChar(by7);
		}
		else {
			ret += "=";
		}

		if (i % (76/4*3) == 0) {
			ret += "\r\n";
		}
	}
	return ret;
}

std::string Base64::decode(const std::string &s)
{
	std::string str;
	std::string ret;

	for ( size_t i=0; i<s.length(); i++ ) {
		if (isBase64(s[i])) {
			str += s[i];
		}
		else
			return "";
	}

	if (str.length() == 0) {
		return "";
	}

	for ( size_t i=0; i<str.length(); i+=4 ) {

		char c1='A',c2='A',c3='A',c4='A';
		c1 = str[i];

		if (i+1<str.length()) {
			c2 = str[i+1];
		}
	
		if (i+2<str.length()) {
			c3 = str[i+2];
		}
	
		if (i+3<str.length()) {
			c4 = str[i+3];
		}
	
		unsigned char by1=0,by2=0,by3=0,by4=0;

		by1 = decodeChar(c1);
		by2 = decodeChar(c2);
		by3 = decodeChar(c3);
		by4 = decodeChar(c4);
		
		ret += (by1<<2)|(by2>>4);

		if (c3 != '=') {
			ret += ((by2&0xf)<<4)|(by3>>2);
		}
	
		if (c4 != '=') {
			ret += ((by3&0x3)<<6)|by4;				
		}
	}
	return ret;
}

char Base64::encodeChar(unsigned char c)
{
	if (c < 26) {
		return 'A'+c;
	}

	if (c < 52) {
		return 'a'+(c-26);
	}

	if (c < 62) {
		return '0'+(c-52);
	}
	if (c == 62) {
		return '+';
	}

	return '/';	
}

char Base64::decodeChar(unsigned char c)
{
	if (c >= 'A' && c <= 'Z') {
		return c - 'A';
	}

	if (c >= 'a' && c <= 'z') {
		return c - 'a' + 26;
	}

	if (c >= '0' && c <= '9') {
		return c - '0' + 52;
	}

	if (c == '+') {
		return 62;
	};

	return 63;
}

bool Base64::isBase64(char c)
{
	if (c >= 'A' && c <= 'Z') {
		return true;
	}

	if (c >= 'a' && c <= 'z') {
		return true;
	}
	if (c >= '0' && c <= '9') {
		return true;
	}

	if (c == '+') {
		return true;
	};

	if (c == '/') {
		return true;
	};

	if (c == '=') {
		return true;
	};

	return false;
}
