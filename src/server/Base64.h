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

#ifndef guard_base64_h
#define guard_base64_h

/**
* Base64.
* Util class for encoding and decoding from/to base64 format.
*/
class Base64
{
public:
	/**
	 * Encode the given string.
	 * @param s the string to encode
	 * @return the encoded string
	 */
	static std::string encode(const std::string &s);

	/**
	 * Decode the given string.
	 * @param s the string to decode
	 * @return the decoded string
	 */
	static std::string decode(const std::string &s);

private:
	/**
	 * Encode a single character.
	 * @param c the character to encode
	 * @return the encoded character
	 */
	static char encodeChar(unsigned char c);

	/**
	 * Decode a single character.
	 * @param c the character to decode
	 * @return the decoded character
	 */
	static char decodeChar(unsigned char c);

	/**
	 * Get whether a given character is base64 encoded or not.
	 * @param c the character to check
	 * @return whether the given character is base64 encoded
	 */
	static bool isBase64(char c);
};

#endif
