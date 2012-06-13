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

Ext.namespace("vibe.dd");

/**
 * @class vibe.dd.DDHelper
 * A drag drop helper class used for simplifying loading of dropped data 
 * when dragging and dropping between components in the same ddGroup.
 * An instance of DDHelper should be applied to the drag source component, which
 * will be the one deciding whether the drop is valid and also responsible
 * of loading the drop data and calling the callback given by the drop target.
 * @constructor
 */
vibe.dd.DDHelper = function(config) {
	Ext.apply(this,config);
};

vibe.dd.DDHelper.prototype = 
{
    /**
     * Abstract method for right before a drop is about to occur.
     * This is used to check whether the drop is valid.
     * Subclasses should override this method.
     * @param {Ext.dd.DragSource} source The drag source that was dragged
     * @param {Event} e The event
     * @param {Object} data An object containing arbitrary data supplied by the drag source
     * @return true if the drop is valid
     */
    validateDrop : function(source,e,data) {
      return false;
    },
		
    /**
     * Abstract method for when a drop should be handled.
     * This is used for loading and returning the data to be dropped to the given callback.
     * Subclasses should override this method.
     * @param {Ext.dd.DragSource} source The drag source that was dragged
     * @param {Event} e The event
     * @param {Object} data An object containing arbitrary data supplied by the drag source
     * @param {Function} callback the callback method that should be called when the
     * data to be dropped has been loaded completely.
     * @param {Object} scope the scope to use for the callback method
     * @param {Array} args an array of arguments to be passed to the callback method
     */
    loadDrop : function(source,e,data,callback,scope,args) {
		
	}
};