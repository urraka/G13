(function() {

g13 = window.g13 || {};
g13["Editor"] = Editor;

window.requestAnimationFrame = window.requestAnimationFrame || window.mozRequestAnimationFrame;

function Editor()
{
	var self = this;

	this.map = null;

	this.canvas = document.createElement("canvas");
	this.update = function() { self.event({type: "draw"}); self.invalidated = false; };
	this.invalidated = false;

	gfx.initialize(this.canvas, {alpha: false, antialias: false});
	gfx.bgcolor(0xC0, 0xC0, 0xC0, 1);
	gfx.pointSize(7);

	this.textures = {
		"soldier": (function() {
			var tx = new gfx.Texture("res/soldier.png");
			tx.filter(gfx.LinearMipmapLinear, gfx.Linear);
			tx.generateMipmap();
			return tx;
		})()
	};

	this.background = new g13.CanvasBackground();

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
		"polygon":   new g13.tools.Polygon(),
		"soldier":   new g13.tools.Soldier(),
		"pan":       new g13.tools.Pan()
	};

	this.listeners = [
		this.ui,
		this.tools["pan"]
	];

	// initialize

	this.event({type: "resize"});
	this.newMap(1500, 800);
}

Editor.prototype.newMap = function(width, height)
{
	this.map = new g13.Map();
	this.map.resize(width, height);

	this.event({type: "newmap", map: this.map});
}

Editor.prototype.getCanvas = function()
{
	return this.canvas;
}

Editor.prototype.getTexture = function(id)
{
	return this.textures[id];
}

Editor.prototype.getSelection = function()
{
	return this.map.selection;
}

Editor.prototype.getView = function()
{
	return this.map.view;
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
	if (this.cursor.current !== cursor)
	{
		this.cursor.current = cursor;
		$(this.getCanvas()).css("cursor", this.ui.cursors[cursor].css);
	}
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
	if (!this.invalidated)
	{
		this.invalidated = true;
		requestAnimationFrame(this.update);
	}
}

Editor.prototype.cancel = function()
{
	this.event({type: "cancel"});
}

Editor.prototype.delete = function()
{
	var selection = this.getSelection();

	if (!selection.isEmpty())
	{
		var objects = selection.objects.slice(0);

		this.execute({
			undo: {func: "add_objects", data: {objects: objects, select: true}},
			redo: {func: "remove_objects", data: {objects: objects}}
		});
	}
}

Editor.prototype.execute = function(action)
{
	g13.actions[action.redo.func](this, action.redo.data);

	this.pushHistory(action);
	this.invalidate();
}

Editor.prototype.pushHistory = function(action)
{
	var history = this.map.history;

	if (history.index < history.actions.length - 1)
		history.actions.splice(history.index + 1, history.actions.length);

	history.actions.push(action);
	history.index++;

	// TODO: set some limits

	ui.enable("undo");
	ui.disable("redo");
}

Editor.prototype.undo = function()
{
	var history = this.map.history;

	if (history.index >= 0)
	{
		var action = history.actions[history.index--].undo;
		g13.actions[action.func](this, action.data);
		this.invalidate();

		ui.enable("redo");

		if (history.index < 0)
			ui.disable("undo");
	}
}

Editor.prototype.redo = function()
{
	var history = this.map.history;

	if (history.index < history.actions.length - 1)
	{
		var action = history.actions[++history.index].redo;
		g13.actions[action.func](this, action.data);
		this.invalidate();

		ui.enable("undo");

		if (history.index === history.actions.length - 1)
			ui.disable("redo");
	}
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

Editor.prototype.on["draw"] = function()
{
	var view = this.getView();

	gfx.clear();
	gfx.identity();
	gfx.translate(this.getCanvas().width / 2, this.getCanvas().height / 2);
	gfx.scale(view.zoom, view.zoom);
	gfx.translate(view.x, view.y);

	this.background.draw();

	if (this.map !== null)
		this.map.draw(this);
}

Editor.prototype.on["resize"] = function()
{
	this.canvas.width = 0;
	this.canvas.height = 0;

	var w = $(this.ui.panels["view"]).width();
	var h = $(this.ui.panels["view"]).height();

	this.canvas.width = w;
	this.canvas.height = h;

	gfx.viewport(w, h);

	this.invalidate();
}

Editor.prototype.on["newmap"] = function(event)
{
	this.setTool("selection");
	this.background.update(event.map);
	this.updateCursorPosition();
	this.invalidate();
}

Editor.prototype.on["zoomchange"] = function(event)
{
	this.background.update(this.map);
	this.updateCursorPosition();
	this.invalidate();
}

})();
