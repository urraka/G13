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

	this.update = function() {
		self.draw(editor);
		self.invalidated = false;
	};

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
	var view = editor.getView();

	gfx.clear();
	gfx.identity();
	gfx.translate(this.canvas.width / 2, this.canvas.height / 2);
	gfx.scale(view.zoom, view.zoom);
	gfx.translate(view.x, view.y);

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

	editor.event({type: "draw"});
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
	this.background.update(event.map);
	this.soldiers.clear();
	this.invalidate();

	editor.updateCursorPosition();
}

Renderer.prototype.on["zoomchange"] = function(editor, event)
{
	this.background.update(editor.map);
	this.invalidate();

	editor.updateCursorPosition();
}

})();
