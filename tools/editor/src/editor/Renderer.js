(function() {

g13 = window.g13 || {};
g13["Renderer"] = Renderer;

function Renderer(editor, container)
{
	var self = this;

	window.requestAnimationFrame = window.requestAnimationFrame ||
		window.mozRequestAnimationFrame;

	function update() {
		self.draw();
		self.invalidated = false;
	}

	this.editor = editor;
	this.container = container;
	this.canvas = document.createElement("canvas");
	this.invalidated = false;
	this.update = update;
	this.zoom = 1;
	this.position = {x: 0, y: 0};

	gfx.initialize(this.canvas, {alpha: false});
	gfx.bgcolor(0xC0, 0xC0, 0xC0, 1);

	this.background = {
		ibo: new gfx.IBO(6 * 8),
		vbo: new gfx.VBO(4 + 8 * 4, gfx.Static),
		texture: new gfx.Texture(16, 16, gfx.RGB, function(x, y, color) {
			color.a = 1;
			color.r = 255;
			color.g = 255;
			color.b = 255;

			if (Math.floor(x / 8) % 2 === Math.floor(y / 8) % 2)
			{
				color.r = 0.749;
				color.g = 0.749;
				color.b = 0.749;
			}
		}),
		shadow: new gfx.Texture(32, 32, gfx.RGBA, function(x, y, color) {
			y = 32 - y - 1;

			color.r = 1;
			color.g = 1;
			color.b = 1;
			color.a = 1 - (Math.sqrt(x * x + y * y) / 32);
		})
	};

	var indices = [1, 3, 8 + 2, 1, 8 + 0, 8 + 2, 5, 7, 8 + 6, 5, 8 + 4, 8 + 6, 0, 1, 2, 1, 2, 3, 2, 3, 4,
		3, 4, 5, 4, 5, 6, 5, 6, 7, 8 + 0, 8 + 1, 8 + 2, 8 + 1, 8 + 2, 8 + 3, 8 + 2, 8 + 3, 8 + 4,
		8 + 3, 8 + 4, 8 + 5, 8 + 4, 8 + 5, 8 + 6, 8 + 5, 8 + 6, 8 + 7];

	for (var i = 0; i < indices.length; i++)
		this.background.ibo.set(i, 4 + indices[i]);

	this.background.ibo.upload();
	this.background.vbo.mode = gfx.TriangleStrip;
	this.background.texture.filter(gfx.Nearest, gfx.Nearest);
	this.background.texture.wrap(gfx.Repeat, gfx.Repeat);

	this.soldiers = new gfx.SpriteBatch(5, gfx.Static);

	$(this.canvas).appendTo(container);

	this.onResize();
}

Renderer.prototype.invalidate = function()
{
	if (!this.invalidated)
	{
		this.invalidated = true;
		requestAnimationFrame(this.update);
	}
}

Renderer.prototype.draw = function()
{
	gfx.clear();
	gfx.identity();
	gfx.translate(this.canvas.width / 2, this.canvas.height / 2);
	gfx.scale(this.zoom, this.zoom);

	if (this.editor.map)
	{
		this.background.vbo.mode = gfx.Triangles;
		gfx.bind(this.background.shadow);
		gfx.draw(this.background.vbo, this.background.ibo);

		this.background.vbo.mode = gfx.TriangleFan;
		gfx.bind(this.background.texture);
		gfx.draw(this.background.vbo, 4);

		gfx.bind(this.editor.resources["soldier"]);
		this.soldiers.draw();
	}

	this.editor.event({type: "draw"});
}

Renderer.prototype.onResize = function()
{
	this.canvas.width = 0;
	this.canvas.height = 0;

	var w = $(this.container).width();
	var h = $(this.container).height();

	this.canvas.width = w;
	this.canvas.height = h;

	gfx.viewport(w, h);

	this.invalidate();
}

Renderer.prototype.updateBackground = function()
{
	var zoom = this.zoom;

	var w = this.editor.map.width;
	var h = this.editor.map.height;

	var u = zoom * w / this.background.texture.width;
	var v = zoom * h / this.background.texture.height;

	var x0 = -w/2;
	var y0 = -h/2;
	var x1 =  w/2;
	var y1 =  h/2;

	var vbo = this.background.vbo;
	var ibo = this.background.ibo;
	var i = 0;

	vbo.set(i++, x0, y0, 0, 0, 255, 255, 255, 1);
	vbo.set(i++, x1, y0, u, 0, 255, 255, 255, 1);
	vbo.set(i++, x1, y1, u, v, 255, 255, 255, 1);
	vbo.set(i++, x0, y1, 0, v, 255, 255, 255, 1);

	var R = 4 / zoom;
	var A = 0.8;

	vbo.set(i++, x0 - R, y0 - R, 1, 0, 0, 0, 0, A);
	vbo.set(i++, x0 - R, y0 + R, 1, 1, 0, 0, 0, A);
	vbo.set(i++, x0 + R, y0 - R, 0, 0, 0, 0, 0, A);
	vbo.set(i++, x0 + R, y0 + R, 0, 1, 0, 0, 0, A);
	vbo.set(i++, x1 - R, y0 - R, 0, 0, 0, 0, 0, A);
	vbo.set(i++, x1 - R, y0 + R, 0, 1, 0, 0, 0, A);
	vbo.set(i++, x1 + R, y0 - R, 1, 0, 0, 0, 0, A);
	vbo.set(i++, x1 + R, y0 + R, 1, 1, 0, 0, 0, A);

	vbo.set(i++, x0 - R, y1 - R, 1, 1, 0, 0, 0, A);
	vbo.set(i++, x0 - R, y1 + R, 1, 0, 0, 0, 0, A);
	vbo.set(i++, x0 + R, y1 - R, 0, 1, 0, 0, 0, A);
	vbo.set(i++, x0 + R, y1 + R, 0, 0, 0, 0, 0, A);
	vbo.set(i++, x1 - R, y1 - R, 0, 1, 0, 0, 0, A);
	vbo.set(i++, x1 - R, y1 + R, 0, 0, 0, 0, 0, A);
	vbo.set(i++, x1 + R, y1 - R, 1, 1, 0, 0, 0, A);
	vbo.set(i++, x1 + R, y1 + R, 1, 0, 0, 0, 0, A);

	vbo.upload();
}

Renderer.prototype.onNewMap = function()
{
	this.updateBackground();
	this.invalidate();
}

Renderer.prototype.addSoldier = function(sprite)
{
	if (this.soldiers.size === this.soldiers.maxSize)
		this.soldiers.resize(this.soldiers.size * 2, gfx.Static, true);

	this.soldiers.add(sprite);
	this.soldiers.upload();
	this.invalidate();
}

})();
