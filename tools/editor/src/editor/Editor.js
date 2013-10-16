(function() {

g13 = window.g13 || {};
g13["Editor"] = Editor;

function Editor()
{
	this.map = null;

	this.ui = new g13.UI(this);
	this.renderer = new g13.Renderer(this, $("#panel-view")[0]);
	this.cursor = null;
	this.cursorActive = false;
	this.cursorPosition = {x: 0, y: 0};
	this.cursorMapPosition = {x: 0, y: 0};
	this.selection = [];

	this.pan = {
		active: false,
		hook: {x: 0, y: 0},
		position: {x: 0, y: 0},
		cursor: null
	};

	this.tools = {};
	this.tools["selection"] = new g13.tools.Selection();
	this.tools["soldier"] = new g13.tools.Soldier();
	this.currentTool = null;

	this.newMap(1500, 800);

	// events
	var self = this;
	function fwd(event) { self.event(event); }
	$(window).on("resize", fwd);
	$(document).on("keydown keyup", fwd);
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

	this.setCurrentTool("selection");
	this.ui.onNewMap();
	this.renderer.onNewMap();
	this.updateCursorPosition(this.cursorPosition.x, this.cursorPosition.y);
}

Editor.prototype.setCurrentTool = function(tool)
{
	var nextTool = null;

	if (tool.constructor === String && (tool in this.tools))
		nextTool = this.tools[tool];
	else
		nextTool = tool;

	if (nextTool)
	{
		var prevTool = this.currentTool;

		this.event({type: "tooldeactivate", nextTool: nextTool});
		this.currentTool = nextTool;
		this.event({type: "toolactivate", prevTool: prevTool});
	}
	else
	{
		console.error("Tool '" + tool + "' doesn't exist.");
	}
}

Editor.prototype.setCursor = function(cursor, x, y)
{
	if (cursor)
	{
		if (typeof x !== "undefined")
			$(this.renderer.canvas).css("cursor", "url(" + cursor + ") " + x + " " + y + ", default");
		else
			$(this.renderer.canvas).css("cursor", "url(" + cursor + "), default");
	}
	else
	{
		$(this.renderer.canvas).css("cursor", "none");
	}

	this.cursor = cursor;
}

Editor.prototype.isCursorActive = function()
{
	return this.cursorActive;
}

Editor.prototype.setZoom = function(zoom)
{
	if (zoom !== this.renderer.zoom)
	{
		this.renderer.zoom = zoom;
		this.updateCursorPosition(this.cursorPosition.x, this.cursorPosition.y);
		this.renderer.updateBackground();
		this.renderer.invalidate();

		if (this.pan.active)
		{
			this.pan.hook.x = this.cursorPosition.x;
			this.pan.hook.y = this.cursorPosition.y;
			this.pan.position.x = this.renderer.position.x;
			this.pan.position.y = this.renderer.position.y;
		}
	}
}

Editor.prototype.zoomIn = function()
{
	this.setZoom(this.renderer.zoom * 1.5);
}

Editor.prototype.zoomOut = function()
{
	this.setZoom(this.renderer.zoom / 1.5);
}

Editor.prototype.addObject = function(object)
{
	this.map.objects.push(object);

	switch (object.constructor)
	{
		case g13.Soldier:
			this.renderer.addSoldier(object.sprite());
			break;
	}
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

Editor.prototype.invalidate = function()
{
	this.renderer.invalidate();
}

// -----------------------------------------------------------------------------
// events
// -----------------------------------------------------------------------------

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
	if (!ui.hasCapture(this.renderer.canvas))
		this.cursorActive = false;

	this.updateCursorPosition(event.pageX, event.pageY);
}

Editor.prototype.mousemove = function(event)
{
	this.updateCursorPosition(event.pageX, event.pageY);

	if (this.pan.active)
	{
		var x = (this.cursorPosition.x - this.pan.hook.x) * (1 / this.renderer.zoom);
		var y = (this.cursorPosition.y - this.pan.hook.y) * (1 / this.renderer.zoom);

		this.renderer.position.x = this.pan.position.x + x;
		this.renderer.position.y = this.pan.position.y + y;

		this.updateCursorPosition(event.pageX, event.pageY);
		this.renderer.invalidate();
	}
}

Editor.prototype.mousedown = function(event)
{
	this.updateCursorPosition(event.pageX, event.pageY);

	if (event.which === 2)
	{
		this.pan.active = true;
		this.pan.cursor = this.cursor;
		this.pan.hook.x = this.cursorPosition.x;
		this.pan.hook.y = this.cursorPosition.y;
		this.pan.position.x = this.renderer.position.x;
		this.pan.position.y = this.renderer.position.y;
		this.setCursor("res/pan.cur");

		ui.capture(this.renderer.canvas);
	}
}

Editor.prototype.mouseup = function(event)
{
	if (event.which === 2 && this.pan.active)
	{
		ui.capture(null);

		this.pan.active = false;
		this.setCursor(this.pan.cursor);
	}
}

Editor.prototype.keydown = function(event)
{
	if (event.ctrlKey)
	{
		switch (event.which)
		{
			case 107:
				this.zoomIn();
				break;

			case 109:
				this.zoomOut();
				break;

			case 96:
				this.setZoom(1);
				break;
		}
	}
}

})();
