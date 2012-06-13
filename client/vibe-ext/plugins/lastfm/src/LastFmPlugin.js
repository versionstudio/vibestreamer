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

Ext.namespace("vibe.plugin.lastfm");

Ext.onReady(function()
{
	vibe.Application.on("beforeload",function(app) {
		app.addPlugin(new vibe.plugin.lastfm.LastFmPlugin);
	});
});

/**
 * @class vibe.plugin.lastfm.LastFmPlugin
 * @extends vibe.plugin.Plugin
 * The last.fm plugin.
 * A simple implementation to make it possible to scrobble tracks.
 */
vibe.plugin.lastfm.LastFmPlugin = Ext.extend(vibe.plugin.Plugin,
{	
	// private
	panel : null,
		
	// private
	userNameField : null,
	
	// private
	passwordField : null,
	
	// private
	loginButton : null,
	
	// private
	statusField : null,
				
	// private
	trackTimeStartStamp : -1,
	
	// private
	trackPauseTimeStamp : -1,
	
	// private
	trackPauseDuration : 0,
	
	// private
	loggedOn : false,
	
	/**
	 * @override
	 */
	init : function(app)
	{
		// apply default plugin language
		Ext.applyIf(vibe.Language.plugin,{lastfm:[]});		
		Ext.applyIf(vibe.Language.plugin.lastfm,{
			CONNECTING: "Connecting to Last.fm",
			LOGGED_ON: "Logged on as <a target=\"vibewnd\" href=\"http://www.last.fm/user/{0}\">{0}</a>",
			LOGIN: "Login",
			LOGIN_FAILED: "Login failed - Try again",
			LOGIN_TO_START_SCROBBLING: "Login to start scrobbling",
			PASSWORD: "Password",
			TITLE: "Last.fm",
			USERNAME: "Username"
		});
	},
	
	/**
	 * @override
	 */
	startup : function(app)
	{
		var logoPanel = new Ext.Panel({
            border: false,
			html: "<img src=\"plugins/lastfm/resources/images/lastfm-logo.gif\"/>"
        });
		
		this.statusField = new Ext.Panel({
            border: false,				
           	cls: "x-form-item vibe-plugin-lastfm-status",
			html: vibe.Language.plugin.lastfm.LOGIN_TO_START_SCROBBLING,
            layout: "form"
        });
		
		this.userNameField = new Ext.form.TextField({
			fieldLabel: vibe.Language.plugin.lastfm.USERNAME,
			width: 112
		});
		
		this.passwordField = new Ext.form.TextField({
			fieldLabel: vibe.Language.plugin.lastfm.PASSWORD,
			inputType: "password",
			width: 112
		});
		
		this.loginButton = new Ext.Button({
			handler: this.onLoginButtonClick,
			scope: this,
			text: vibe.Language.plugin.lastfm.LOGIN				
		});
		
		this.statusPanel = new Ext.Panel({
			autoHeight: true,
			border: false,
			items: [logoPanel,
				this.statusField]
		});
		
		this.loginForm = new Ext.form.FieldSet({
			autoHeight: true,
			border: false,
			items: [this.userNameField,
				this.passwordField,
				this.loginButton]
		});			
		
		this.panel = new Ext.FormPanel({
			border: false,
			cls: "vibe-plugin-lastfm",
			collapsed: true,
			items: [this.statusPanel,
				this.loginForm
			],
			labelWidth: 61,
			title: vibe.Language.plugin.lastfm.TITLE
		});
		
		app.getPlayerPanel().on("trackpause", this.onPlayerPanelTrackPause, this);
		app.getPlayerPanel().on("trackresume", this.onPlayerPanelTrackResume, this);
		app.getPlayerPanel().on("trackstart", this.onPlayerPanelTrackStart, this);
		app.getPlayerPanel().on("trackstop", this.onPlayerPanelTrackStop, this);
		
		app.addModule(this.panel,"bottom");
	},	
	
	// private
	onLoginButtonClick : function(component)
	{
		var userName = this.userNameField.getValue();
		var password = this.passwordField.getValue();
		
		if ( userName.length==0 || password.length==0 ) {
			return;
		}
		
		component.disable();

		this.statusField.setVisible(true);
		this.statusField.body.update(vibe.Language.plugin.lastfm.CONNECTING + "...");
		
		var url = "plugins/lastfm/resources/ajax/request-handshake.vibe";
		var params = "username=" + userName + "&password=" + password;
		
		vibe.util.JsonLoader.load(url,params,function(result)
		{
			if ( result!=null && result.status=="OK" ) 
			{
				this.statusField.body.update(String.format(vibe.Language.plugin.lastfm.LOGGED_ON,userName));
				this.loginForm.setVisible(false);
				this.loggedOn = true;
			}
			else {
				this.statusField.body.update(vibe.Language.plugin.lastfm.LOGIN_FAILED);
				component.enable();
			}
		},
		this);
	},
	
	// private
	onPlayerPanelTrackPause : function(playerPanel,record)
	{
		this.trackPauseTimeStamp = parseInt(new Date().getTime()/1000);		
	},
	
	// private
	onPlayerPanelTrackResume : function(playerPanel,record)
	{
		var timeStamp = parseInt(new Date().getTime()/1000);
		var pauseDuration = timeStamp-this.trackPauseTimeStamp;
		
		this.trackPauseDuration += pauseDuration;
		this.trackPauseTimeStamp = -1;
	},	
		
	// private
	onPlayerPanelTrackStart : function(playerPanel,record)
	{
		this.trackStartTimeStamp = parseInt(new Date().getTime()/1000);

		if ( this.loggedOn ) 
		{
			if ( record.get("mdArtist")==null || record.get("mdTitle")==null ) {
				return;	
			}
			
			var url = "plugins/lastfm/resources/ajax/request-nowplaying.vibe";
			var params = "shareId=" + record.get("shareId") + "&hash=" + record.get("hash");
			vibe.util.JsonLoader.load(url,params,function(result)
			{
				if ( result==null || result.status!="OK" ) {
					// TODO: handle error
				}
			},
			this);
		}
	},
	
	// private
	onPlayerPanelTrackStop : function(playerPanel,record,completed)
	{
		var timeStamp = parseInt(new Date().getTime()/1000);
		var trackDuration = (timeStamp-this.trackStartTimeStamp)-this.trackPauseDuration;
				
		if ( this.loggedOn ) 
		{
			var length = record.get("mdLength");
			var i = this.trackStartTimeStamp;
	
			if ( record.get("mdArtist")==null || record.get("mdTitle")==null || i<1 ) {
				return;
			}
				
			// make sure track is valid for submission
			if ( length>30 && (trackDuration>240 || trackDuration>(length/2)) )
			{				
				var url = "plugins/lastfm/resources/ajax/request-submission.vibe";
				var params = "shareId=" + record.get("shareId") + "&hash=" + record.get("hash") + "&i=" + i;
				vibe.util.JsonLoader.load(url,params,function(result)
				{
					if ( result==null || result.status!="OK" ) {
						// TODO: handle error
					}
				},				
				this);
			}
		}
		
		this.trackStartTimeStamp = -1;
		this.trackPauseTimeStamp = -1;
		this.trackPauseDuration = 0;
	}
});