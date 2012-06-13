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
 * @class vibe.util.ResourceManager
 * Singleton class for handling resource objects loaded through JSON requests.
 */
vibe.util.ResourceManager = function()
{
	return Ext.apply(new Ext.util.Observable(),
	{
		// private
		events : [
			/**
			 * @event update
			 * Fires when the resource manager has updated the given resource.
			 * @param {String} name
			 * @param {String} data
			 */
			"update",

			/**
			 * @event updateall
			 * Fires when the resource manager has updated all its resources.
			 */
			"updateall"
		],
		
		// private
		resources : new Array(),
		
		/**
		 * Binds a resource to a given url for fetching resource data.
		 * @param {String} name
		 * @param {String} uri
		 */
		bindResource : function(name,url) 
		{
			var resource = this.findResourceByName(name);
			if ( resource!=null ) {
				resource.data = null;
				resource.url = url;
			}
			else {
				this.resources.push({
					name: name,
					url: url,
					data : null
				});
			}
		},
		
		/**
		 * Updates the given resource.
		 * @param {String} name
		 */
		update : function(name)
		{
			var resource = this.findResourceByName(name);
			if ( resource!=null ) {
				this.updateData(resource);
			}			
		},
		
		/**
		 * Updates all resource data.
		 */
		updateAll : function() {
			this.updateAllData(this.resources.slice(0));
		},	
		
		/**
		 * Get resource data by resource name.
		 * @param {String} name
		 * @return the found resource data
		 */
		getData : function(name) 
		{
			var resource = this.findResourceByName(name);
			if ( resource!=null ) {
				return resource.data;
			}
			
			return null;
		},
		
		/**
		 * Set resource data by resource name.
		 * @param {String} name
		 * @param {String} data
		 */
		setData : function(name,data) 
		{
			var resource = this.findResourceByName(name);
			if ( resource!=null ) {
				resource.data = data;
			}			
		},
		
		// private
		updateData : function(resource)
		{
			vibe.util.JsonLoader.load(resource.url,null,function(result) {
				this.setData(resource.name,result);
				this.fireEvent("update",resource.name,resource.data);
			},
			this);
		},		
		
		// private
		updateAllData : function(resources)
		{
			if ( resources.length>0 ) 
			{
				var resource = resources[0];
				resources.splice(0,1);
				
				vibe.util.JsonLoader.load(resource.url,null,function(result) {
					this.setData(resource.name,result);
					this.updateAllData(resources);
				},
				this);
			}
			else {
				this.fireEvent("updateall");
			}
		},

		// private:		
		findResourceByName : function(name) 
		{
			for ( var i=0, len=this.resources.length; i<len; i++ ) {
				var resource = this.resources[i];
				if ( resource.name==name ) {
					return resource;
				}
			}
			
			return null;
		}		
	});
}();