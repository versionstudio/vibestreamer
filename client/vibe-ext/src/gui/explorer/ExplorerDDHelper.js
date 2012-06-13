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

Ext.namespace("vibe.gui.explorer");

/**
 * @vibe.gui.explorer.ExplorerDDHelper
 * @extends vibe.dd.DDHelper
 * DDHelper used by the explorer.
 */
vibe.gui.explorer.ExplorerDDHelper = Ext.extend(vibe.dd.DDHelper,
{
	/**
	 * @override
	 */
	validateDrop : function(source,e,data) 
	{
		for ( var i=0, len=data.selections.length; i<len; i++ ) 
		{
			var record = data.selections[i];
			var extension = vibe.util.StringUtil.getFileExtension(record.get("name"));
			var supported =  vibe.Config.app.PLAYER_FILETYPES.indexOf(extension.toLowerCase())!=-1;
			
			if ( record.get("directory")!=true && !supported ) {
				return false;
			}
		}
		
		return true;
	},
	
	/**
	 * @override
	 */
    loadDrop : function(source,e,data,callback,scope,args) 
	{
		var loadedRecords = new Array();
		this.loadDropRecords(data.selections,loadedRecords,
			callback,scope,args);
    },
	
	// private
	loadDropRecords : function(records,loadedRecords,callback,scope,args)
	{
		if ( records.length>0 ) 
		{
			var record = records.shift();
			if ( record.get("directory")==true )
			{
				if ( record.get("directories")>0 || record.get("files")>0 )
				{
					var shareId = record.get("shareId");
					var hash = record.get("hash");
					
					var url = "resources/ajax/get-index-files.vibe";
					var params = "shareId=" + shareId + "&directories=true&files=true&hash=" + hash;
					
					// load directory content and append to record queue
					vibe.util.JsonLoader.load(url,params,function(result)
					{
						if ( result!=null ) {
							records = vibe.util.IndexUtil.createRecords(result).concat(records);
						}

						this.loadDropRecords(records,loadedRecords,
							callback,scope,args);
					},
					this);
					return;
				}
			}
			else {
				loadedRecords.push(new vibe.util.IndexUtil.Record(record.data));
			}
			
			this.loadDropRecords(records,loadedRecords,
				callback,scope,args);
		}
		else {
			callback.call(scope,loadedRecords,args);
		}
	}
});