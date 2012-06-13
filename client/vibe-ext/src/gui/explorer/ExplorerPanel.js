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
 * @class vibe.gui.explorer.ExplorerPanel
 * @extends Ext.Panel
 * @param {vibe.gui.search.SearchPanel} searchPanel
 * @param {vibe.gui.playlist.PlaylistGrid} playlistGrid
 * Panel for exploring all indexed files and directories.
 */
vibe.gui.explorer.ExplorerPanel = function(searchPanel,playlistGrid,config) {
	this.searchPanel = searchPanel;
	this.playlistGrid = playlistGrid;
	vibe.gui.explorer.ExplorerPanel.superclass.constructor.call(this,config);
};

Ext.extend(vibe.gui.explorer.ExplorerPanel,Ext.Panel,
{
	// private
	bufferedGrid : null,
		
	// private
	bufferedTreeGrid : null,
	
	/**
	 * @override
	 */
	initComponent : function()
	{
		this.bufferedGrid = new vibe.widget.BufferedGrid({
			cm: new Ext.grid.ColumnModel([
				{
					align: "left",
					dataIndex: "name",
					header: vibe.Language.app.NAME,
					hideable: false,					
					sortable: false
				},
				{
					align: "left",
					dataIndex: "size",
					hidden: true,
					hideable: true,
					header: vibe.Language.app.SIZE,
					renderer: function(value,metadata,record,rowIndex,colIndex,store) {
						return (value!=null && value>0 ? Ext.util.Format.fileSize(value) : null);
					},
					sortable: false,
					width: 100
				},
				{
					align: "left",
					dataIndex: "lastWriteTime",
					hidden: true,
					hideable: true,
					header: vibe.Language.app.DATE_MODIFIED,
					renderer: function(value,metadata,record,rowIndex,colIndex,store) {
						return Ext.util.Format.date(new Date(value*1000),vibe.Language.app.DATE_FORMAT_LONG);
					},
					sortable: false,
					width: 140
				}				
			]),
			ddGroup: vibe.Config.app.INDEX_DDGROUP,
			ddHelper: new vibe.gui.explorer.ExplorerDDHelper(),
			ddText: vibe.Language.app.DRAGDROP_TEXT,
			enableColumnMove : true,
			enableDragDrop : true,
			getIconClass : this.getBufferedGridIconClass,
			header: false,
			region: "center",
			split: true
		});
				
		this.bufferedTreeGrid = new vibe.widget.BufferedTreeGrid({
			cm: new Ext.grid.ColumnModel([
				{
					align: "left",
					dataIndex: "name",
					hideable: false,
					header: vibe.Language.app.SHARES,
					sortable: false,
					resizable: false
				}		
			]),
			ddGroup: vibe.Config.app.INDEX_DDGROUP,
			ddHelper: new vibe.gui.explorer.ExplorerDDHelper(),
			ddText: vibe.Language.app.DRAGDROP_TEXT,
			enableColumnMove: false,
			enableDragDrop: true,
			enableHdMenu: false,
			getIconClass: this.getBufferedTreeGridIconClass,
			header: false,
			isExpandable: this.isBufferedTreeGridExpandable,
			region: "west",
			split: true,
			width: 250
		});
		
		Ext.apply(this,
		{
			border: false,
			cls: "vibe-explorer",
			items: [this.bufferedTreeGrid,
				this.bufferedGrid],
			layout: "border"
		});
		
		vibe.gui.explorer.ExplorerPanel.superclass.initComponent.call(this);
		
		this.bufferedGrid.on("celldblclick",this.onBufferedGridCellDblClick,this);
		this.bufferedGrid.on("render",this.onBufferedGridRender,this);
		this.bufferedTreeGrid.on("itemclick",this.onBufferedTreeGridItemClick,this);
		this.bufferedTreeGrid.on("itemexpand",this.onBufferedTreeGridItemExpand,this);
		this.bufferedTreeGrid.on("render",this.onBufferedTreeGridRender,this);	
		
		this.searchPanel.on("search",this.onSearchPanelSearch,this);
	},
	
	// private
	loadContentRecords : function(record)
	{
		if ( this.bufferedGrid.getView()._loadMaskAnchor.isMasked() ) {
			return;
		}
		
		this.bufferedGrid.getView().showLoadMask(true);		
		
		var shareId = record.get("shareId");
		var hash = record.get("hash");
		
		var url = "resources/ajax/get-index-files.vibe";
		var params = "shareId=" + shareId + "&directories=true&files=true";
		
		if ( hash!=null ) {
			params += "&hash=" + hash;
		}
				
		vibe.util.JsonLoader.load(url,params,function(result)
		{
			if ( result!=null ) {
				this.bufferedGrid.showRecords(vibe.util.IndexUtil.createRecords(result));
			}
			
			this.bufferedGrid.getView().showLoadMask(false);
		},
		this);		
	},
	
	// private
	updateStatusMessage : function()
	{
		/*
		var view = this.bufferedGrid.view;
		var msg = String.format(vibe.Language.app.STATUS_MSG,Math.min(view.ds.totalLength,view.rowIndex+1),
			Math.min(view.ds.totalLength,view.rowIndex+view.visibleRows),view.ds.totalLength);
		
		this.statusBar.setText(msg);
		*/
	},
	
	// private
	getBufferedGridIconClass : function(record,index)
	{
		var directory = record.get("directory");
		if ( record.get("directory")==true ) {
			return "vibe-icon-folder";
		}
		else {
			var extension = vibe.util.StringUtil.getFileExtension(record.get("name"));
			return "vibe-icon-file-" + extension;
		}
	},
			
	// private
	getBufferedTreeGridIconClass : function(record,index)
	{
		var directory = record.get("directory");
		if ( directory==null || directory==true ) 
		{
			if ( record.get("expanded")==true ) {
				return "vibe-icon-folder-open";
			}
			else {
				return "vibe-icon-folder";
			}
		}
		else {
			var extension = vibe.util.StringUtil.getFileExtension(record.get("name"));
			return "vibe-icon-file-" + extension;			
		}
	},
	
	// private
	isBufferedTreeGridExpandable : function(record,index) 
	{
		var directory = record.get("directory");
		if ( (directory==null || directory==true) 
			&& record.get("directories")>0 
		) {
			return true;
		}
		else {
			return false;	
		}
	},
	
	// private
	onBufferedGridCellDblClick : function(grid,rowIndex,columnIndex,e)
	{
		var record = grid.getStore().getAt(rowIndex);
	
		if ( record.get("directory")==true ) {
			this.loadContentRecords(record);
		}
		else
		{
			var session = vibe.util.ResourceManager.getData("vibe.session");
			if ( session.user.browser ) {
				Ext.Msg.alert(vibe.Language.app.PERMISSION_DENIED,vibe.Language.app.BROWSE_ONLY);
				return;
			}
			else if ( !this.playlistGrid.isBusy() )
			{
				if ( this.playlistGrid.insertTracks(vibe.util.IndexUtil.createRecords([record.data]))==0 ) {
					window.open(vibe.Config.app.SHARE_URI + record.get("hash"),"vibewnd");
				}
			}
		}
	},
	
	// private
	onBufferedTreeGridItemClick : function(grid,rowIndex) {
		this.loadContentRecords(grid.getStore().getAt(rowIndex));
	},
	
	// private
    onBufferedGridRender : function(component) 
	{
		component.view.on("cursormove",function() {
			this.updateStatusMessage();	
		},this);
						
		component.view.on("rowremoved",function() {
			this.updateStatusMessage();	
		},this);

		component.view.on("rowsinserted",function() {
			this.updateStatusMessage();	
		},this);
	},
	
	// private
	onBufferedTreeGridItemExpand : function(grid,rowIndex)
	{
		var record = grid.getStore().getAt(rowIndex);
		var shareId = record.get("shareId");
		var hash = record.get("hash");		
		
		var url = "resources/ajax/get-index-files.vibe";
		var params = "shareId=" + shareId + "&directories=true";
		
		if ( hash!=null ) {
			params += "&hash=" + hash;
		}
		
		vibe.util.JsonLoader.load(url,params,function(result)
		{
			if ( result!=null ) {
				this.bufferedTreeGrid.insertRecords(vibe.util.IndexUtil.createRecords(result),rowIndex);
			}
		},
		this);
	},
	
	// private
    onBufferedTreeGridRender : function(component) 
	{
		var shares = vibe.util.ResourceManager.getData("vibe.server").shares;
		if ( shares!=null )
		{
			var records = new Array();
			
			// create share records
			for ( var i=0,len=shares.length; i<len; i++ ) {
				var share = shares[i];
				records.push({
					directories : share.directories,
					files : share.files,
					name : share.name,
					shareId : share.dbId
				});
			}
			
			// sort share records
			records.sort(function(a,b) {
				var x = a.name.toLowerCase();
				var y = b.name.toLowerCase();
				return ((x < y) ? -1 : ((x > y) ? 1 : 0));
			});
			
			this.bufferedTreeGrid.insertRecords(vibe.util.IndexUtil.createRecords(records),-1);
		}
	},
	
	// private
	onSearchPanelSearch : function(searchPanel,expression)
	{
		if ( this.collapsed ) {
			this.expand(true);
		}
		
		var url = "resources/ajax/get-index-searchfiles.vibe";
		var params = "directories=true&files=true&expression=" + encodeURIComponent(expression);
		
		vibe.util.JsonLoader.load(url,params,function(result) {
			this.bufferedGrid.showRecords(vibe.util.IndexUtil.createRecords(result));
		},
		this);
	}
});