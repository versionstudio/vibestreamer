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
 * @class vibe.util.FormatUtil
 * Util class with static methods used for formatting.
 */
vibe.util.FormatUtil = function()
{
	return {
		/**
		 * Format the given time value into the duration in minutes and seconds.
		 * @param {Number} value the time value in seconds counted from 1/1 1970.
		 * @return the rendered value
		 */
		duration : function(value)
		{
			if ( value!=null )
			{
				var minutes = Math.floor(value/60);
				var seconds = Math.floor(value%60);
				
				if ( minutes<10 ) minutes = "0" + minutes;
				if ( seconds<10 ) seconds = "0" + seconds;
		
				return minutes + ":" + seconds;
			}
			
			return 0;
		}
	};
}();