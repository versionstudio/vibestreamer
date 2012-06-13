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

/**
 * @vibe.plugin.playlists.PlaylistsDDHelper
 * @extends vibe.dd.DDHelper
 * DDHelper used by the playlists plugin.
 * @constructor
 * @param {vibe.plugin.playlists.Playlists} playlistPlugin
 * @param {vibe.gui.playlist.PlaylistGrid} playlistGrid
 */
vibe.plugin.playlists.PlaylistsDDHelper = function(playlistsPlugin,playlistGrid) {
	this.playlistsPlugin = playlistsPlugin;
	this.playlistGrid = playlistGrid;
	vibe.plugin.playlists.PlaylistsDDHelper.superclass.constructor.call(this);
};

Ext.extend(vibe.plugin.playlists.PlaylistsDDHelper,vibe.dd.DDHelper,
{
	/**
	 * @override
	 */
	validateDrop : function(source,e,data) {
		return true;
	},
	
	/**
	 * @override
	 */
    loadDrop : function(source,e,data,callback,scope,args) 
	{
		// since playlists can only be 
		// dropped on the playlist grid
		this.playlistGrid.removeAllTracks();
		
		var playlistId = data.node.attributes["data"].playlistId;
		this.playlistsPlugin.loadPlaylist(playlistId,function(records) 
		{
			if ( callback!=null && scope!=null ) {
				callback.call(scope,records,args);
			}
			else if ( callback!=null ) {
				callback(records,args);
			}
		},
		this);
    }
});