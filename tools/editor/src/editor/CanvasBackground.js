(function() {

g13 = window.g13 || {};
g13["CanvasBackground"] = CanvasBackground;

function CanvasBackground()
{
	this.ibo = new gfx.IBO(6 * 8, gfx.Static);
	this.vbo = new gfx.VBO(4 + 8 * 4, gfx.Static);

	this.texture = new gfx.Texture(16, 16, gfx.RGB, function(x, y, color) {
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
	});

	this.texture.filter(gfx.Nearest, gfx.Nearest);
	this.texture.wrap(gfx.Repeat, gfx.Repeat);

	this.shadow = new gfx.Texture(32, 32, gfx.RGBA, function(x, y, color) {
		y = 32 - y - 1;
		color.r = 1;
		color.g = 1;
		color.b = 1;
		color.a = 1 - (Math.sqrt(x * x + y * y) / 32);
	});

	var indices = [1, 3, 8 + 2, 1, 8 + 0, 8 + 2, 5, 7, 8 + 6, 5, 8 + 4, 8 + 6, 0, 1, 2, 1, 2, 3, 2, 3, 4,
		3, 4, 5, 4, 5, 6, 5, 6, 7, 8 + 0, 8 + 1, 8 + 2, 8 + 1, 8 + 2, 8 + 3, 8 + 2, 8 + 3, 8 + 4,
		8 + 3, 8 + 4, 8 + 5, 8 + 4, 8 + 5, 8 + 6, 8 + 5, 8 + 6, 8 + 7];

	for (var i = 0; i < indices.length; i++)
		this.ibo.set(i, 4 + indices[i]);

	this.ibo.upload();
}

CanvasBackground.prototype.draw = function()
{
	this.vbo.mode = gfx.Triangles;
	gfx.bind(this.shadow);
	gfx.draw(this.vbo, this.ibo);

	this.vbo.mode = gfx.TriangleFan;
	gfx.bind(this.texture);
	gfx.draw(this.vbo, 4);
}

CanvasBackground.prototype.update = function(map)
{
	var w = map.width;
	var h = map.height;

	var u = map.view.zoom * w / this.texture.width;
	var v = map.view.zoom * h / this.texture.height;

	var x0 = -w/2;
	var y0 = -h/2;
	var x1 =  w/2;
	var y1 =  h/2;

	var vbo = this.vbo;
	var ibo = this.ibo;
	var i = 0;

	vbo.set(i++, x0, y0, 0, 0, 255, 255, 255, 1);
	vbo.set(i++, x1, y0, u, 0, 255, 255, 255, 1);
	vbo.set(i++, x1, y1, u, v, 255, 255, 255, 1);
	vbo.set(i++, x0, y1, 0, v, 255, 255, 255, 1);

	var R = 4 / map.view.zoom;
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

})();
