(function() {

g13 = window.g13 || {};
g13["Editor"] = Editor;

function Editor()
{
	this.map = null;
	this.renderer = new g13.Renderer();
	this.ui = new g13.UI(this);

	this.cursor = {
		active: false,
		current: null,
		absX: 0,
		absY: 0,
		mapX: 0,
		mapY: 0
	};

	this.tools = {
		current: null,
		"selection": new g13.tools.Selection(),
		"polygon": new g13.tools.Polygon(),
		"soldier": new g13.tools.Soldier(),
		"pan": new g13.tools.Pan()
	};

	this.listeners = [
		this.renderer,
		this.ui,
		this.tools["pan"]
	];

	// initialize

	this.event({type: "resize"});
	this.newMap(1500, 800);
}

Editor.prototype.newMap = function(width, height)
{
	// TODO: replace this with new g13.Map()
	this.map = {
		width: width,
		height: height,
		objects: [],
		selection: new g13.Selection(),
		view: {x: 0, y: 0, zoom: 1}
	};

	this.setTool("selection");
	this.event({type: "newmap", map: this.map});
}

Editor.prototype.getCanvas = function()
{
	return this.renderer.canvas;
}

Editor.prototype.getTexture = function(id)
{
	return this.renderer.textures[id];
}

Editor.prototype.getSelection = function()
{
	return this.map.selection;
}

Editor.prototype.getView = function()
{
	return this.map.view;
}

Editor.prototype.getObjects = function()
{
	return this.map.objects;
}

Editor.prototype.setTool = function(tool)
{
	var nextTool = null;

	if (tool.constructor === String && (tool in this.tools))
		nextTool = this.tools[tool];
	else
		nextTool = tool;

	if (nextTool)
	{
		var prevTool = this.tools.current;

		this.event({type: "tooldeactivate", nextTool: nextTool});
		this.tools.current = nextTool;
		this.event({type: "toolactivate", prevTool: prevTool});
	}
	else
	{
		console.error("Tool '" + tool + "' doesn't exist.");
	}
}

Editor.prototype.setCursor = function(cursor)
{
	var css = "none";

	if (cursor)
		css = "url(" + cursor + "), default";

	$(this.getCanvas()).css("cursor", css);

	this.cursor.current = cursor;
}

Editor.prototype.isCursorActive = function()
{
	return this.cursor.active;
}

Editor.prototype.setZoom = function(zoom)
{
	if (zoom !== this.map.view.zoom)
	{
		this.map.view.zoom = zoom;
		this.event({type: "zoomchange", zoom: zoom});
	}
}

Editor.prototype.getZoom = function()
{
	return this.map.view.zoom;
}

Editor.prototype.zoomIn = function()
{
	this.setZoom(this.getZoom() * 1.5);
}

Editor.prototype.zoomOut = function()
{
	this.setZoom(this.getZoom() / 1.5);
}

Editor.prototype.addObject = function(object)
{
	this.map.objects.push(object);
	this.renderer.invalidate();
}

Editor.prototype.updateCursorPosition = function(x, y)
{
	if (arguments.length === 0)
	{
		x = this.cursor.absX;
		y = this.cursor.absY;
	}

	var offset = $(this.getCanvas()).offset();

	this.cursor.absX = x;
	this.cursor.absY = y;

	var vx = this.map.view.x;
	var vy = this.map.view.y;
	var vz = this.map.view.zoom;
	var cw = this.getCanvas().width;
	var ch = this.getCanvas().height;
	var cx = offset.left;
	var cy = offset.top;

	this.cursor.mapX = (x - cx - cw / 2) * (1 / vz) - vx;
	this.cursor.mapY = (y - cy - ch / 2) * (1 / vz) - vy;
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
	if (event.type in this.on)
		this.on[event.type].call(this, event);

	for (var i = 0; i < this.listeners.length; i++)
	{
		if (event.type in this.listeners[i].on)
			this.listeners[i].on[event.type].call(this.listeners[i], this, event);
	}

	if (this.tools.current !== null && event.type in this.tools.current.on)
		this.tools.current.on[event.type].call(this.tools.current, this, event);
}

Editor.prototype.on = {};

Editor.prototype.on["mouseenter"] = function(event)
{
	this.cursor.active = true;
	this.updateCursorPosition(event.pageX, event.pageY);
}

Editor.prototype.on["mouseleave"] = function(event)
{
	if (!ui.hasCapture(this.getCanvas()))
		this.cursor.active = false;

	this.updateCursorPosition(event.pageX, event.pageY);
}

Editor.prototype.on["mousemove"] = function(event)
{
	this.updateCursorPosition(event.pageX, event.pageY);
}

Editor.prototype.on["mousedown"] = function(event)
{
	this.updateCursorPosition(event.pageX, event.pageY);
}

Editor.prototype.on["keydown"] = function(event)
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
