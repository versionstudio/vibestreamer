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
 * @class PlaylistGridDropTarget
 * @extends vibe.dd.GridDropTarget
 * Drop target class for usage with the playlist grid.
 */
vibe.gui.playlist.PlaylistGridDropTarget = Ext.extend(vibe.dd.GridDropTarget,
{
	/**
	 * @override
	 */
	allowReorder: true,
	
	/**
	 * @override
	 */
	drop : function(source,e,data) {
		return this.grid.fireEvent("drop",this.grid,source,e,data);
	},
	
	/**
	 * @override
	 */
	validateDrop : function(source,e,data) {
		return this.grid.fireEvent("validatedrop",this.grid,source,e,data);
	}
});