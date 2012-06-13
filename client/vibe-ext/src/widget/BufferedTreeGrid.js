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

Ext.namespace("vibe.widget");

/**
 * @class vibe.widget.BufferedTreeGrid
 * @extends Ext.grid.GridPanel
 * A buffered tree grid that wrapping around Ext-ux Livegrid for displaying
 * items in a tree-structured grid.
 * @constructor
 * @param {Object} config (optional) the config object
 */
vibe.widget.BufferedTreeGrid = Ext.extend(Ext.grid.GridPanel,
{
	/**
	 * @override
	 */
	initComponent : function()
	{
		this.addEvents(
			/**
			 * @event beforeitemcollapse
			 * Fires before an expandable item row is collapsed.
			 * @param {vibe.widget.BufferedTreeGrid} this
			 * @param {Number} rowIndex
			 */
			"beforeitemcollapse",
			
			/**
			 * @event beforeitemexpand
			 * Fires before an expandable item row is expanded.
			 * @param {vibe.widget.BufferedTreeGrid} this
			 * @param {Number} rowIndex
			 */
			"beforeitemexpand",
			
			/**
			 * @event itemclick
			 * Fires when an item row is clicked.
			 * @param {vibe.widget.BufferedTreeGrid} this
			 * @param {Number} rowIndex
			 */
			"beforeitemexpand",
			
			/**
			 * @event itemcollapse
			 * Fires when an expandable item row is collapsed.
			 * @param {vibe.widget.BufferedTreeGrid} this
			 * @param {Number} rowIndex
			 */
			"itemcollapse",
			
			/**
			 * @event itemexpand
			 * Fires when an expandable item row is expanded.
			 * @param {Number} rowIndex
			 */
			"itemexpand"									
		);
				
		var ds = new Ext.ux.grid.livegrid.Store({
			autoLoad: false,
			bufferSize: vibe.Config.app.GRID_BUFFER_SIZE,
			grid: this // required for the icon renderer to access the grid
		});
			
		var view = new Ext.ux.grid.livegrid.GridView({
			forceFit: true,
			loadMask: { msg: vibe.Language.app.LOADMASK },
			nearLimit: 100
		});
		
		var sm = new Ext.ux.grid.livegrid.RowSelectionModel();
				
		Ext.apply(this,{
			cls: "vibe-btgrid",
			ds: ds,
			sm: sm,
			view: view
		});
		
		vibe.widget.BufferedTreeGrid.superclass.initComponent.call(this);
		
		this.getColumnModel().setRenderer(0,this.renderIconColumn);
		
		this.on("rowclick",this.onRowClick,this);
	},
	
	/**
	 * Insert records in the grid.
	 * @param {Object} records the records that should be added
	 * @param {Object} rowIndex the row that the records should be children of
	 */
	insertRecords : function(records,rowIndex)
	{
		var level = 0;
		var slowInserts = 0;
		
		if ( rowIndex!=-1 ) 
		{
			level = this.store.getAt(rowIndex).get("level")+1;

			// due to a bug in livegrid the possible number of rows visible
			// in the grid will be inserted separately one by one,
			// while the rest will be bulk-inserted as normal
			slowInserts = Math.floor(this.getSize(true).height/this.view.rowHeight);			
		}
		else {
			this.store.removeAll();
		}
		
		for ( var i=0,len=records.length;i<len; i++ ) 
		{
			records[i].set("level",level);
			
			if ( i<slowInserts ) {
				this.store.insert(rowIndex+1+i,records[i]);
			}
		}
		
		if ( slowInserts>0 ) {
			records.splice(0,slowInserts);
		}

		if ( records.length>0 ) {
			records.reverse();			
			this.store.insert(rowIndex+slowInserts+1,records);
		}
	},
	
	/**
	 * Get the css class name of the icon to be displayed when
	 * the given record is being rendered.
	 * This function can be overriden for custom icons to be rendered for each record.
	 * @param {vibe.util.IndexLoader.Record} record the record being rendered
	 * @param {Integer} index the row index for the record being rendered
	 * @return the css icon class to be rendered for the record
	 */
	getIconClass : function(record,index)	{
		return null;
	},
	
	/**
	 * Get whether the given record being rendered is expandable
	 * and should be rendered with a plus/minus elbow.
	 * This function should be overridden and return true if the rendered
	 * record should be able to be expanded.
	 * @param {vibe.util.IndexLoader.Record} record the record being rendered
	 * @param {Integer} index the row index for the record being rendered
	 * @return true if the currently rendered record should be expandable
	 */
	isExpandable : function(record,index) {
		return false;
	},
	
	// private
	renderIconColumn : function(value,metadata,record,rowIndex,colIndex,store)
	{
		var level = record.get("level");
		var margin = level*16;
		
		var elbowCls = "vibe-btgrid-elbow-none";
		var iconCls = store.grid.getIconClass(record,rowIndex);
		var expandable = store.grid.isExpandable(record,rowIndex);
			
		if ( expandable )
		{
			var expanded = record.get("expanded");
			if ( expanded==true ) {
				elbowCls = "vibe-btgrid-elbow-minus";
			}
			else {
				elbowCls = "vibe-btgrid-elbow-plus";					
			}
		}
		
		if ( iconCls!=null ) {
			return "<div class=\"" + elbowCls + "\" style=\"margin:0 0 0 " + margin + "px\">" +
				"<div class=\"vibe-btgrid-icon " + iconCls + "\">" + value + "</div></div>";
		}
		else {
			return "<div class=\"" + elbowCls + "\" style=\"margin:0 0 0 " + margin + "px\">" +
				"<div class=\"vibe-btgrid-icon\">" + value + "</div></div>";			
		}
	},
	
	// private
	expandRow : function(rowIndex)
	{
		var record = this.store.getAt(rowIndex);
		if ( record!=null && this.fireEvent("beforeitemexpand",this,rowIndex) )
		{
			record.set("expanded",true);
			record.commit();
			
			this.fireEvent("itemexpand",this,rowIndex);
		}
	},
	
	// private
	collapseRow : function(rowIndex)
	{
		var record = this.store.getAt(rowIndex);
		if ( record!=null && this.fireEvent("beforeitemcollapse",this,record,rowIndex) )
		{
			record.set("expanded",false);
			record.commit();
			
			var level = record.get("level");
			var childRecords = new Array();
			
			// find all "childrecords" that should be removed
			for ( var i=rowIndex+1, len=this.store.data.length; i<len; i++ ) {
				var childRecord = this.store.getAt(i);
				if ( childRecord.get("level")<=level ) {
					break;
				}
				childRecords.push(childRecord);				
			}

			this.store.bulkRemove(childRecords);
			this.fireEvent("itemcollapse",this,rowIndex);
		}
	},	
	
	// private
	onRowClick : function(grid,rowIndex,e)
	{
		var target = e.getTarget();
		
		if ( target.className=="vibe-btgrid-elbow-plus" ) {
			this.expandRow(rowIndex);			
		}
		else if ( target.className=="vibe-btgrid-elbow-minus" ) {
			this.collapseRow(rowIndex);
		}
		else {
			this.fireEvent("itemclick",grid,rowIndex);
		}
	}	
});