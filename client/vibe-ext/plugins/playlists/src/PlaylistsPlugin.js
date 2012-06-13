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

Ext.namespace("vibe.plugin.playlists");

Ext.onReady(function()
{
	vibe.Application.on("beforeload",function(app) {
		app.addPlugin(new vibe.plugin.playlists.PlaylistsPlugin);
	});
});

/**
 * @class vibe.plugin.playlists.PlaylistsPlugin
 * @extends vibe.plugin.Plugin
 * The playlists plugin.
 * Makes it possible to load and save playlists.
 */
vibe.plugin.playlists.PlaylistsPlugin = Ext.extend(vibe.plugin.Plugin,
{		
	// private
	loadBuffer : 250,
	
	// private
	loadMask : null,

	// private
	newButton : null,
	
	// private
	panel : null,
	
	// private
	playlistGrid : null,
	
	// private
	playlistTree : null,
	
	// private
	saveBuffer : 250,
	
	// private
	treeButton : null,
		
	// private
	treeEditor : null,
	
	// private
	treeSorter : null,
	
	/**
	 * @override
	 */
	init : function(app)
	{
		// apply default plugin language
		Ext.applyIf(vibe.Language.plugin,{playlists:[]});		
		Ext.applyIf(vibe.Language.plugin.playlists,{
			CONFIRM_REMOVE_PLAYLIST: "Do you really want to remove the playlist \"{0}\"?",
			CREATE_NEW_PLAYLIST: "Create New Playlist",
			DRAG_PLAYLIST_TO_LOAD: "Drag playlist to queue to load",
			ENTER_PLAYLIST_NAME: "Enter playlist name or save as existing",
			LOAD: "Load",
			LOAD_PLAYLIST: "Load playlist",
			REMOVE_PLAYLIST: "Remove playlist",
			SAVE: "Save",
			SAVE_PLAYLIST: "Save playlist",
			SELECT_PLAYLIST: "Select playlist",
			TITLE: "Playlists"
		});
	},
	
	/**
	 * @override
	 */
	startup : function(app)
	{
		this.playlistGrid = app.getPlaylistGrid();

		this.newButton = new vibe.widget.LinkButton({
			height: 20,
			iconCls: "vibe-plugin-playlists-icon-add",
			text: vibe.Language.plugin.playlists.CREATE_NEW_PLAYLIST
		});
				
		this.playlistTree = new Ext.tree.TreePanel({
			autoScroll: true,
			border: false,
			ddAppendOnly: true,
			ddGroup: vibe.Config.app.INDEX_DDGROUP,
			ddHelper: new vibe.plugin.playlists.PlaylistsDDHelper(this,this.playlistGrid),
			enableDrag: true,
			enableDrop: true,
			lines:false,
			loader: null,
			root: new Ext.tree.TreeNode(),
			rootVisible: false
		});
		
		this.treeButton = new vibe.widget.TreeButton(this.playlistTree,{
			iconCls: "vibe-plugin-playlists-icon-remove",
			offsets: [175,0]
		});
		
		this.treeEditor = new Ext.tree.TreeEditor(this.playlistTree,{
			allowBlank: false,
			blankText: this.nameRequiredLabel
		},{
			cancelOnEsc: true,
			completeOnEnter: true,
			editDelay: 250,			
			ignoreNoChange: false
		});
				
		this.treeSorter = new Ext.tree.TreeSorter(this.playlistTree,{
		    dir: "asc",
		    folderSort: true				
		});
		
		this.panel = new Ext.Panel({
			autoScroll: true,
			border: false,
			cls: "vibe-plugin-playlists-panel",
			collapsed: true,
			items: [this.newButton,
				this.playlistTree],
			title: vibe.Language.plugin.playlists.TITLE
		});
	
		this.newButton.on("click",this.onNewButtonClick,this);
		this.panel.on("render",this.onPanelRender,this);
		this.playlistTree.on("beforenodedrop",this.onPlaylistTreeBeforeNodeDrop,this);
		this.playlistTree.on("nodedragover",this.onPlaylistTreeNodeDragOver,this);
		this.treeButton.on("click",this.onTreeButtonClick,this);
		this.treeEditor.on("complete",this.onTreeEditorComplete,this);
		
		app.addModule(this.panel,"bottom");
	},
	
	// private
	onNewButtonClick : function(component)
	{
		var newNode = this.createPlaylistNode(vibe.Language.app.UNTITLED_PLAYLIST);
		this.treeEditor.triggerEdit(newNode);
	},
	
	// private
	onPanelRender : function(component)
	{
		this.loadMask = new Ext.LoadMask(component.body,
			{ msg:vibe.Language.app.LOADMASK });
			
		var tbar = this.playlistGrid.getTopToolbar();
				
		// insert "save" button
		tbar.insertButton(3,new Ext.Toolbar.Button({
			handler: this.showSaveDialog,
			scope: this,
			text: vibe.Language.plugin.playlists.SAVE,
			tooltip: vibe.Language.plugin.playlists.SAVE_PLAYLIST
		}));		
		
		tbar.insertButton(3,new Ext.Toolbar.Separator());
		
		// insert "load" button
		tbar.insertButton(3,new Ext.Toolbar.Button({
			handler: this.showLoadDialog,
			scope: this,
			text: vibe.Language.plugin.playlists.LOAD,
			tooltip: vibe.Language.plugin.playlists.LOAD_PLAYLIST
		}));

		tbar.insertButton(3,new Ext.Toolbar.Separator());
		
		this.updateTree();
	},
	
	// private
	onPlaylistTreeBeforeNodeDrop : function(e)
	{
		var ddHelper = this.getSupportedDDHelper(e.data);
		if ( ddHelper==null ) {
			return false;
		}
		
		this.loadMask.show();
		
		var playlistId = e.target.attributes["data"].playlistId;
		ddHelper.loadDrop(e.source,e,e.data,
			this.dropLoadCallback,this,[playlistId]);
	},
	
	// private
	onPlaylistTreeNodeDragOver : function(e)
	{
		if ( e.source.tree==this.playlistTree ) {
			e.cancel = true;
		}
		else 
		{
			var ddHelper = this.getSupportedDDHelper(e.data);
			if ( ddHelper==null ) {
				return false;
			}
			
			if ( this.loadMask.el.isMasked() ) {
				return false;
			}
			
			return ddHelper.validateDrop(e.source,e,e.data);
		}
	},
	
	// private
	onTreeButtonClick : function(treeButton,node)
	{
		Ext.Msg.show({
			title: vibe.Language.plugin.playlists.REMOVE_PLAYLIST,
			msg: String.format(vibe.Language.plugin.playlists.CONFIRM_REMOVE_PLAYLIST,node.text),
			buttons: Ext.Msg.YESNO,
			fn: function(btn,text)
			{
				if ( btn=="yes" ) 
				{
					var playlistId = node.attributes["data"].playlistId;
					this.removePlaylist(playlistId,function(success) {
						if ( success ) {
							node.remove();
						}
					});
				}
			},
			icon: Ext.MessageBox.QUESTION,
			scope: this
		});
	},	
	
	// private
	onTreeEditorComplete : function(editor,value,startValue)
	{
		var editNode = editor.editNode;
		if ( editNode.attributes["data"]==null )
		{
			// create new playlist
			this.createPlaylist(value,function(playlistData)
			{
				if ( playlistData!=null ) {
					editNode.attributes["data"] = playlistData;
					editNode.setText(playlistData.name);
				}
				else {
					editNode.remove();
				}
			});
		}
		else if ( value!=startValue )
		{
			// update existing playlist
			var playlistId = editNode.attributes["data"].playlistId;
			this.updatePlaylist(playlistId,value,function(playlistData)
			{
				if ( playlistData!=null ) {
					editNode.attributes["data"] = playlistData;
					editNode.setText(playlistData.name);
				}
				else {
					editNode.setText(startValue);
				}
			});
		}
	},
	
	// private
	createPlaylistNode : function(name)
	{
		var node = new Ext.tree.TreeNode({
			iconCls: "vibe-plugin-playlists-icon-playlist",
			text: name
		});
		
		return this.playlistTree.root.appendChild(node);
	},
	
	// private
	showLoadDialog : function()
	{
		var dlg = new vibe.plugin.playlists.PlaylistsDialog({
			data: this.getPlaylistData(),
			emptyText: vibe.Language.plugin.playlists.SELECT_PLAYLIST + "...",
			requireSelection: true,
			title: vibe.Language.plugin.playlists.LOAD_PLAYLIST
		});
		
		dlg.on("submit",function(name,playlistId)
		{
			this.playlistGrid.setBusy(true);
			this.playlistGrid.removeAllTracks();
			this.loadPlaylist(playlistId,function(records)
			{
				if ( records!=null ) {
					this.playlistGrid.insertTracks(records);
				}
				
				this.playlistGrid.setBusy(false);
			},
			this);
		},
		this);
		
		dlg.show();
	},
	
	// private
	showSaveDialog : function()
	{
		var dlg = new vibe.plugin.playlists.PlaylistsDialog({
			data: this.getPlaylistData(),
			emptyText: vibe.Language.plugin.playlists.ENTER_PLAYLIST_NAME + "...",
			height: 110,
			requireSelection: false,					
			title: vibe.Language.plugin.playlists.SAVE_PLAYLIST,
		    width: 300
		});
		
		dlg.on("submit",function(name,playlistId)
		{
			this.loadMask.show();
			this.playlistGrid.setBusy(true);

			var records = this.playlistGrid.getStore().getRange();
												
			if ( playlistId!=null )
			{
				// save existing playlist
				this.savePlaylist(playlistId,records,true,function() {
					this.playlistGrid.setBusy(false);
					this.loadMask.hide();
				},
				this);
			}
			else
			{
				// create new playlist
				this.createPlaylist(name,function(playlistData)
				{
					if ( playlistData!=null ) 
					{
						var newNode = this.createPlaylistNode(playlistData.name);
						newNode.attributes["data"] = playlistData;
						
						// save tracks to new playlist
						this.savePlaylist(playlistData.playlistId,records,false,function() {
							this.playlistGrid.setBusy(false);
							this.loadMask.hide();
						},
						this);
					}
					else
					{
						this.playlistGrid.setBusy(false);
						this.loadMask.hide();
					}					
				},
				this);
			}
		},
		this);
		
		dlg.show();
	},

	/**
	 * Load a playlist.
	 * @param {Number} playlistId The id of the playlist to load
	 * @param {Function} callback The callback method to be called when the playlist has loaded.
	 * @param {Object} scope The scope to use for the callback method
	 */
	loadPlaylist : function(playlistId,callback,scope)
	{
		var url = "plugins/playlists/resources/ajax/get-playlistitems.vibe";
		var params = "playlistId=" + playlistId;
		
		vibe.util.JsonLoader.load(url,params,function(result) {
			var loadedRecords = new Array();
			this.loadPlaylistRecords(result,loadedRecords,
				callback,scope);
		},
		this);
	},
	
	// private
	loadPlaylistRecords : function(items,loadedRecords,callback,scope)
	{
		if ( items!=null && items.length>0 )
		{
			var hashes = [];
			while ( items.length>0 )
			{
				var item = items.shift();
				hashes.push(item.hash);
				if ( hashes.length>=this.loadBuffer ) {
					break;
				}
			}

			var url = "resources/ajax/get-index-items.vibe";
			var params = "hashes=" + hashes.toString();
			
			vibe.util.JsonLoader.load(url,params,function(result) 
			{
				if ( result!=null ) {
					loadedRecords = loadedRecords.concat(vibe.util.IndexUtil.createRecords(result));
				}
				
				this.loadPlaylistRecords(items,loadedRecords,
					callback,scope);
			},
			this);		
		}
		else
		{
			if ( callback!=null && scope!=null ) {
				callback.call(scope,loadedRecords);
			}
			else if ( callback!=null ) {
				callback(loadedRecords);
			}
		}
	},
	
	// private
	savePlaylist : function(playlistId,records,clear,callback,scope)
	{
		this.loadMask.show();
		
		var hashes = new Array();
		
		while ( records.length>0 ) 
		{
			var record = records.shift();
			hashes.push(record.get("hash"));
			if ( hashes.length>=this.saveBuffer ) {
				break;
			}
		}
		
		if ( hashes.length>0 || clear==true )
		{
			var url = "plugins/playlists/resources/ajax/save-playlistitems.vibe";
			var params = "playlistId=" + playlistId + "&clear=" + clear + "&hashes=" + hashes.toString();

			vibe.util.JsonLoader.load(url,params,function(result) {
				this.savePlaylist(playlistId,records,false,callback,scope);
			},
			this);
		}
		else if ( records.length==0 ) 
		{
			this.loadMask.hide();
			
			if ( callback!=null && scope!=null ) {
				callback.call(scope);
			}
			else if ( callback!=null ) {
				callback();
			}
		}
	},
	
	// private
	createPlaylist : function(name,callback,scope)
	{
		this.loadMask.show();
		
		var url = "plugins/playlists/resources/ajax/create-playlist.vibe";
		var params = "name=" + encodeURIComponent(name);
		
		vibe.util.JsonLoader.load(url,params,function(result)
		{
			this.loadMask.hide();
			
			var playlistData = null;
			if ( result!=null && result.length>0 ) {
				playlistData = result[0];
			}
			
			if ( callback!=null && scope!=null ) {
				callback.call(scope,playlistData);
			}
			else if ( callback!=null ) {
				callback(playlistData);
			}
		},
		this);
	},	
	
	// private
	removePlaylist : function(playlistId,callback,scope)
	{
		this.loadMask.show();
		
		var url = "plugins/playlists/resources/ajax/remove-playlist.vibe";
		var params = "playlistId=" + playlistId;
		
		vibe.util.JsonLoader.load(url,params,function(result)
		{
			this.loadMask.hide();
			
			var success = result;
			
			if ( callback!=null && scope!=null ) {
				callback.call(scope,success);
			}
			else if ( callback!=null ) {
				callback(success);
			}
		},
		this);
	},
	
	// private
	updatePlaylist : function(playlistId,name,callback,scope)
	{
		this.loadMask.show();
		
		var url = "plugins/playlists/resources/ajax/update-playlist.vibe";
		var params = "playlistId=" + playlistId + "&name=" + encodeURIComponent(name);
		
		vibe.util.JsonLoader.load(url,params,function(result)
		{
			this.loadMask.hide();
			
			var playlistData = null;
			if ( result!=null && result.length>0 ) {
				playlistData = result[0];
			}
			
			if ( callback!=null && scope!=null ) {
				callback.call(scope,playlistData);
			}
			else if ( callback!=null ) {
				callback(playlistData);
			}
		},
		this);		
	},	
	
	// private
	updateTree : function()
	{
		var url = "plugins/playlists/resources/ajax/get-playlists.vibe";
		
		vibe.util.JsonLoader.load(url,null,function(result)
		{
			var root = this.playlistTree.root;
			while ( node = root.childNodes[0] ) {
				root.removeChild(node);
			}
			
			if ( result.length>0 )
			{
				root.beginUpdate();

				for ( var i=0, len=result.length; i<len; i++ )
				{
					var playlist = result[i];
					var node = new Ext.tree.TreeNode({
						iconCls: "vibe-plugin-playlists-icon-playlist",
						qtip: vibe.Language.plugin.playlists.DRAG_PLAYLIST_TO_LOAD,
						text: playlist.name
					});
					
					node.attributes["data"] = playlist;
					root.appendChild(node);
				}
				
				root.endUpdate();
			}
		},
		this);
	},	
	
	// private
	dropLoadCallback : function(records,args)
	{
		var playlistId = args[0];
		this.savePlaylist(playlistId,records,false);
	},
	
	// private
	getPlaylistData : function()
	{
		var data = [];
		
		var root = this.playlistTree.root;
		for ( var i=0,len=root.childNodes.length; i<len; i++ )
		{
			var node = root.childNodes[i];
			data.push([node.attributes["data"].name,
				node.attributes["data"].playlistId]);
		}
		
		return data;
	},

	// private
	getSupportedDDHelper : function(data)
	{
		if ( data.grid ) {
			return data.grid.ddHelper;
		}
		else if ( data.node ) {
			return data.node.getOwnerTree().ddHelper;
		}
		
		return null;
	}
});