(function() {

g13 = window.g13 || {};
g13["Renderer"] = Renderer;

var cache = {
	matrix: mat3.create()
};

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
	gfx.pointSize(5);

	this.textures = {
		"soldier": (function() {
			var tx = new gfx.Texture("res/soldier.png");
			tx.filter(gfx.LinearMipmapLinear, gfx.Linear);
			tx.generateMipmap();
			return tx;
		})()
	};

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

	this.selection = {
		ibo: null,
		vbo: null
	};

	this.soldiers = new gfx.SpriteBatch(5, gfx.Static);
	this.soldiers.texture = this.textures["soldier"];

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
	gfx.translate(this.position.x, this.position.y);

	this.background.vbo.mode = gfx.Triangles;
	gfx.bind(this.background.shadow);
	gfx.draw(this.background.vbo, this.background.ibo);

	this.background.vbo.mode = gfx.TriangleFan;
	gfx.bind(this.background.texture);
	gfx.draw(this.background.vbo, 4);

	this.soldiers.draw();

	if (this.editor.selection.length > 0)
	{
		var selcount = this.editor.selection.length;

		var vbo = this.selection.vbo;
		var ibo = this.selection.ibo;

		if (!vbo || vbo.size < selcount * 4)
		{
			if (!vbo)
			{
				vbo = this.selection.vbo = new gfx.VBO(selcount * 4, gfx.Dynamic);
				ibo = this.selection.ibo = new gfx.IBO(selcount * 8, gfx.Dynamic);
			}
			else
			{
				vbo.resize(selcount * 4, gfx.Dynamic);
				ibo.resize(selcount * 8, gfx.Dynamic);
			}

			for (var i = 0; i < selcount; i++)
			{
				ibo.set(i * 8 + 0, i * 4 + 0);
				ibo.set(i * 8 + 1, i * 4 + 1);
				ibo.set(i * 8 + 2, i * 4 + 1);
				ibo.set(i * 8 + 3, i * 4 + 2);
				ibo.set(i * 8 + 4, i * 4 + 2);
				ibo.set(i * 8 + 5, i * 4 + 3);
				ibo.set(i * 8 + 6, i * 4 + 3);
				ibo.set(i * 8 + 7, i * 4 + 0);
			}

			ibo.upload();
		}

		var m = mat3.copy(gfx.transform(), cache.matrix);

		var mx = mat3.mulx;
		var my = mat3.muly;
		var fl = Math.floor;

		for (var i = 0; i < selcount; i++)
		{
			var x0 = this.editor.selection[i].bounds.x;
			var y0 = this.editor.selection[i].bounds.y;
			var x1 = this.editor.selection[i].bounds.w + x0;
			var y1 = this.editor.selection[i].bounds.h + y0;

			vbo.set(i * 4 + 0, fl(mx(m, x0, y0)) + 0.5, fl(my(m, x0, y0)) + 0.5, 0, 0, 0, 0, 0, 1);
			vbo.set(i * 4 + 1, fl(mx(m, x1, y0)) + 0.5, fl(my(m, x1, y0)) + 0.5, 0, 0, 0, 0, 0, 1);
			vbo.set(i * 4 + 2, fl(mx(m, x1, y1)) + 0.5, fl(my(m, x1, y1)) + 0.5, 0, 0, 0, 0, 0, 1);
			vbo.set(i * 4 + 3, fl(mx(m, x0, y1)) + 0.5, fl(my(m, x0, y1)) + 0.5, 0, 0, 0, 0, 0, 1);
		}

		vbo.upload();

		gfx.identity();
		gfx.bind(gfx.White);

		vbo.mode = gfx.Lines;
		gfx.draw(vbo, ibo, selcount * 8);

		vbo.mode = gfx.Points;
		gfx.draw(vbo, selcount * 4);

		gfx.transform(m);
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
	this.zoom = 1;
	this.position.x = 0;
	this.position.y = 0;
	this.updateBackground();
	this.soldiers.clear();
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
