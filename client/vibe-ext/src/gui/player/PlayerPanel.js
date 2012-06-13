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

Ext.namespace("vibe.gui.player");

/**
 * @class vibe.gui.player.PlayerPanel
 * @extends Ext.Panel
 * Class representing the player panel.
 */
vibe.gui.player.PlayerPanel = Ext.extend(Ext.Panel,
{
	// private
	currentTrack : null,
		
	/**
	 * @override
	 */
	initComponent : function()
	{
		this.addEvents(
			/**
			 * @event beforenext
			 * Fired when the player requests the next track to play.
			 * The event listener is responsible to call the playTrack
			 * method and return 'false' to end the request, if a track was found.
			 * @param {vibe.gui.player.PlayerPanel} this
			 * @param {vibe.util.IndexLoader.Record} The current playing track
			 */
			"beforenext",
			
			/**
			 * @event beforeprev
			 * Fired when the player requests the previous track to play.
			 * The event listener is responsible to call the playTrack
			 * method and return 'false' to end the request, if a track was found.
			 * @param {vibe.gui.player.PlayerPanel} this
			 * @param {vibe.util.IndexLoader.Record} The current playing track
			 */
			"beforeprev",			
					
			/**
			 * @event trackpause
			 * Fires when the player pauses a track.
			 * @param {vibe.gui.player.PlayerPanel} this
			 * @param {vibe.util.IndexLoader.Record} The track record that was paused.
			 */
			"trackpause",
			
			/**
			 * @event trackresume
			 * Fires when the player resumes a track.
			 * @param {vibe.gui.player.PlayerPanel} this
			 * @param {vibe.util.IndexLoader.Record} The track record that was resumed.
			 */
			"trackresume",
			
			/**
			 * @event trackstart
			 * Fires when the player starts playing a track.
			 * @param {vibe.gui.player.PlayerPanel} this
			 * @param {vibe.util.IndexLoader.Record} The track record that was started.
			 */
			"trackstart",
			
			/**
			 * @event trackstop
			 * Fires when the player stops playing a track.
			 * @param {vibe.gui.player.PlayerPanel} this
			 * @param {vibe.util.IndexLoader.Record} The track record that was stopped.
			 */
			"trackstop"
		);
		
		Ext.apply(this,{
			border: false,
			cls: "vibe-playerpanel"
		});
		
		vibe.gui.player.PlayerPanel.superclass.initComponent.call(this);
	},
	
	/**
	 * @override
	 */
    onRender: function()
	{
		vibe.gui.player.PlayerPanel.superclass.onRender.apply(this,arguments);
		
		var session = vibe.util.ResourceManager.getData("vibe.session");
		if ( session.settings.theme!=null )
		{
			var swfUrl = "themes/" + session.settings.theme + "/resources/swf/build/flashplayer.swf";
			var swfObject = new SWFObject(swfUrl,"flashplayer",this.width,this.height,"9");
			swfObject.addParam("wmode","transparent");
			swfObject.addVariable("bufferTime",session.settings.playerBufferTime);
			swfObject.addVariable("callbackObject","vibe.Application.getPlayerPanel()");
			swfObject.write(this.el.id);
		}
	},
	
	/**
	 * Play a given track record.
	 * @param {vibe.util.IndexLoader.Record} record the track record to play
	 */
	playTrack : function(record)
	{		
		var hash = record.get("hash");
		var artist = record.get("mdArtist");
		var title = record.get("mdTitle");
		var bitRate = record.get("mdBitRate")
		
		var trackDetails = null;
		if ( artist!=null && artist.length>0 ) {
			trackDetails = artist + " - " + title;
		}
		else if ( title!=null && title.length>0 ) {
			trackDetails = title;
		}
		else {
			trackDetails = record.get("name");
		}
		
		var url = vibe.Config.app.SHARE_URI + hash + "?play";
		var session = vibe.util.ResourceManager.getData("vibe.session");
		if ( session.settings.allowCaching=="true" ) {
			url += "&allowcaching";
		}
		
		Ext.get("flashplayer").dom.playTrack(url,trackDetails,bitRate);
		
		var oldCurrentTrack = null;
		if ( this.currentTrack!=null ) {
			oldCurrentTrack = this.currentTrack;
		}
		
		this.currentTrack = record;
		this.currentTrack.commit();
		
		if ( oldCurrentTrack!=null ) {
			oldCurrentTrack.commit();
		}
	},
	
	/**
	 * Callback function used by the flash application.
	 */
	flashPlayerNextClick : function() {
		this.fireEvent("beforenext",this,this.currentTrack);
	},
	
	/**
	 * Callback function used by the flash application.
	 */
	flashPlayerPlayClick : function() {
		this.fireEvent("beforenext",this,this.currentTrack);
	},
	
	/**
	 * Callback function used by the flash application.
	 */
	flashPlayerPrevClick : function() {
		this.fireEvent("beforeprev",this,this.currentTrack);
	},
		
	/**
	 * Callback function used by the flash application.
	 */
	flashPlayerTrackPause : function() {
		this.fireEvent("trackpause",this,this.currentTrack);
	},
	
	/**
	 * Callback function used by the flash application.
	 */
	flashPlayerTrackResume : function() {
		this.fireEvent("trackresume",this,this.currentTrack);
	},	
		
	/**
	 * Callback function used by the flash application.
	 */
	flashPlayerTrackStart : function() {
		this.fireEvent("trackstart",this,this.currentTrack);
	},
	
	/**
	 * Callback function used by the flash application.
	 * @param {Boolean} completed true if the track has completed
	 */
	flashPlayerTrackStop : function(completed)
	{
		this.fireEvent("trackstop",this,this.currentTrack,completed);
		
		if ( completed )
		{
			if ( this.fireEvent("beforenext",this,this.currentTrack) )
			{
				var oldCurrentTrack = this.currentTrack;
				this.currentTrack = null;
				if ( oldCurrentTrack!=null ) {
					oldCurrentTrack.commit();
				}
			}
		}
	},
	
	/**
	 * Get the current playing track.
	 * @return {vibe.util.IndexUtil.Record} the current playing track
	 */
	getCurrentTrack : function() {
		return this.currentTrack;
	}
});