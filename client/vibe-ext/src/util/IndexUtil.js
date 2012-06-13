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
 * @class vibe.util.IndexUtil
 * Util class with static methods used for creating index records.
 */
vibe.util.IndexUtil = function()
{
	return {
		/**
		 * Record constructor for usage when creating new index records.
		 * Defines all the fields that are mandatory for a record.
		 */
		Record : Ext.data.Record.create([
			{ name: "directory" },
			{ name: "directories" },
			{ name: "files" },
			{ name: "hash" },
			{ name: "itemId" },
			{ name: "lastWriteTime" },
			{ name: "name" },
			{ name: "parentItemId" },
			{ name: "size" },
			{ name: "shareId" }
		]),
		
		/**
		 * Creates an array of index records from a given array of objects
		 * and makes sure all mandatory fields contains a value.
		 * @param {[Object]} objects the array of objects to create the records from
		 * @return a collection of the created records
		 */
		createRecords : function(objects)
		{
			var records = new Array();
			
			if ( objects!=null ) {
				for ( var i=0,len=objects.length; i<len; i++ ) {
					records.push(new vibe.util.IndexUtil.Record(objects[i]));
				}
			}
						
			return records;
		}
	};
}();