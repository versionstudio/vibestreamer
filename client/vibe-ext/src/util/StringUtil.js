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

Ext.namespace("vibe.util");

/**
 * @class vibe.util.StringUtil
 * Util class with static methods used for parsing and working with strings.
 */
vibe.util.StringUtil = function()
{
	return {
		/**
		 * Returns the file extension of the given path.
		 * @param {String} path the path or filename to get the extension for
		 * @return the file extension
		 */
		getFileExtension : function(path)
		{
			var pos = path.lastIndexOf(".");
			if ( pos>-1 ) {
				return path.substr(pos+1);	
			}
			else {
				return "";
			}
		},
		
		/**
		* Check whether a given string ends with a prefix.
		*/
		endsWith : function(s,prefix) {
			return s.lastIndexOf(prefix)==s.length-prefix.length;
		},
		
		/**
		* Check whether a given string starts with a prefix.
		*/
		startsWith : function(s,prefix) {
			return s.indexOf(prefix)==0;
		}
	};
}();