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

Ext.namespace("vibe.gui.playlist");

/**
 * @class vibe.gui.playlist.PlaylistGrid
 * @extends Ext.grid.GridPanel
 * Class representing the playlist grid.
 * @constructor
 * @param {vibe.gui.player.PlayerPanel} playerPanel
 * @param {Object} config (optional) the config object
 */
vibe.gui.playlist.PlaylistGrid = function(playerPanel,config) {
	this.playerPanel = playerPanel;
	vibe.gui.playlist.PlaylistGrid.superclass.constructor.call(this,config);
};

Ext.extend(vibe.gui.playlist.PlaylistGrid,Ext.grid.GridPanel,
{
	// private
	busy : false,
		
	// private
	dropTarget : null,
	
	// private
	loadMask : null,
	
	// private
	playerPanel : null,
	
	// private
	repeatEnabled : false,
	
	// private
	shuffleEnabled : false,
	
	// private
	shuffleHistory : [],
	
	// private
	shufflePosition : -1,

	// private
	shuffleStore : [],	
	
	/**
	 * @override
	 */
	initComponent : function()
	{
		this.addEvents(
			/**
			 * @event drop
			 * Fires when a drop is executed.
			 * It is up to the listener to find out whether the drop
			 * was triggered by itself and then do the drop as wanted.
			 * @param {vibe.gui.playlist.PlaylistGrid} this
			 * @param {Ext.dd.DragSource} source
			 * @param {Event} e
			 * @param {Object} data
			 */
			"drop",
		
			/**
			 * @event validatedrop
			 * Fires when a drop should be validated.
			 * It is up to the listener to find out whether the drop
			 * was triggered by itself and then return true if the drop is valid.
			 * @param {vibe.gui.playlist.PlaylistGrid} this
			 * @param {Ext.dd.DragSource} source
			 * @param {Event} e
			 * @param {Object} data
			 */
			"validatedrop"
		);
		
		var ds = new Ext.ux.grid.livegrid.Store({
			autoLoad: false,
			bufferSize: vibe.Config.app.GRID_BUFFER_SIZE
		});
	
		var view = new Ext.ux.grid.livegrid.GridView({
			forceFit: true,
			getRowClass : this.getRowClass,
			nearLimit: 100
		});
		
		var sm = new Ext.ux.grid.livegrid.RowSelectionModel();
		
		Ext.apply(this,
		{
			cm: new Ext.grid.ColumnModel([
				{ 
					dataIndex: "mdArtist",				
					header: vibe.Language.app.ARTIST,
					renderer : function(value,metadata,record,rowIndex,colIndex,store) {
						return (value==null || value.length==0 ? vibe.Language.app.UNKNOWN_ARTIST : value);
					},					
					sortable: false, 
					width: 170
				},
				{ 
					dataIndex: "mdTitle",
					header: vibe.Language.app.TITLE,
					renderer : function(value,metadata,record,rowIndex,colIndex,store) {
						return (value==null || value.length==0 ? record.get("name") : value);
					},
					sortable: false,
					width: 250
				},
				{
					dataIndex: "mdAlbum",
					header: vibe.Language.app.ALBUM,
					sortable: false,
					width: 170
				},
				{
					dataIndex: "mdTrack",
					header: vibe.Language.app.TRACK,
					hidden: true,
					sortable: false,
					width: 50
				},
				{ 
					dataIndex: "mdYear", 
					header: vibe.Language.app.YEAR, 
					hidden: true,					
					sortable: false,
					width: 50
				},
				{ 
					align: "right", 
					dataIndex: "mdLength",
					header: vibe.Language.app.TIME,
					renderer: function(value,metadata,record,rowIndex,colIndex,store) {
						return vibe.util.FormatUtil.duration(value);
					},
					sortable: false,
					width: 50
				}
			]),
			cls: "vibe-playlistgrid",
			ddGroup: vibe.Config.app.INDEX_DDGROUP,
			ddHelper: new vibe.gui.playlist.PlaylistGridDDHelper(),
			ddText: vibe.Language.app.DRAGDROP_TEXT,
			ds: ds,
			enableDragDrop: true,
			sm: sm,
			split: true,
			tbar: new Ext.Toolbar({
				height: 25
			}),
			view: view
		});
		
		vibe.gui.playlist.PlaylistGrid.superclass.initComponent.call(this);
		
		this.playerPanel.on("beforenext",this.onPlayerPanelBeforeNext,this);
		this.playerPanel.on("beforeprev",this.onPlayerPanelBeforePrev,this);

		this.on("celldblclick",this.onCellDblClick,this);
		this.on("keydown",this.onKeyDown,this);
		this.on("drop",this.onDrop,this);
		this.on("validatedrop",this.onValidateDrop,this);
	},
	
	/**
	 * @override
	 */
    onRender: function() 
	{
		vibe.gui.playlist.PlaylistGrid.superclass.onRender.apply(this,arguments);
				
		// create the drop target for handling drops and reordering
		this.dropTarget = new vibe.gui.playlist.PlaylistGridDropTarget(this,this.view.scroller,{
			ddGroup: vibe.Config.app.INDEX_DDGROUP
		});
		
		// adjust the grid background position
		this.view.liveScroller.on("scroll",function() {
			if ( this.view.rowIndex%2==0 ) {
				this.view.scroller.dom.style.backgroundPosition="0 0";
			}
			else {
				this.view.scroller.dom.style.backgroundPosition="0 -" + this.view.rowHeight;
			}
		},this);
		
		this.loadMask = new Ext.LoadMask(this.body,
			{ msg: vibe.Language.app.LOADMASK });
						
		var tbar = this.getTopToolbar();
		
		// insert "clear" button
		tbar.addButton(new Ext.Toolbar.Button({
			handler: function(button,e) {
				this.removeAllTracks();
			},
			scope: this,
			text: vibe.Language.app.CLEAR,
			tooltip: vibe.Language.app.REMOVE_ALL_TRACKS
		}));
		
		tbar.addSeparator();
		
		// insert "remove" button
		tbar.addButton(new Ext.Toolbar.Button({
			handler: function(button,e) {
				this.removeSelectedTracks();
			},
			scope: this,
			text: vibe.Language.app.REMOVE,
			tooltip: vibe.Language.app.REMOVE_SELECTED_TRACKS
		}));
		
		tbar.addSeparator();
		tbar.addFill();
		tbar.addSeparator();
		
		// insert "repeat" toggle button/checkbox				
		tbar.addButton(new Ext.Toolbar.Button({
			ctCls: "vibe-playlistgrid-customcheckbox",
			enableToggle: true,
			focus: false,
			listeners: 
			{
				"click": 
				{
					fn: function(component) {
						this.repeatEnabled = component.pressed;
					},
					scope: this	
				}
			},
			scope: this,
			text: vibe.Language.app.REPEAT
		}));
		
		tbar.addSeparator();
		
		// insert "shuffle" toggle button/checkbox				
		tbar.addButton(new Ext.Toolbar.Button({
			ctCls: "vibe-playlistgrid-customcheckbox",
			enableToggle: true,
			focus: false,
			listeners: 
			{
				"click": 
				{
					fn: function(component) {
						this.shuffleEnabled = component.pressed;
						this.resetShuffleStore();
					},
					scope: this	
				}
			},
			scope: this,
			text: vibe.Language.app.SHUFFLE
		}));
	},
	
	// private
	onCellDblClick : function(grid,rowIndex,columnIndex,e) 
	{
		var record = this.store.getAt(rowIndex);
		this.playerPanel.playTrack(record);
	},
	
	// private
	onKeyDown : function(e)
	{
		if ( e.getKey()==e.DELETE ) {
			this.removeSelectedTracks();
		}
	},
	
	// private
	onDrop : function(grid,source,e,data)
	{
		var ddHelper = this.getSupportedDDHelper(data);
		if ( ddHelper==null ) {
			return false;
		}
		
		this.setBusy(true);
		
		ddHelper.loadDrop(source,e,data,
			this.dropLoadCallback,this,null);
	},	
	
	// private
	onValidateDrop : function(grid,source,e,data)
	{
		var ddHelper = this.getSupportedDDHelper(data);
		if ( ddHelper==null ) {
			return false;
		}
		
		if ( this.isBusy() ) {
			return false;
		}

		return ddHelper.validateDrop(source,e,data);
	},
	
	// private
	onPlayerPanelBeforeNext : function(playerPanel,currentTrack)
	{
		var record = null;
				
		if ( this.shuffleEnabled )
		{
			if ( this.shufflePosition<this.shuffleHistory.length-1 ) {
				record = this.shuffleHistory[++this.shufflePosition];
			}
			else
			{
				if ( this.shuffleStore.length==0 && this.repeatEnabled ) {
					this.resetShuffleStore();
				}
				
				if ( this.shuffleStore.length>0 ) {
					record = this.shuffleStore[Math.round(Math.random()*(this.shuffleStore.length-1))];
					this.shuffleStore.remove(record);
					this.shuffleHistory.push(record);
					this.shufflePosition++;
				}
			}
		}
		else if ( currentTrack!=null ) 
		{
			var currentIndex = this.store.indexOf(currentTrack);
			if ( currentIndex!=-1 )
			{
				record = this.store.getAt(currentIndex+1);
				if ( record==null && this.repeatEnabled ) 
				{
					if ( this.store.getTotalCount()>0 ) {
						record = this.store.getAt(0);
					}
				}
			}
		}
		
		if ( record==null && currentTrack==null && this.store.getTotalCount()>0 ) {
			record = this.store.getAt(0);
		}
		
		if ( record!=null ) {
			playerPanel.playTrack(record);
			return false;
		}
		else {
			return true;
		}
	},
	
	// private
	onPlayerPanelBeforePrev : function(playerPanel,currentTrack)
	{
		if ( currentTrack==null ) {
			return true;
		}

		var record = null;
		
		if ( this.shuffleEnabled )
		{
			if ( this.shufflePosition>0 ) {
				record = this.shuffleHistory[--this.shufflePosition];
			}
		}
		else
		{
			var currentIndex = this.store.indexOf(currentTrack);
			if ( currentIndex!=-1 ) {
				record = this.store.getAt(currentIndex-1);
			}
		}
		
		if ( record!=null ) {
			playerPanel.playTrack(record);
			return false;
		}
		else {
			return true;
		}
	},	
	
	/**
	 * Insert the given track records that are supported into the grid and
	 * scrolls to the last inserted record.
	 * Records that are not supported by the player will not be inserted.
	 * @param {Mixed} records
	 * @return the number of records that was inserted successfully
	 */
	insertTracks : function(records)
	{
		// bug fix for when inserting a single track into 
		// an empty playlist grid
		if ( this.store.getTotalCount()==0 ) {
			this.removeAllTracks();
		}
		
		// remove all non-supported records
		for ( var i=0; i<records.length; )
		{
			var extension = vibe.util.StringUtil.getFileExtension(records[i].get("name"));
			var supported =  vibe.Config.app.PLAYER_FILETYPES.indexOf(extension.toLowerCase())!=-1;
			if ( supported ) {
				i++;
			}
			else {
				records.splice(i,1);
			}
		}
		
		// insert all records into grid
		var recordsLength = records.length;
		if ( recordsLength>0 )
		{
			var insertIndex = this.store.findInsertIndex(records);
			this.store.insert(insertIndex,records.reverse());
			this.view.ensureVisible(insertIndex+recordsLength-1,0,false);
			
			if ( this.shuffleEnabled ) 
			{
				for ( var i=0; i<recordsLength; i++ ) {
					this.shuffleStore.push(records[i]);
				}
			}
		}

		return recordsLength;
	},
	
	/**
	 * Remove all tracks.
	 */
	removeAllTracks : function()
	{
		this.store.removeAll();
		this.resetShuffleStore();
	},	
	
	/**
	 * Remove all selected tracks.
	 */
	removeSelectedTracks : function()
	{		
		var records = this.getSelectionModel().getSelections();
		
		var recordsLength = records.length;		
		if ( recordsLength==0 ) {
			return;
		}

		var bottomSelectedIndex = -1;
		for ( var i=0; i<recordsLength; i++ )
		{
			var record = records[i];
			
			// remove track record from shuffle store
			this.shuffleStore.remove(record);
			
			// check if shuffle history has been affected
			var shuffleIndex = this.shuffleHistory.indexOf(record);
			if ( shuffleIndex>-1 ) {
				this.shuffleHistory.splice(shuffleIndex,1);
				if ( shuffleIndex<=this.shufflePosition ) {
					this.shufflePosition--;	
				}
			}
			
			// find the bottom index of the selection
			var index = this.store.indexOf(record);
			if ( index>bottomSelectedIndex ) {
				bottomSelectedIndex = index;
			}
		};

		// find next track to select
		var nextRecord = this.store.getAt(bottomSelectedIndex+1);
					
		// remove all selected rows
		for ( var i=0; i<recordsLength; i++ ) {
			this.store.remove(this.store.getById(records[i].id));
		};
		
		// select the last track if no next track was found
		if ( nextRecord==null ) 
		{
			var totalCount = this.store.getTotalCount();
			if ( totalCount>0 ) {
				nextRecord = this.store.getAt(totalCount-1);
			}
		}
		
		// select next track
		if ( nextRecord!=null ) {
			this.getSelectionModel().selectRecords([nextRecord]);
		}
	},
	
	// private
	resetShuffleStore : function()
	{
		this.shuffleStore.length = 0;
		this.shuffleHistory.length = 0;
		this.shufflePosition = -1;
		
		if ( this.shuffleEnabled ) {
			for ( var i=0,len=this.store.getTotalCount(); i<len; i++ ) {
				this.shuffleStore.push(this.store.getAt(i));
			}
		}
	},
	
	// private
	dropLoadCallback : function(records,args)
	{
		if ( records!=null ) {
			this.insertTracks(records);
		}
		
		this.setBusy(false);
	},
		
	// private
	getRowClass : function(record,index,rowParams,store) 
	{
		var currentTrack = this.grid.playerPanel.getCurrentTrack();
		if ( currentTrack!=null && currentTrack==record ) {
			return "vibe-playlistgrid-currenttrack";
		}
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
	},
	
	/**
	 * Get whether the playlist grid is busy and should not
	 * have any more work done on it until ready.
	 * @return true if the playlist grid is busy
	 */
	isBusy : function() {
		return this.busy;
	},
	
	/**
	 * Set whether the playlist grid is busy.
	 * The load mask is visible when the playlist is busy.
	 */
	setBusy : function(busy)
	{
		this.busy = busy;
		if ( busy ) {
			this.loadMask.show();
		}
		else {
			this.loadMask.hide();
		}
	}
});