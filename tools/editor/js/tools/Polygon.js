(function() {

g13["tools"] = g13["tools"] || {};
g13["tools"]["Polygon"] = Polygon;

var SNAP = 50;

function Polygon()
{
	this.polygon = null;
	this.ortho = false;
	this.vbo = new gfx.VBO(50, gfx.Dynamic);
}

Polygon.prototype.addPoint = function(x, y)
{
	if (this.polygon === null)
		this.polygon = [];

	this.polygon.push({x: x, y: y});

	var index = this.polygon.length - 1;

	if (this.vbo.size <= (index + 1))
	{
		var size = this.vbo.size * 2;
		this.vbo.resize(size, gfx.Dynamic, true);
	}

	this.vbo.set(index, x, y, 0, 0, 0, 0, 0, 1);
}

Polygon.prototype.intersects = function(x, y)
{
	var P = this.polygon;
	var N = P.length;

	var ax = P[N - 1].x;
	var ay = P[N - 1].y;
	var bx = x;
	var by = y;

	for (var i = 0; i < N - 2; i++)
	{
		var cx = P[i + 0].x;
		var cy = P[i + 0].y;
		var dx = P[i + 1].x;
		var dy = P[i + 1].y;

		if ((bx !== cx || by !== cy) && segments_intersect(ax, ay, bx, by, cx, cy, dx, dy))
			return true;
	}

	return false;
}

Polygon.prototype.close = function(editor)
{
	var points = this.polygon;

	if (points.length < 3)
		return;

	if (this.intersects(points[0].x, points[0].y))
		return;

	var polygon = new g13.Polygon(points);

	var objects = [polygon];

	editor.execute({
		undo: {func: "remove_objects", data: {objects: objects}},
		redo: {func: "add_objects", data: {objects: objects, select: false}}
	});

	this.polygon = null;
}

Polygon.prototype.on = {};

Polygon.prototype.on["mousedown"] = function(editor, event)
{
	var poly = this.polygon;

	if (event.which === 1)
	{
		var x = editor.cursor.mapX;
		var y = editor.cursor.mapY;

		this.ortho = event.altKey;

		if (poly === null)
		{
			this.addPoint(x, y);
		}
		else if (this.ortho)
		{
			var i = poly.length - 1;

			if (Math.abs(x - poly[i].x) > Math.abs(y - poly[i].y))
				y = poly[i].y;
			else
				x = poly[i].x;

			if (!this.intersects(x, y))
				this.addPoint(x, y);
		}
		else if (poly.length > 2 && distance2(x, y, poly[0].x, poly[0].y) < SNAP)
		{
			this.close(editor);
		}
		else
		{
			if (!this.intersects(x, y))
				this.addPoint(x, y);
		}

		editor.invalidate();
	}
	else if (event.which === 3)
	{
		if (poly !== null)
		{
			if (poly.length < 3)
				this.polygon = null;
			else
				this.close(editor);

			editor.invalidate();
		}
		else
		{
			editor.setTool("selection");
		}
	}
}

Polygon.prototype.on["mousemove"] = function(editor, event)
{
	if (this.polygon !== null)
	{
		this.ortho = event.altKey;
		editor.invalidate();
	}
}

Polygon.prototype.on["cancel"] = function(editor)
{
	this.polygon = null;
	editor.invalidate();
}

Polygon.prototype.on["keydown"] = function(editor, event)
{
	if (this.polygon !== null)
	{
		if (event.which === Key["alt"] && !this.ortho)
		{
			event.preventDefault();

			this.ortho = true;
			editor.invalidate();
		}
	}
}

Polygon.prototype.on["keyup"] = function(editor, event)
{
	if (this.polygon !== null && event.which === Key["alt"] && this.ortho)
	{
		event.preventDefault();

		this.ortho = false;
		editor.invalidate();
	}
}

Polygon.prototype.on["toolactivate"] = function(editor, event)
{
	editor.setCursor("crosshair");
	$(editor.ui.tools["polygon"]).addClass("enabled");
}

Polygon.prototype.on["tooldeactivate"] = function(editor, event)
{
	$(editor.ui.tools["polygon"]).removeClass("enabled");

	if (this.polygon !== null)
	{
		this.polygon = null;
		editor.invalidate();
	}
}

Polygon.prototype.on["draw"] = function(editor, event)
{
	var poly = this.polygon;

	if (poly !== null)
	{
		var x = editor.cursor.mapX;
		var y = editor.cursor.mapY;

		if (this.ortho)
		{
			var i = poly.length - 1;

			if (Math.abs(x - poly[i].x) > Math.abs(y - poly[i].y))
				y = poly[i].y;
			else
				x = poly[i].x;
		}
		else if (poly.length > 2 && distance2(x, y, poly[0].x, poly[0].y) < SNAP)
		{
			var x = poly[0].x;
			var y = poly[0].y;
		}

		this.vbo.set(poly.length, x, y, 0, 0, 0, 0, 0, 1);
		this.vbo.upload();

		gfx.bind(gfx.White);
		gfx.pixelAlign(true);

		this.vbo.mode = gfx.LineStrip;
		gfx.draw(this.vbo, poly.length + 1);

		this.vbo.mode = gfx.Points;
		gfx.draw(this.vbo, poly.length + 1);

		gfx.pixelAlign(false);
	}
}

})();
