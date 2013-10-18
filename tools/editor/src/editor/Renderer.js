(function() {

g13 = window.g13 || {};
g13["Renderer"] = Renderer;

var cache = {
	matrix: mat3.create(),
	sprite: new gfx.Sprite()
};

window.requestAnimationFrame = window.requestAnimationFrame || window.mozRequestAnimationFrame;

function Renderer()
{
	var self = this;

	this.canvas = document.createElement("canvas");
	this.invalidated = false;
	this.zoom = 1;
	this.position = {x: 0, y: 0};

	this.update = function() {
		self.draw(editor);
		self.invalidated = false;
	};

	gfx.initialize(this.canvas, {alpha: false});
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
	this.selection = new gfx.VBO(4, gfx.Dynamic);

	this.soldiers = new gfx.SpriteBatch(5, gfx.Dynamic);
	this.soldiers.texture = this.textures["soldier"];
}

Renderer.prototype.invalidate = function()
{
	if (!this.invalidated)
	{
		this.invalidated = true;
		requestAnimationFrame(this.update);
	}
}

Renderer.prototype.draw = function(editor)
{
	gfx.clear();
	gfx.identity();
	gfx.translate(this.canvas.width / 2, this.canvas.height / 2);
	gfx.scale(this.zoom, this.zoom);
	gfx.translate(this.position.x, this.position.y);

	this.background.draw();

	var objects = editor.getObjects();
	var soldiers = this.soldiers;

	if (objects.length > 0)
	{
		if (soldiers.maxSize < objects.length)
			soldiers.resize(objects.length);

		soldiers.clear();

		for (var i = 0; i < objects.length; i++)
			soldiers.add(objects[i].sprite(cache.sprite));

		soldiers.upload();
		soldiers.draw();
	}

	//this.drawSelection(editor);

	editor.event({type: "draw"});
}

Renderer.prototype.drawSelection = function(editor)
{
	var selection = editor.getSelection();

	if (selection.objects.length > 0)
	{
		var m = mat3.copy(gfx.transform(), cache.matrix);

		var x0 = selection.bounds.x;
		var y0 = selection.bounds.y;
		var x1 = selection.bounds.w + x0;
		var y1 = selection.bounds.h + y0;

		var vbo = this.selection;

		var mx = mat3.mulx;
		var my = mat3.muly;
		var fl = Math.floor;

		vbo.set(0, fl(mx(m, x0, y0)) + 0.5, fl(my(m, x0, y0)) + 0.5, 0, 0, 255, 255, 255, 1);
		vbo.set(1, fl(mx(m, x1, y0)) + 0.5, fl(my(m, x1, y0)) + 0.5, 0, 0, 255, 255, 255, 1);
		vbo.set(2, fl(mx(m, x1, y1)) + 0.5, fl(my(m, x1, y1)) + 0.5, 0, 0, 255, 255, 255, 1);
		vbo.set(3, fl(mx(m, x0, y1)) + 0.5, fl(my(m, x0, y1)) + 0.5, 0, 0, 255, 255, 255, 1);

		vbo.mode = gfx.Points;
		vbo.upload();

		gfx.identity();
		gfx.bind(gfx.White);
		gfx.blend(gfx.OneMinusDstColor, gfx.OneMinusSrcColor);
		gfx.draw(vbo);
		gfx.blend(gfx.Default);
		gfx.transform(m);
	}
}

Renderer.prototype.on = {};

Renderer.prototype.on["resize"] = function(editor, event)
{
	this.canvas.width = 0;
	this.canvas.height = 0;

	var w = $(editor.ui.panelView).width();
	var h = $(editor.ui.panelView).height();

	this.canvas.width = w;
	this.canvas.height = h;

	gfx.viewport(w, h);

	this.invalidate();
}

Renderer.prototype.on["newmap"] = function(editor, event)
{
	this.zoom = 1;
	this.position.x = 0;
	this.position.y = 0;
	this.background.update(event.map.width, event.map.height, this.zoom);
	this.soldiers.clear();
	this.invalidate();

	editor.updateCursorPosition();
}

Renderer.prototype.on["zoomchange"] = function(editor, event)
{
	this.zoom = event.zoom;
	this.background.update(editor.map.width, editor.map.height, this.zoom);
	this.invalidate();

	editor.updateCursorPosition();
}

})();
