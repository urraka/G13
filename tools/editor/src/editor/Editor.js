(function() {

g13 = window.g13 || {};
g13["Editor"] = Editor;

function Editor()
{
	this.map = null;

	this.ui = new g13.UI(this);
	this.renderer = new g13.Renderer(this, $("#panel-view")[0]);
	this.cursorActive = false;
	this.cursorPosition = {x: 0, y: 0};
	this.cursorMapPosition = {x: 0, y: 0};

	this.resources = {
		"soldier": (function() {
			var tx = new gfx.Texture("res/soldier.png");
			tx.generateMipmap();
			return tx;
		})()
	};

	this.tools = {};
	this.tools["select"] = new g13.tools.Select();
	this.tools["soldier"] = new g13.tools.Soldier();
	this.currentTool = null;

	var self = this;

	function fwd(event) { self.event(event); }

	$(window).on("resize keydown keyup", fwd);
	$(this.renderer.canvas).on("mousemove mousedown mouseup mouseenter mouseleave", fwd);
}

Editor.prototype.newMap = function(width, height)
{
	// TODO: destroy current map

	this.map = {
		width: width,
		height: height,
		objects: []
	};

	this.setCurrentTool("select");
	this.ui.onNewMap();
	this.renderer.onNewMap();
}

Editor.prototype.invalidate = function()
{
	this.renderer.invalidate();
}

Editor.prototype.event = function(event)
{
	if (event.type in this)
		this[event.type].call(this, event);

	if (this.currentTool !== null && event.type in this.currentTool)
		this.currentTool[event.type].call(this.currentTool, this, event);
}

Editor.prototype.resize = function()
{
	this.renderer.onResize();
}

Editor.prototype.mouseenter = function(event)
{
	this.cursorActive = true;
	this.updateCursorPosition(event.pageX, event.pageY);
}

Editor.prototype.mouseleave = function(event)
{
	this.cursorActive = false;
	this.updateCursorPosition(event.pageX, event.pageY);
}

Editor.prototype.mousemove = function(event)
{
	this.updateCursorPosition(event.pageX, event.pageY);
}

Editor.prototype.updateCursorPosition = function(x, y)
{
	var offset = $(this.renderer.canvas).offset();

	this.cursorPosition.x = x;
	this.cursorPosition.y = y;

	var vx = this.renderer.position.x;
	var vy = this.renderer.position.y;
	var vz = this.renderer.zoom;
	var cw = this.renderer.canvas.width;
	var ch = this.renderer.canvas.height;
	var cx = offset.left;
	var cy = offset.top;

	this.cursorMapPosition.x = (x - cx - cw / 2) * (1 / vz) - vx;
	this.cursorMapPosition.y = (y - cy - ch / 2) * (1 / vz) - vy;
}

Editor.prototype.setCurrentTool = function(tool)
{
	if (tool in this.tools)
	{
		this.event({type: "tooldeactivate"});
		this.currentTool = this.tools[tool];
		this.event({type: "toolactivate"});
	}
	else
	{
		console.error("Tool '" + tool + "' doesn't exist.");
	}
}

Editor.prototype.setCursor = function(cursor)
{
	if (cursor)
		$(this.renderer.canvas).css("cursor", "url(res/" + cursor + ".cur), default");
	else
		$(this.renderer.canvas).css("cursor", "none");
}

Editor.prototype.isCursorActive = function()
{
	return this.cursorActive;
}

Editor.prototype.setZoom = function(zoom)
{
	this.renderer.zoom = zoom;
	this.updateCursorPosition(this.cursorPosition.x, this.cursorPosition.y);
	this.renderer.updateBackground();
	this.renderer.invalidate();
}

Editor.prototype.addObject = function(object)
{
	this.map.objects.push(object);
}

})();
