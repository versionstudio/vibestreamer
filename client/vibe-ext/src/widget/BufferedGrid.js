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
 * @class vibe.widget.BufferedGrid
 * @extends Ext.grid.GridPanel
 * A buffered grid that wrapping around Ext-ux Livegrid for displaying
 * items in a grid in a file-view structured grid.
 * @constructor
 * @param {Object} config (optional) the config object
 */
vibe.widget.BufferedGrid = Ext.extend(Ext.grid.GridPanel,
{
	/**
	 * @override
	 */
	initComponent : function()
	{
		var ds = new Ext.ux.grid.livegrid.Store({
			autoLoad: false,
			bufferSize: vibe.Config.app.GRID_BUFFER_SIZE
		});
			
		var view = new Ext.ux.grid.livegrid.GridView({
			forceFit: true,
			getRowClass : this.getRowClass,
			loadMask: { msg: vibe.Language.app.LOADMASK },
			nearLimit: 100
		});
		
		var sm = new Ext.ux.grid.livegrid.RowSelectionModel();
				
		Ext.apply(this,{
			cls: "vibe-bgrid",
			ds: ds,
			sm: sm,
			view: view
		});
		
		vibe.widget.BufferedGrid.superclass.initComponent.call(this);
	},
		
	/**
	 * Show records in the grid.
	 * @param {Object} records the records that should be displayed.
	 */
	showRecords : function(records)
	{
		records.reverse();
		
		this.store.removeAll();
		this.store.insert(0,records);
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
	
	// private
	getRowClass : function(record,index,rowParams,store)
	{
		var iconCls = this.grid.getIconClass(record,index);
		if ( iconCls!=null ) {
			return "vibe-bgrid-icon " + iconCls;
		}
		else {
			return "vibe-bgrid-icon";
		}
	}
});