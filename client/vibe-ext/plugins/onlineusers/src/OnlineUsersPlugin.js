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

Ext.namespace("vibe.plugin.onlineusers");

Ext.onReady(function()
{
	vibe.Application.on("beforeload",function(app) {
		app.addPlugin(new vibe.plugin.onlineusers.OnlineUsersPlugin);
	});
});

/**
 * @class vibe.plugin.onlineusers.OnlineUsersPlugin
 * @extends vibe.plugin.Plugin
 * The online users plugin.
 * Lists the currently online users.
 */
vibe.plugin.onlineusers.OnlineUsersPlugin = Ext.extend(vibe.plugin.Plugin,
{
	// private
	panel : null,
	
	// private
	userTree : null,
	
	/**
	 * @override
	 */
	init : function(app)
	{
		// apply default plugin config
		Ext.applyIf(vibe.Config.plugin,{onlineusers:[]});
		Ext.applyIf(vibe.Config.plugin.onlineusers,{
			UPDATE_INTERVAL: 10000
		});
		
		// apply default plugin language
		Ext.applyIf(vibe.Language.plugin,{onlineusers:[]});
		Ext.applyIf(vibe.Language.plugin.onlineusers,{
			TITLE: "Online Users",
			ONLINE_USER_DETAILS: "Online User Details"
		});
	},
	
	/**
	 * @override
	 */
	startup : function(app)
	{
		this.userTree = new Ext.tree.TreePanel({
			autoScroll: true,
			border: false,
			lines: false,
			loader: null,
			root: new Ext.tree.TreeNode(),
			rootVisible: false
		});
		
		this.panel = new Ext.Panel({
			autoScroll: true,
			border: false,
			collapsed: true,
			items: [this.userTree],
			title: vibe.Language.plugin.onlineusers.TITLE
		});
			
		app.addModule(this.panel,"bottom");
		
		var taskRunner = new Ext.util.TaskRunner();
		taskRunner.start({
			interval: vibe.Config.plugin.onlineusers.UPDATE_INTERVAL,
			run: this.updateTree,
			scope: this
		});
	},
		
	// private
	updateTree : function()
	{
		var url = "plugins/onlineusers/resources/ajax/get-onlineusers.vibe";
		
		vibe.util.JsonLoader.load(url,null,function(result)
		{			
			var root = this.userTree.root;
			while ( node = root.childNodes[0] ) {
				root.removeChild(node);
			}
			
			var onlineUsers = result;			
			if ( onlineUsers!=null && onlineUsers.length>0 )
			{
				root.beginUpdate();
				
				for ( var i=0,len=onlineUsers.length; i<len; i++ )
				{
					var onlineUser = onlineUsers[i];
					var node = new Ext.tree.TreeNode({
						iconCls: "vibe-plugin-onlineusers-icon-user",
						text: onlineUser.name + " (" + onlineUser.remoteAddress + ")",
						tipCfg: {
							autoLoad: {url: "plugins/onlineusers/resources/ajax/userdetails.vibe",
								params: "userGuid=" + onlineUser.guid + "&lastLogin=" + onlineUser.lastLogin + "&lastPlayed=" + onlineUser.lastPlayed},
							width: 200
						}						
					});
					
					node.attributes["data"] = onlineUser;
					root.appendChild(node);
				}
				
				root.endUpdate();
			}
		},
		this);
	}
});