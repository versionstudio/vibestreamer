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

vibe.plugin.playlists.PlaylistsDialog = Ext.extend(Ext.Window,
{
    /**
     * @cfg {Mixed} data
     * The data records for the combo box simple store.
     */
	data: [],
		
    /**
     * @cfg {String} emptyText
     * The text to display if the combo box is empty.
     */
	emptyText : "",	
	
    /**
     * @cfg {String} requireSelection
     * Require a selection in the list.
     */
	requireSelection : false,
	
	// private
	closeButton : null,
		
	// private
	comboBox : null,
	
	// private
	okButton : null,
		
	/**
	 * @override
	 */
    initComponent: function() 
	{
		this.addEvents(
			/**
			 * @event submit
			 * Fired when the dialog is successfully submitted through
			 * a click on the OK button.
			 * @param {String} name the name of the playlist
			 * @param {Number} playlistId the database id of the playlist or null if
			 * playlist does not exists in the database.
			 */
			"submit"
		);
				
		var store = new Ext.data.SimpleStore({
		    autoLoad: false,
		    data: this.data,
		    fields: ["name","playlistId"]
		});	

		this.comboBox = new Ext.form.ComboBox({
		    displayField: "name",
		    emptyText: this.emptyText,
			enableKeyEvents: true,
			forceSelection: false,			
		    mode: "local",
		    selectOnFocus: true,
		    store: store,
		    triggerAction: "all",
			typeAhead: this.requireSelection
		});
		
        this.okButton = new Ext.Button({
			disabled: true,
            minWidth: 75,
            scope: this,
            text: vibe.Language.app.OK
        });
		
        this.closeButton = new Ext.Button({
            minWidth: 75,
            scope: this,
            text: vibe.Language.app.CLOSE
        });

		Ext.apply(this,
		{
			bodyStyle: "padding: 10px 10px 10px 10px",
			buttonAlign: "center",
			buttons: [this.okButton,
				this.closeButton],
			height: 110,
			items: [this.comboBox],
			modal: true,
		    layout: "fit",
			resizable: false,
		    shadowOffset: 6,
			width: 300
		});
				
        vibe.plugin.playlists.PlaylistsDialog.superclass.initComponent.call(this);
		
		this.closeButton.on("click",this.onCloseButtonClick,this);
		this.comboBox.on("keyup",this.onComboBoxKeyUp,this);
		this.comboBox.on("select",this.onComboBoxSelect,this);
		this.okButton.on("click",this.onOkButtonClick,this);
    },
	
	// private
	onComboBoxKeyUp : function(comboBox,e)
	{
		if ( this.comboBox.getValue().length==0 ) {
			this.okButton.disable();
			return;
		}
		
		if ( this.requireSelection && this.getSelectedRecord()==null ) {
			this.okButton.disable();
		}
		else {
			this.okButton.enable();
		}
	},	
	
	// private
	onComboBoxSelect : function(comboBox,record,index)
	{
		this.okButton.enable();
	},
	
	// private
	onCloseButtonClick : function()
	{
		this.close();
	},
	
	// private
	onOkButtonClick : function()
	{
		var name = null;
		var playlistId = null;
		
		var record = this.getSelectedRecord();
		if ( record!=null ) {
			name = record.get("name");
			playlistId = record.get("playlistId");
		}
		else {
			name = this.comboBox.getValue();
		}
		
		this.fireEvent("submit",name,playlistId);
		this.close();		
	},
	
	// private
	getSelectedRecord : function()
	{
		var selectedRecord = null;
		
		var index = this.comboBox.selectedIndex;
		if ( index!=-1 )
		{
			// make sure selected record matches exactly
			// the combo box value
			var record = this.comboBox.store.getAt(index);
			if ( record.get("name")==this.comboBox.getValue() ) {
				selectedRecord = record;
			}
		}
		
		return selectedRecord;
	}
});