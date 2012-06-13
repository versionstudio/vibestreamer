/**
 * Fix for bug when removing multiple rows.
 * Credit to 'aj3423' at the ExtJS forum.
 */
Ext.override(Ext.ux.grid.livegrid.GridView,
{
	removeRows : function(firstRow,lastRow)
	{
	    if (firstRow < 0) firstRow = 0; // add this line ,the 'firstRow' can be negative..
	    var bd = this.mainBody.dom;
	    for (var rowIndex = firstRow; rowIndex <= lastRow; rowIndex++) {
	        Ext.removeNode(bd.childNodes[firstRow]); // if firstRow<0 , uncaught exception occur.
	    }
	}
});

/**
 * Fix for making it possible for supporting tool tip instances
 * on a tree node.
 * Credit to 'Jack Slocum' at the ExtJS forum.
 */
Ext.tree.TreeNodeUI.prototype.initEvents = Ext.tree.TreeNodeUI.prototype.initEvents.createSequence(function()
{
	if ( this.node.attributes.tipCfg ) {
		var o = this.node.attributes.tipCfg;
		o.target = Ext.id(this.textNode);
		this.node.attributes.tip = new Ext.ToolTip(o);
	}
});