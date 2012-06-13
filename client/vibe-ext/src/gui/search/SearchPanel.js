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

Ext.namespace("vibe.gui.search");

/**
 * @class vibe.gui.search.SearchPanel
 * @extends Ext.Panel
 * Class representing the search panel.
*/
vibe.gui.search.SearchPanel = Ext.extend(Ext.Panel,
{
	// private
	searchField : null,
	
	/**
	 * @override
	 */
	initComponent : function()
	{
		this.addEvents(
			/**
			 * @event search
			 * Fires when a playlist is added.
			 * @param {vibe.gui.search.SearchPanel} this
			 * @param {String} expression
			 */
			"search"
		);
				
		this.searchField = new Ext.form.TextField({
			enableKeyEvents: true,
			emptyText: vibe.Language.app.SEARCH + "...",
			height: 20,
			width: 150
		});
		
		Ext.apply(this,{
			border: false,
			cls: "vibe-searchpanel",
			items: [this.searchField]
		});
		
		vibe.gui.search.SearchPanel.superclass.initComponent.call(this);

		this.searchField.on("keydown",this.onSearchFieldKeydown,this);
	},
		
	// private
	onSearchFieldKeydown : function(component,e)
	{
		if ( e.keyCode==13 ) {
			this.fireEvent("search",this,this.searchField.getValue());
		}
	}
});