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

Ext.namespace("vibe.dd");

/**
 * @class vibe.dd.GridDropTarget
 * @extends Ext.dd.DropTarget
 * A drop target implementation that makes it possible to drop and re-order
 * items in a grid.
 * @constructor
 * @param {Mixed} grid the grid to handle dropping for
 * @param {Mixed} el the container element
 */
vibe.dd.GridDropTarget = function(grid,el,config) {
	this.grid = grid;
	vibe.dd.GridDropTarget.superclass.constructor.call(this,el,config);
};

Ext.extend(vibe.dd.GridDropTarget,Ext.dd.DropTarget,
{
    /**
     * @cfg {Ext.grid.GridPanel} allowReorder
     * Allow re-ordering by dragging items within the grid.
     */
	allowReorder : false,
	
	/**
	 * @override
	 */
	notifyDrop : function(source,e,data)
	{
		// check if drag is from within the grid
		if ( data.grid==this.grid )
		{
			if ( this.allowReorder )
			{
				var store = data.grid.getStore();			
				var selectedRows = data.grid.getSelectionModel().getSelections();
				
				// sort selected rows
				selectedRows.sort(function(a,b) {
					var x = store.indexOf(a);
					var y = store.indexOf(b);
					return ((x > y) ? -1 : ((x < y) ? 1 : 0));
				});
				
				store.bulkRemove(selectedRows);
				
				var targetIndex = this.getTargetIndex(source,e);
				if ( targetIndex==null ) {
					targetIndex = store.data.length;
				}
				
				// insert rows at new position
				store.insert(targetIndex,selectedRows);
											
				// select inserted rows
				data.grid.getSelectionModel().selectRecords(selectedRows);
				
				return true;
			}
			else {
				return false;
			}
		}
		else
		{
			if ( !this.validateDrop(source,e,data) ) {
				return false;
			}
			
			return this.drop(source,e,data);
		}
	},
	
	/**
	 * @override
	 */
	notifyEnter : function(source,e,data)
	{
		if ( data.grid==this.grid ) 
		{
			if ( this.allowReorder ) {
				return source.dropAllowed;
			}
			else {
				return source.dropNotAllowed;
			}
		}
		else
		{
			if (this.validateDrop(source,e,data)) {
				return source.dropAllowed;
			}
			else {
				return source.dropNotAllowed;
			}
		}
	},
	
	/**
	 * @override
	 */
	notifyOver : function(source,e,data) 
	{
		if (data.grid == this.grid) 
		{
			if ( this.allowReorder ) {
				return source.dropAllowed;
			}
			else {
				return source.dropNotAllowed;
			}
		}
		else 
		{
			if ( this.validateDrop(source,e,data) ) {
				return source.dropAllowed;
			}
			else {
				return source.dropNotAllowed;
			}
		}
	},

	/**
	 * This method is called when an item is dropped on the grid.
	 * Subclasses should override this.
     * @param {Ext.dd.DragSource} source The drag source that was dragged
     * @param {Event} e The event
     * @param {Object} data An object containing arbitrary data supplied by the drag source
     * @return false if drop is not valid
	 */
	drop : function(source,e,data) {
		return false;
	},
	
	/**
	 * This method is called to validate before a drop.
	 * Subclasses should override this.
     * @param {Ext.dd.DragSource} source The drag source that was dragged
     * @param {Event} e The event
     * @param {Object} data An object containing arbitrary data supplied by the drag source
     * @return false is drop is not valid
	 */
	validateDrop : function(source,e,data) {
		return false;
	},
	
	// private
	getTargetIndex : function(source,e)
	{
		var targetIndex = source.getDragData(e).rowIndex;
		
		// bug fix for IE
		if ( targetIndex==null && Ext.isIE ) {					
			var x = Ext.lib.Event.getXY(e)[0];
			var y = Ext.lib.Event.getXY(e)[1];
			var dropEl = document.elementFromPoint(x,y);
			targetIndex =  this.grid.getView().findRowIndex(dropEl);
		}
		
		return targetIndex;	
	}
});