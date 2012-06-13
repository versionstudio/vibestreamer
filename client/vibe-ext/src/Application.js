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
 * @class vibe.Application
 * Singleton class for the Vibe Streamer web client application.
 */
vibe.Application = function()
{
	return Ext.apply(new Ext.util.Observable(),
	{
		events : [
			/**
			 * @event beforeload
			 * Fires before the application is loaded.
			 * @param {vibe.Application} this
			 */
			"beforeload",		
					
			/**
			 * @event beforestartup
			 * Fires before the application is started.
			 * @param {vibe.Application} this
			 */
			"beforestartup",
			
			/**
			 * @event load
			 * Fires when after the application has loaded.
			 * @param {vibe.Application} this
			 */
			"load",			
			
			/**
			 * @event startup
			 * Fires after the application has been started.
			 * @param {vibe.Application} this
			 */				
			"startup"
		],
		
		// private
		connected : true,
		
		// private
		explorerPanel : null,
		
		// private
		modulePanel : null,
		
		// private
		playerPanel : null,
								
		// private
		playlistGrid : null,
		
		// private
		plugins : [],
		
		// private
		searchPanel : null,
		
		// private
		statusBar : null,
		
		// private
		viewPort : null,
		
		/**
		 * Initializes the application - must be run first.
		 */
		init: function() 
		{
			Ext.BLANK_IMAGE_URL = "resources/images/blank.gif";
			Ext.SSL_SECURE_URL = "blank.html";
			Ext.state.Manager.setProvider(new Ext.state.CookieProvider());
			Ext.QuickTips.init();
			
			if ( this.fireEvent("beforeload",this) )
			{
				var resourceManager = vibe.util.ResourceManager;
	
				// bind application resources
				resourceManager.bindResource("vibe.session","resources/ajax/get-session.vibe");
				resourceManager.bindResource("vibe.server","resources/ajax/get-server.vibe");
				resourceManager.bindResource("vibe.site","resources/ajax/get-site.vibe");
				
				// initialize all plugins
				for ( var i=0,len=this.plugins.length; i<len; i++ ) {
					this.plugins[i].init(this);
				}		
				
				// startup application when all resources are loaded
				resourceManager.addListener("updateall",function() 
				{
					// make sure all mandatory resources are loaded
					if (resourceManager.getData("vibe.session")==null) {
						Ext.Msg.alert(vibe.Language.app.ERROR, String.format(vibe.Language.app.MISSING_RESOURCE,"vibe.session"));
					}
					else if (resourceManager.getData("vibe.server")==null) {
						Ext.Msg.alert(vibe.Language.app.ERROR, String.format(vibe.Language.app.MISSING_RESOURCE,"vibe.server"));
					}
					else if (resourceManager.getData("vibe.site")==null) {
						Ext.Msg.alert(vibe.Language.app.ERROR, String.format(vibe.Language.app.MISSING_RESOURCE,"vibe.site"));
					}
					else
					{
						if ( this.fireEvent("load",this) ) {
							this.startup();
						}
					}
				},
				this,{ single: true })
				
				resourceManager.updateAll();
			}
		},
		
		// private
		startup : function()
		{
			if ( this.fireEvent("beforestartup",this) )
			{
				var resourceManager = vibe.util.ResourceManager;
				var session = resourceManager.getData("vibe.session");

				this.playerPanel = new vibe.gui.player.PlayerPanel({
					cellCls: "vibe-headerpanel-col1",
					height: 50,
					width: 666
				});

				this.searchPanel = new vibe.gui.search.SearchPanel({
					cellCls: "vibe-headerpanel-col2",
					height: 50
				});

				this.playlistGrid = new vibe.gui.playlist.PlaylistGrid(this.playerPanel,{
					region: "center",
					stateId: "vibe-playlistgrid"
				});					

				this.explorerPanel = new vibe.gui.explorer.ExplorerPanel(this.searchPanel,this.playlistGrid,{
					collapsed: false,
				    height: 250,
				    minSize: 50,
					region: "south",
					split: true,
					stateId: "vibe-explorerpanel"
				});

				var logoPanel = new Ext.Panel({
					border: false,
					cellCls: "vibe-headerpanel-col3",
					cls: "vibe-logopanel",
					html: "<a href=\"http://www.vibestreamer.com/\" target=\"_blank\"><div class=\"vibe-logopanel-logo\"></div></a>"
				});			

				var headerPanel = new Ext.Panel({
					border: false,
					cls: "vibe-headerpanel",
				    layoutConfig: {
				        columns: 3
				    },						
					height: 50,
					items: [this.playerPanel,
						this.searchPanel,
						logoPanel
					],
					layout: "table",
					region: "north"
				});

				this.modulePanel = new Ext.Panel({
					cls: "vibe-modulepanel",
					cmargins: "5 5 0 5",
					collapsible: true,
					layout: "accordion",
					layoutConfig: { animate:true },			
					minSize: 200,
					margins: "5 0 0 5",
					maxSize: 400,
					region: "west",
					split: true,
					stateId: "vibe-modulepanel",
					title: "Vibe Streamer",
					width: 200
				});				

				var centerPanel = new Ext.Panel({
					border: false,
					cls: "vibe-centerpanel",
					items: [this.playlistGrid,
						this.explorerPanel],
					layout: "border",
					margins: "5 5 0 0",
					region: "center"
				});

				this.statusBar = new Ext.StatusBar({
					height: 25,
					margins: "0 5 5 5",
					region: "south",
					statusAlign: "right",
			        items: [{
						cls: "x-btn-text-icon",
						handler: function() {
							location.href="logout.vibe";
						},
						iconCls: "vibe-icon-logout",
			            text: vibe.Language.app.LOGOUT
			        },
					{
						cls: "x-btn-text-icon",
						handler: function() 
						{
							var dlg = new vibe.gui.settings.SettingsDialog();
							dlg.show();
						},
						disabled: !session.user.admin,
						iconCls: "vibe-icon-settings",
			            text: vibe.Language.app.SETTINGS
			        },
					{
						cls: "x-btn-text-icon",
						enableToggle: true,
						toggleHandler: function(button,state) 
						{
							if ( state ) {
								this.explorerPanel.expand(true);
							}
							else {
								this.explorerPanel.collapse(true);
							}
						},
						iconCls: "vibe-icon-explorer",
						pressed: !this.explorerPanel.collapsed,
						scope: this,
			            text: vibe.Language.app.EXPLORER
			        }]
				});

				this.viewPort = new Ext.Viewport({
					layout: "border",
					items: [headerPanel,
						this.modulePanel,
						centerPanel,
						this.statusBar]
				});

				var statusTaskRunner = new Ext.util.TaskRunner();
				statusTaskRunner.start({
					interval: vibe.Config.app.STATUS_INTERVAL,
					run: this.checkStatus,
					scope: this
				});
				
				// startup all plugins
				for ( var i=0,len=this.plugins.length; i<len; i++ ) {
					this.plugins[i].startup(this);
				}
				
				// expand top module
				if ( this.modulePanel.items.length>0 ) {
					this.modulePanel.items.get(0).expand(false);
				}
				
				this.fireEvent("startup",this);
			}	
		},
		
		// private
		checkStatus : function()
		{
			var url = "resources/ajax/get-status.vibe";
			
			vibe.util.JsonLoader.load(url,null,function(result)
			{
				if ( result!=null && !this.connected ) {
					this.connected = true;
					this.viewPort.container.unmask();
				}
				else if ( result==null && this.connected ) {
					this.connected = false;
					this.viewPort.container.mask(vibe.Language.app.CONNECTION_LOST);
				}
			},
			this);
		},
		
		/**
		 * Add a module to the module panel.
		 * Can only be called after application has started up.
		 * @param {Ext.Panel} panel the panel to add as a module
		 * @param {String} position the wanted position of the module. Can be either "top" or "bottom"
		 */
		addModule : function(panel,position) 
		{
			if ( position=="top" ) {
				this.modulePanel.insert(0,panel);	
			}
			else if ( position=="bottom" ) {
				this.modulePanel.add(panel);
			}
			
			this.modulePanel.doLayout();
		},
		
		/**
		 * Add a plugin to the application.
		 * Can only be called before application has started up.
		 * @param {vibe.plugin.Plugin} plugin the plugin to add to the application
		 */
		addPlugin : function(plugin) {
			this.plugins.push(plugin);
		},
		
		/**
		 * Get the player panel instance.
		 * @return the player panel instance
		 */
		getPlayerPanel : function() {
			return this.playerPanel;
		},
		
		/**
		 * Get the playlist grid instance.
		 * @return the playlist grid instance
		 */
		getPlaylistGrid : function() {
			return this.playlistGrid;
		},
		
		/**
		 * Get the search panel instance.
		 * @return the search panel instance
		 */
		getSearchPanel : function() {
			return this.searchPanel;
		},		
		
		/**
		 * Get the status bar for the application.
		 * @return the status bar for the application
		 */
		getStatusBar : function() {
			return this.statusBar;
		}
	});
}();