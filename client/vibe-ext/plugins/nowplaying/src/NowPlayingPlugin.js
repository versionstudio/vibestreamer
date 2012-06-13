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

Ext.namespace("vibe.plugin.nowplaying");

Ext.onReady(function()
{
	vibe.Application.on("beforeload",function(app) {
		app.addPlugin(new vibe.plugin.nowplaying.NowPlayingPlugin);
	});
});

/**
 * @class vibe.plugin.nowplaying.NowPlayingPlugin
 * @extends vibe.plugin.Plugin
 * The now playing plugin.
 * Displays the currently playing track as well as fetches the cover image.
 */
vibe.plugin.nowplaying.NowPlayingPlugin = Ext.extend(vibe.plugin.Plugin,
{	
	// private
	panel : null,
	
	// private
	lastAlbum : null,
	
	// private
	lastArtist : null,

	/**
	 * @override
	 */
	init : function(app)
	{
		// apply default plugin config
		Ext.applyIf(vibe.Config.plugin,{nowplaying:[]});
		Ext.applyIf(vibe.Config.plugin.nowplaying,{
			NOCOVER_IMAGE: "plugins/nowplaying/resources/images/nocover.gif"
		});
		
		// apply default plugin language
		Ext.applyIf(vibe.Language.plugin,{nowplaying:[]});		
		Ext.applyIf(vibe.Language.plugin.nowplaying,{
			ALBUM: "Album",
			ARTIST: "Artist",
			TITLE: "Title",
			UNKNOWN: "Unknown"
		});		
	},
	
	/**
	 * @override
	 */
	startup : function(app)
	{
		this.panel = new Ext.Panel({
			autoScroll: true,
			border: false,
			collapsed: true,
			title: "Now Playing"
		});
		
		this.panel.on("render",this.onPanelRender,this);
		
		app.getPlayerPanel().on("trackstart",this.onPlayerPanelTrackStart,this);
		
		app.addModule(this.panel,"top");
	},
	
	// private
	onPanelRender : function(component)
	{
		component.body.hide();
		
		Ext.DomHelper.insertFirst(component.body,
			"<div class=\"vibe-plugin-nowplaying-cover\"><img src=\"" + Ext.BLANK_IMAGE_URL + "\"&></div>" +
			"<div class=\"vibe-plugin-nowplaying-header\">" + vibe.Language.plugin.nowplaying.TITLE + "</div>" +
			"<div class=\"vibe-plugin-nowplaying-field\"></div>" +
			"<div class=\"vibe-plugin-nowplaying-header\">" + vibe.Language.plugin.nowplaying.ARTIST + "</div>" +
			"<div class=\"vibe-plugin-nowplaying-field\"></div>" +
			"<div class=\"vibe-plugin-nowplaying-header\">" + vibe.Language.plugin.nowplaying.ALBUM + "</div>" +
			"<div class=\"vibe-plugin-nowplaying-field\"></div>");
			
		var el = this.panel.body.dom;
		var imageEl = el.childNodes[0].childNodes[0];
		
		Ext.EventManager.on(imageEl,"error",function() {
			Ext.get(imageEl).hide();
			imageEl.src = vibe.Config.plugin.nowplaying.NOCOVER_IMAGE;
		});
		
		Ext.EventManager.on(imageEl,"load",function() {
			Ext.get(imageEl).show();
		});
	},
	
	// private
	onPlayerPanelTrackStart : function(playerPanel,record)
	{
		this.panel.body.show();
				
		var shareId = record.get("shareId");		
		var hash = record.get("hash");
		
		var album = record.get("mdAlbum") || vibe.Language.plugin.nowplaying.UNKNOWN;
		var artist = record.get("mdArtist") || vibe.Language.plugin.nowplaying.UNKNOWN;
		var title = record.get("mdTitle") || record.get("name");
		var year = record.get("mdYear");
				
		if ( year!=null && year>0 ) {
			album += " (" + year + ")";	
		}
				
		var el = this.panel.body.dom;
		var imageEl = el.childNodes[0].childNodes[0];
				
		Ext.DomHelper.overwrite(el.childNodes[2],title);
		Ext.DomHelper.overwrite(el.childNodes[4],artist);		
		Ext.DomHelper.overwrite(el.childNodes[6],album);
					
		if ( this.lastAlbum==null || this.lastArtist==null || 
			album!=this.lastAlbum || artist!=this.lastArtist )
		{
			var url = "plugins/nowplaying/resources/ajax/get-cover.vibe";
			var params = "shareId=" + shareId + "&hash=" + hash;
			
			vibe.util.JsonLoader.load(url,params,function(result)
			{
				if ( result!=null && result.hash!=null ) {
					imageEl.src = vibe.Config.app.SHARE_URI + result.hash;
				}
				else {
					imageEl.src = "plugins/nowplaying/resources/ajax/cover.vibe?" + params;
				}
				
				this.lastAlbum = album;
				this.lastArtist = artist;
			},
			this);
		}
	}
});