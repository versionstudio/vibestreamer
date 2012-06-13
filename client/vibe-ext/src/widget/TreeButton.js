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

Ext.namespace("vibe.widget");

/**
 * @class vibe.widget.TreeButton
 * @extends Ext.Component
 * Provides a button next to each selected tree node.
 * @constructor
 * @param {TreePanel} tree
 * @param {Object} config (optional) the config object
 */
vibe.widget.TreeButton = function(tree,config)
{
	vibe.widget.TreeButton.superclass.constructor.call(this,config);

    this.tree = tree;
	
    if ( !tree.rendered) {
		tree.on("render",this.initButton,this);
    } else {
		this.initButton(tree);
    }
};

Ext.extend(vibe.widget.TreeButton,Ext.Component,
{
	/**
	 * @cfg {String} cls
	 */
	
    /**
     * @cfg {String} iconCls
     */
	
	/**
	 * @cfg {String} offsets
	 */	
	offsets : [100,0],
	
    /**
     * @cfg {String} target
     */
	target : "javascript:void(0);",
		
	/**
	 * @override
	 */
    autoEl : {
		cls: "vibe-treebtn",
        tag: "img"
    },
	
	// private
	currentNode : null,	
		
	/**
	 * @override
	 */
    initComponent : function()
	{
       vibe.widget.TreeButton.superclass.initComponent.call(this);
	   
		this.addEvents(
			/**
			 * @event click
			 * Fires when the button is clicked.
			 * @param {vibe.widget.TreeButton} this
			 * @param {Ext.tree.TreeNode} the current selected node
			 */
			"click"									
		);
    },
	
	/**
	 * @override
	 */	
    onRender: function() 
	{
        vibe.widget.TreeButton.superclass.onRender.apply(this,arguments);
		
        this.el.on("click",this.onClick,this);
		
		if ( this.icon!=null ) {
			this.el.addClass("vibe-treebtn-icon");
			this.el.setStyle("background-image","url(" + this.icon + ")");
			this.el.dom.src = Ext.BLANK_IMAGE_URL;
		}
		else if ( this.iconCls!=null ) {
			this.el.addClass("vibe-treebtn-icon");
			this.el.addClass(this.iconCls);
			this.el.dom.src = Ext.BLANK_IMAGE_URL;
		}
    },	
	
	// private
	initButton : function(tree)
	{
		this.tree. getSelectionModel().on("selectionchange",this.onTreeSelectionChange,this);
	},
	
	// private
    onClick: function() {
        this.fireEvent("click",this,this.currentNode);
    },	
	
	// private
	onTreeSelectionChange : function(selModel,node)
	{
	    if ( !this.rendered ){
			this.render(this.tree.el);
	    }
		
		if ( node!=null ) {
			this.el.show();
			this.el.anchorTo(node.ui.wrap,"tl",this.offsets);		
		}
		else {
			this.el.hide();
		}
		
		this.currentNode = node;
	}	
});