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
 * @class vibe.widget.LinkButton
 * @extends Ext.BoxComponent
 * A button component presented as an link with either an icon, text or both.
 * @constructor
 * @param {Object} config (optional) the config object
 */
vibe.widget.LinkButton = Ext.extend(Ext.BoxComponent,
{
    /**
     * @cfg {String} cls
     */
		
    /**
     * @cfg {String} iconCls
     */
	
    /**
     * @cfg {String} target
     */
	target: "javascript:void(0);",
	
    /**
     * @cfg {String} text
     */
	
	/**
	 * @override
	 */
    autoEl: {
		cls: "vibe-linkbtn",
        tag: "div"
    },

	/**
	 * @override
	 */	
    initComponent: function() 
	{
        vibe.widget.LinkButton.superclass.initComponent.apply(this,arguments);

		this.addEvents(
			/**
			 * @event click
			 * Fires when the button is clicked.
			 * @param {vibe.widget.LinkButton} this
			 */
			"click"
		);
    },
	
	/**
	 * @override
	 */	
    onRender: function() 
	{
        vibe.widget.LinkButton.superclass.onRender.apply(this,arguments);
		
		var linkEl = Ext.DomHelper.append(this.el,{tag: "a"},true);
        linkEl.on("click",this.onClick,this);
		linkEl.dom.href = this.target;
		
		if ( this.icon!=null ) {
			this.el.addClass("vibe-linkbtn-icon");
			linkEl.setStyle("background-image","url(" + this.icon + ")");
		}
		else if ( this.iconCls!=null ) {
			this.el.addClass("vibe-linkbtn-icon");
			linkEl.addClass(this.iconCls);
		}
		
		if ( this.text!=null ) {
			linkEl.update(this.text);
		}
    },
	
	// private
    onClick: function() {
        this.fireEvent("click",this);
    }
});