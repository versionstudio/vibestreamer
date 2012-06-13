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

#ifndef guard_util_h
#define guard_util_h

namespace Util
{
	/**
	* ConvertUtil.
	* Util class with utility functions for converting between types.
	*/
	class ConvertUtil
	{
	public:
		/**
		* Convert a string to a boolean.
		* @param value the value to convert
		* @return the converted string
		*/
		static bool toBool(const std::string &value);

		/**
		* Convert a wide string to a boolean.
		* @param value the value to convert
		* @return the converted string
		*/
		static bool toBool(const std::wstring &value);

		/**
		* Convert a string to an integer.
		* @param value the value to convert
		* @return the converted string
		*/
		static int toInt(const std::string &value);

		/**
		* Convert a wide string to an integer.
		* @param value the value to convert
		* @return the converted string
		*/
		static int toInt(const std::wstring &value);

		/**
		* Convert a string to a 64 bit integer
		* @param value the value to convert
		* @return the converted string
		*/
		static int64_t toInt64(const std::string &value);

		/**
		* Convert a wide string to a 64 bit integer
		* @param value the value to convert
		* @return the converted string
		*/
		static int64_t toInt64(const std::wstring &value);

		/**
		* Convert a string to an unsigned integer.
		* @param value the value to convert
		* @return the converted value
		*/
		static unsigned int toUnsignedInt(const std::string &value);

		/**
		* Convert a wide string to an unsigned integer.
		* @param value the value to convert
		* @return the converted value
		*/
		static unsigned int toUnsignedInt(const std::wstring &value);

		/**
		* Convert a string to an unsigned 64 bit integer
		* @param value the value to convert
		* @return the converted value
		*/
		static uint64_t toUnsignedInt64(const std::string &value);

		/**
		* Convert a wide string to an unsigned 64 bit integer
		* @param value the value to convert
		* @return the converted value
		*/
		static uint64_t toUnsignedInt64(const std::wstring &value);

		/**
		* Convert a boolean to a string.
		* @param value the value to convert
		* @return the converted value
		*/
		static std::string toString(const bool value);

		/**
		* Convert an integer to a string.
		* @param value the value to convert
		* @return the converted value
		*/
		static std::string toString(const int value);

		/**
		* Convert a 64 bit integer to a string.
		* @param value the value to convert
		* @return the converted value
		*/
		static std::string toString(const int64_t value);

		/**
		* Convert an unsigned integer to a string.
		* @param value the value to convert
		* @return the converted value
		*/
		static std::string toString(const unsigned int value);

		/**
		* Convert an unsigned 64 bit integer to a string.
		* @param value the value to convert
		* @return the converted value
		*/
		static std::string toString(const uint64_t value);

		/**
		* Convert a wide string into a string.
		* @param value the value to convert
		* @return the converted value
		*/
		static std::string toString(const std::wstring &value);

		/**
		* Convert a boolean to a wide string.
		* @param value the value to convert
		* @return the converted value
		*/
		static std::wstring toWideString(const bool value);

		/**
		* Convert an integer to a wide string.
		* @param value the value to convert
		* @return the converted value
		*/
		static std::wstring toWideString(const int value);

		/**
		* Convert a 64 bit integer to a wide string.
		* @param value the value to convert
		* @return the converted value
		*/
		static std::wstring toWideString(const int64_t value);

		/**
		* Convert an unsigned integer to a wide string.
		* @param value the value to convert
		* @return the converted value
		*/
		static std::wstring toWideString(const unsigned int value);

		/**
		* Convert an unsigned 64 bit integer to a wide string.
		* @param value the value to convert
		* @return the converted value
		*/
		static std::wstring toWideString(const uint64_t value);

		/**
		* Convert the given string into a wide string.
		* @param value the value to convert
		* @return the converted value.
		*/
		static std::wstring toWideString(const std::string &value);
	};

	/**
	* CryptoUtil.
	* Util class with cryptography utility functions.
	*/
	class CryptoUtil
	{
	public:
		/**
		* Generate a guid.
		* @return the generated guid
		*/
		static std::string generateGuid();

		/**
		* Generate an md5 hash sum based on the given string.
		* @param s the string used to generate the md5 hash
		* @param len the length of the string
		* @return the md5 sum
		*/
		static std::string md5Encode(const char *s,size_t len);

		/**
		* Url encode a string.
		* @param s the string to url encode
		* @return the url encoded string
		*/
		static std::string urlEncode(const std::string &s);

		/**
		* Url decode a string.
		* @param s the string to url decode
		* @return the url decoded string
		*/
		static std::string urlDecode(const std::string &s);

	private:
		/**
		* Convert a character to hex.
		* @param value the value to convert
		* @return the converted value
		*/
		static unsigned char toHex(const unsigned char &value);

		/**
		* Convert a character to hex int.
		* @param value the value to convert
		* @return the converted value
		*/
		static int toHexInt(const unsigned char &value);
	};

	/**
	* StringUtil.
	* Util class with utility functions for working with strings.
	*/
	class StringUtil
	{
	public:
		/**
		* Get whether a string contains only alphabetic characters.
		* @param s the string to validate
		* @return true if the string contains only alphabetic characters
		*/
		static bool isAlpha(const std::string &s);

		/**
		* Get whether a string contains only alphabetic characters or digits.
		* @param s the string to validate
		* @return true if the string contains only alphabetic characters or digits
		*/
		static bool isAlphaNumeric(const std::string &s);

		/**
		* Get whether a string is a valid ip address.
		* Addresses are validated on the format x.x.x.x where numerical values 0-999 are allowed.
		* Wildcards in ip addresses are supported as an option.
		* @param ipAddress the ip address to validate
		* @param wildcards whether wildcards are allowed in the ip address
		* @return true if the string is a valid ip address
		*/
		static bool isIpAddress(const std::string &ipAddress,bool wildcards=false);

		/**
		* Get whether a string matches an ip address pattern.
		* Addresses are validated on the format x.x.x.x where numerical values 0-999 are allowed.
		* @param ipAddress the ip address to validate against the pattern
		* @param pattern the ip address pattern to match towards. Wildcards are supported.
		* @return true if the string matches the pattern
		*/
		static bool isMatchingIpAddress(const std::string &ipAddress,const std::string &pattern);

		/**
		* Get whether a string contains only digits.
		* @param s the string to check
		* @return true if the string contains only digits
		*/
		static bool isNumeric(const std::string &s);

		/**
		* Format a string. See sprintf() for supported parameters.
		* @param fmt the formatted string together with any additional arguments
		* @return the formatted string
		*/
		static std::string format(const char *fmt,...);

		/**
		* Format a string using the arguments list.
		* @param fmt the formatted string
		* @param args the arguments list
		* @return the formatted string
		*/
		static std::string formatArgs(const char *fmt,va_list args);
	};

	/**
	* TimeUtil.
	* Util class with utility functions for working with time.
	*/
	class TimeUtil
	{
	public:
		/**
		* Format a time value into a string.
		* @param localTime the time to format
		* @param format the time format, as supported by the strftime() method
		* @return the formatted time string
		*/
		static std::string format(const tm &localTime,const char *fmt);

		/**
		* Get the current calendar time.
		* @return the current calendar time
		*/
		static time_t getCalendarTime();

		/**
		* Get the current local time.
		* @param localTime the out parameter for the local time
		* @return true if time was retrieved successfully
		*/
		static bool getLocalTime(tm *localTime);

		/**
		* Get the local time based on the given calendar time.
		* @param calendarTime the calendar time
		* @param localTime the out parameter for the local time
		* @return true if time was retrieved successfully
		*/
		static bool getLocalTime(const time_t& calendarTime,tm *localTime);
	};

	/**
	* UriUtil.
	* Util class with utility functions for working with uri's.
	*/
	class UriUtil
	{
	public:
		/**
		* Get the last segment from the uri.
		* @param uri the uri
		* @return the last segment of the uri
		*/
		static std::string getLastSegment(const std::string &uri);

		/**
		* Get the parent segment from the uri.
		* If compared to a file path, the returned parent segment is the path
		* to the directory a file is located in.
		* @param uri the uri
		* @return the parent segment of the uri
		*/
		static std::string getParentSegment(const std::string &uri);

		/**
		* Get whether the given uri is absolute.
		* @param uri the uri
		* @return true if the uri absolute
		*/
		static bool isAbsolute(const std::string &uri);


		/**
		* Get whether the given uri is valid.
		* @param uri the uri
		* @return true if the given uri is valid
		*/
		static bool isValid(const std::string &uri);
	};
};

#endif
