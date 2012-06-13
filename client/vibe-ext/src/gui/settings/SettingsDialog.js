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

Ext.namespace("vibe.gui.settings");

vibe.gui.settings.SettingsDialog = Ext.extend(Ext.Window,
{
	// private
	busy : false,
	
	// private
	closeButton : null,
	
	// private
	comboBox : null,
	
	// private
	currentGroupGuid : null,
	
	// private
	currentUserGuid : null,
		
	// private
	saveButton : null,
	
	// private
	settingsGrid : null,
	
	// private
	settingsLoadMask : null,
	
	// private
	settingsSelModel : null,
			
	/**
	 * @override
	 */
    initComponent: function() 
	{		
        this.closeButton = new Ext.Button({
            minWidth: 75,
            scope: this,
            text: vibe.Language.app.CLOSE
        });
		
		this.saveButton = new Ext.Button({
			disabled: true,
			minWidth: 75,
			scope: this,
			text: vibe.Language.app.SAVE
		});
		
		this.comboBox = new Ext.form.ComboBox({
			disabled: true,
		    displayField: "name",
			editable: false,
			enableKeyEvents: true,
			forceSelection: true,
		    mode: "local",
		    selectOnFocus: true,
			store: new Ext.data.SimpleStore({
				data: [[vibe.Language.app.EVERYONE,null,null]],
			    fields: ["name","userGuid","groupGuid"]
			}),
		    triggerAction: "all",
			typeAhead: true,
			width: 425
		});
		
		this.settingsSelModel = new Ext.grid.CheckboxSelectionModel();
	
		this.settingsGrid = new Ext.grid.EditorGridPanel({
			bodyStyle: "margin: 10px 0 10px 0",
			clicksToEdit: 100, // we want to handle when to edit ourselves
			cm: new Ext.grid.ColumnModel([
				this.settingsSelModel,
				{ 
					dataIndex: "title",
					header: "title",
					sortable: false
				},
				{
					align: "right",
					dataIndex: "value",
	                editor: new Ext.form.TextField({
	                    allowBlank: true
	                }),					
					header: "value",
					sortable: false
				}
			]),
			height: 129,
			hideHeaders: true,
			selModel: this.settingsSelModel,
			store: new Ext.data.SimpleStore({
				fields: ["title","name","value"]
		    }),			
	        viewConfig : {
	            forceFit: true
	        }
		});
		
		Ext.apply(this,
		{
			bodyStyle: "padding: 10px 10px 10px 10px",
			buttonAlign: "center",
			buttons: [this.saveButton,this.closeButton],
			height: 270,
			items: [this.comboBox,
				this.settingsGrid,
				new Ext.form.Label({text: vibe.Language.app.CHECKED_SETTINGS_ARE_SPECIFIC})],
			modal: true,
			resizable: false,
		    shadowOffset: 6,
			title: vibe.Language.app.SETTINGS,
			width: 460
		});
				
        vibe.gui.settings.SettingsDialog.superclass.initComponent.call(this);
		
		this.closeButton.on("click",this.onCloseButtonClick,this);
		this.comboBox.on("select",this.onComboBoxSelect,this);
		this.saveButton.on("click",this.onSaveButtonClick,this);
		this.settingsGrid.on("afteredit",this.onSettingsGridAfterEdit,this);
		this.settingsGrid.on("beforeedit",this.onSettingsGridBeforeEdit,this);
		this.settingsGrid.on("render",this.onSettingsGridRender,this);
		this.settingsSelModel.on("rowdeselect",this.onSettingsSelModelRowDeselect,this);
		this.settingsSelModel.on("rowselect",this.onSettingsSelModelRowSelect,this);
		this.settingsSelModel.on("selectionchange",this.onSettingsSelModelSelectionChange,this)
    },
	
	/**
	 * @override
	 */
    onRender: function()
	{
		vibe.gui.settings.SettingsDialog.superclass.onRender.apply(this,arguments);
		
		var url = "resources/ajax/get-users.vibe";
		vibe.util.JsonLoader.load(url,null,function(result)
		{
			var users = result;
			
			url = "resources/ajax/get-groups.vibe";
			vibe.util.JsonLoader.load(url,null,function(result)
			{
				var groups = result;
				
				if ( users!=null && groups!=null )
				{
					for ( var i=0,len=users.length; i<len; i++ ) {
						var user = users[i];
						var data = [[user.name,user.guid,null]];
						this.comboBox.getStore().loadData(data,true);
					}
					
					for ( var i=0,len=groups.length; i<len; i++ ) {
						var group = groups[i];
						var data = [[group.name,null,group.guid]];
						this.comboBox.getStore().loadData(data,true);
					}
					
					this.comboBox.getStore().sort("title","ASC");
					this.comboBox.setValue(vibe.Language.app.EVERYONE);
					
					this.loadSettings(null,null);
				}
			},
			this);
		},
		this);
	},
	
	// private
	loadSettings : function(userGuid,groupGuid)
	{
		this.busy = true;
		
		this.comboBox.disable();
		this.saveButton.disable();
		this.settingsLoadMask.show();

		this.settingsGrid.getStore().removeAll();
		
		this.currentUserGuid = userGuid;
		this.currentGroupGuid = groupGuid;

		var params = null;
		if ( userGuid!=null ) {
			params = "userGuid=" + userGuid;
		}
		else if ( groupGuid!=null ) {
			params = "groupGuid=" + groupGuid;
		}

		var url = "resources/ajax/get-settings.vibe";		
		vibe.util.JsonLoader.load(url,params,function(result)
		{
			for ( var i=0,len=result.length; i<len; i++ ) 
			{
				var setting = result[i];
				var data = [[setting.title,setting.name,setting.value]];
				this.settingsGrid.getStore().loadData(data,true);
				
				if ( !setting.inherited ) {
					this.settingsSelModel.selectRow(
						this.settingsGrid.getStore().find("name", setting.name), true);
				}
			}
			
			this.settingsGrid.getStore().sort("title","ASC");
			
			this.comboBox.enable();
			this.settingsLoadMask.hide();
			
			this.busy = false;
		},
		this);
	},
	
	// private
	saveSettings : function()
	{
		this.busy = true;
		
		this.comboBox.disable();
		this.saveButton.disable();
		this.settingsLoadMask.show();
		
		var params = null;
		if ( this.currentUserGuid!=null ) {
			params = "userGuid=" + this.currentUserGuid;
		}
		else if ( this.currentGroupGuid!=null ) {
			params = "groupGuid=" + this.currentGroupGuid;
		}
		
		var settings = this.settingsSelModel.getSelections();
		for ( var i=0,len=settings.length; i<len; i++ ) 
		{
			var setting = settings[i];
			
			if ( params==null ) {
				params = "";
			}
			else {
				params += "&";				
			}
			
			params += setting.get("name") + "=" + encodeURIComponent(setting.get("value"));
		}
		
		var url = "resources/ajax/set-settings.vibe";	
		vibe.util.JsonLoader.load(url,params,function(result) {
			this.loadSettings(this.currentUserGuid,this.currentGroupGuid);
		},
		this);
	},
	
	// private
	onCloseButtonClick : function() 
	{
		this.close();
	},
	
	// private
	onComboBoxSelect : function(comboBox,record,index) 
	{
		this.loadSettings(record.get("userGuid"),record.get("groupGuid"));
	},	
	
	// private
	onSaveButtonClick : function() 
	{
		this.saveSettings();
	},
	
	// private
	onSettingsGridAfterEdit : function(e)
	{
		if ( e.value!=e.originalValue ) {
			this.saveButton.enable();
		}
	},
	
	// private
	onSettingsGridBeforeEdit : function(e)
	{
		// make sure we cannot edit a non selected row
		if ( !this.settingsSelModel.isSelected(e.record) ) {
			return false;
		}
	},
	
	// private
	onSettingsGridRender : function(component)
	{
		this.settingsLoadMask = new Ext.LoadMask(component.body,
			{ msg: vibe.Language.app.LOADMASK });
	},
	
	// private
	onSettingsSelModelRowDeselect : function(selModel,rowIndex,record) 
	{
		this.settingsGrid.stopEditing();
	},		
	
	// private
	onSettingsSelModelRowSelect : function(selModel,rowIndex,record) 
	{
		if ( !this.busy ) 
		{
			(function(){
			    this.settingsGrid.startEditing(rowIndex,2);
			}).defer(200,this);
		}
	},
	
	// private
	onSettingsSelModelSelectionChange : function(selModel) 
	{
		if ( !this.busy ) {
			this.saveButton.enable();
		}
	}
});