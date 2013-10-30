(function() {

g13["tools"] = g13["tools"] || {};
g13["tools"]["Linestrip"] = Linestrip;

function Linestrip()
{
	this.points = null;
	this.ortho = false;
	this.vbo = new gfx.VBO(50, gfx.Dynamic);
}

Linestrip.prototype.addPoint = function(editor, x, y)
{
	if (!this.on["addpoint"].call(this, editor, {x: x, y: y}))
		return false;

	if (this.points === null)
		this.points = [];

	this.points.push({x: x, y: y});

	var index = this.points.length - 1;

	if (this.vbo.size <= (index + 1))
	{
		var size = this.vbo.size * 2;
		this.vbo.resize(size, gfx.Dynamic, true);
	}

	this.vbo.set(index, x, y, 0, 0, 0, 0, 0, 1);

	return true;
}

Linestrip.prototype.intersects = function(x, y)
{
	var P = this.points;
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

Linestrip.prototype.snaptest = function(x, y, r, p)
{
	if (this.points === null)
		return false;

	var points = this.points;
	var N = points.length;

	r *= r;

	for (var i = 0; i < N; i++)
	{
		if (distance2(x, y, points[i].x, points[i].y) < r)
		{
			p.x = points[i].x;
			p.y = points[i].y;

			return true;
		}
	}

	return false;
}

Linestrip.prototype.on = {};

Linestrip.prototype.on["mousedown"] = function(editor, event)
{
	var points = this.points;

	if (event.which === 1)
	{
		var x = editor.cursor.snapX;
		var y = editor.cursor.snapY;

		this.ortho = event.altKey;

		if (points === null)
		{
			this.addPoint(editor, x, y);
		}
		else if (this.ortho)
		{
			var i = points.length - 1;

			if (Math.abs(x - points[i].x) > Math.abs(y - points[i].y))
				y = points[i].y;
			else
				x = points[i].x;

			this.addPoint(editor, x, y);
		}
		else
		{
			this.addPoint(editor, x, y);
		}

		editor.invalidate();
	}
	else if (event.which === 3)
	{
		if (points !== null)
		{
			if (this.on["finish"].call(this, editor))
				this.points = null;
		}
		else
		{
			editor.setTool("selection");
		}
	}
}

Linestrip.prototype.on["mousemove"] = function(editor, event)
{
	this.ortho = event.altKey;
	editor.invalidate();
}

Linestrip.prototype.on["cancel"] = function(editor)
{
	this.points = null;
	editor.invalidate();
}

Linestrip.prototype.on["keydown"] = function(editor, event)
{
	if (this.points !== null)
	{
		if (event.which === Key["alt"] && !this.ortho)
		{
			event.preventDefault();

			this.ortho = true;
			editor.invalidate();
		}
	}
}

Linestrip.prototype.on["keyup"] = function(editor, event)
{
	if (this.points !== null && event.which === Key["alt"] && this.ortho)
	{
		event.preventDefault();

		this.ortho = false;
		editor.invalidate();
	}
}

Linestrip.prototype.on["toolactivate"] = function(editor, event)
{
	editor.setCursor("crosshair");

	if (this.toolType in editor.ui.tools)
		$(editor.ui.tools[this.toolType]).addClass("enabled");
}

Linestrip.prototype.on["tooldeactivate"] = function(editor, event)
{
	if (this.toolType in editor.ui.tools)
		$(editor.ui.tools[this.toolType]).removeClass("enabled");

	this.points = null;
	editor.invalidate();
}

Linestrip.prototype.on["draw"] = function(editor, event)
{
	var points = this.points;

	if (points !== null)
	{
		var x = editor.cursor.snapX;
		var y = editor.cursor.snapY;

		if (this.ortho)
		{
			var i = points.length - 1;

			if (Math.abs(x - points[i].x) > Math.abs(y - points[i].y))
				y = points[i].y;
			else
				x = points[i].x;
		}

		this.vbo.set(points.length, x, y, 0, 0, 0, 0, 0, 1);
		this.vbo.upload();

		gfx.bind(gfx.White);
		gfx.pixelAlign(true);
		this.vbo.mode = gfx.LineStrip;
		gfx.draw(this.vbo, points.length + 1);
		this.vbo.mode = gfx.Points;
		gfx.draw(this.vbo, points.length + 1);
		gfx.pixelAlign(false);
	}
	else
	{
		var x = editor.cursor.snapX;
		var y = editor.cursor.snapY;

		this.vbo.set(0, x, y, 0, 0, 0, 0, 0, 1);
		this.vbo.upload();

		gfx.bind(gfx.White);
		gfx.pixelAlign(true);
		this.vbo.mode = gfx.Points;
		gfx.draw(this.vbo, 1);
		gfx.pixelAlign(false);
	}
}

})();
