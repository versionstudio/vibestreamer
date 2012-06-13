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

Ext.namespace("vibe");

/**
 * @class vibe.Language
 * Language properties container class.
 * This class is used as a container for both application
 * and plugin language settings.
 */
vibe.Language = function()
{
	return {
		/**
		 * Language properties for the application.
		 */
		app : {
	  		ALBUM: "Album",
			ARTIST: "Artist",
			BROWSE_ONLY: "Your account is limited to browsing files only.",
			CANCEL: "Cancel",
			CHECKED_SETTINGS_ARE_SPECIFIC: "Checked settings are specific to the user/group. Non-checked settings are inherited.",
			CLEAR: "Clear",
			CLOSE: "Close",
			CONNECTION_LOST : "Connection to server lost...",
			DATE_FORMAT_LONG: "Y-m-d h:i",
			DATE_FORMAT_SHORT: "Y-m-d",
			DATE_MODIFIED: "Date Modified",
			DRAGDROP_TEXT: "{0} selected item(s)",
			ERROR: "Error",
			EVERYONE: "Everyone",
			EXPLORER: "Explorer",
			FORUM: "Forum",
			LOADMASK: "Please wait...",
			LOGOUT: "Logout",
			MISSING_RESOURCE: "Could not find '{0}' resource",			
			NAME: "Name",
			OK: "OK",
			PERMISSION_DENIED: "Permission denied",
			REMOVE: "Remove",
			REMOVE_ALL_TRACKS: "Remove all tracks from queue",
			REMOVE_SELECTED_TRACKS: "Remove selected track(s) from queue",
			REPEAT: "Repeat",
			SAVE: "Save",
			SEARCH: "Search",
			SETTINGS: "Settings",			
			SHARES: "Shares",
			SHUFFLE: "Shuffle",
			SIZE: "Size",
			STATUS_MSG: "Displaying {0} - {1} of {2} items",
			TIME: "Time",
			TITLE: "Title",
			TRACK: "Track",
			UNKNOWN_ARTIST: "Unknown artist",
			UNTITLED_PLAYLIST: "Untitled Playlist",
			YEAR: "Year"
		},
		
		/**
		 * Language properties for plugins.
		 */
		plugin : {}
	};
}();