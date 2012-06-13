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
 * @class vibe.util.JsonLoader
 * Util class with static methods used for loading and parsing 
 * objects from requests returning json formatted result.
 */
vibe.util.JsonLoader = function()
{
	return {
		/**
		 * Loads an object retrieved and parsed through json format at a given url.
		 * @param {Object} url
		 * @param {Object} params
		 * @param {Object} callback
		 * @param {Object} scope
		 * @return the parsed object
		 */
		load : function(url,params,callback,scope)
		{				
			Ext.Ajax.request(
			{
				method: "POST",
				url: url,
				params: params,
				scope: this,
				callback: function(options,success,response)
				{
					var result = null;
				
					if ( success )
					{
						try	{ 
							result = Ext.util.JSON.decode(response.responseText); 
						}
						catch(e) {
						
						}
					}

					if ( callback!=null && scope!=null ) {
						callback.call(scope,result);
					}
					else if ( callback!=null ) {
						callback(result);
					}			
				}
			});	
		}
	};
}();