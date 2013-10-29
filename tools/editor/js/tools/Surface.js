(function() {

g13["tools"] = g13["tools"] || {};
g13["tools"]["Surface"] = Surface;

function Surface()
{
	this.points = null;
	this.ortho = false;
	this.vbo = new gfx.VBO(50, gfx.Dynamic);
}

Surface.prototype.addPoint = function(x, y)
{
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
}

Surface.prototype.finish = function(editor)
{
	var surface = new g13.Surface(this.points, editor.getResource("grass"));
	var objects = [surface];

	editor.execute({
		undo: {func: "remove_objects", data: {objects: objects}},
		redo: {func: "add_objects", data: {objects: objects, select: false}}
	});

	this.points = null;
}

Surface.prototype.on = {};

Surface.prototype.on["mousedown"] = function(editor, event)
{
	var points = this.points;

	if (event.which === 1)
	{
		var x = editor.cursor.mapX;
		var y = editor.cursor.mapY;

		this.ortho = event.altKey;

		if (points === null)
		{
			this.addPoint(x, y);
		}
		else if (this.ortho)
		{
			var i = points.length - 1;

			if (Math.abs(x - points[i].x) > Math.abs(y - points[i].y))
				y = points[i].y;
			else
				x = points[i].x;

			this.addPoint(x, y);
		}
		else
		{
			this.addPoint(x, y);
		}

		editor.invalidate();
	}
	else if (event.which === 3)
	{
		if (points !== null)
		{
			this.finish(editor);
			editor.invalidate();
		}
		else
		{
			editor.setTool("selection");
		}
	}
}

Surface.prototype.on["mousemove"] = function(editor, event)
{
	if (this.points !== null)
	{
		this.ortho = event.altKey;
		editor.invalidate();
	}
}

Surface.prototype.on["cancel"] = function(editor)
{
	if (this.points !== null)
	{
		this.finish();
		editor.invalidate();
	}
}

Surface.prototype.on["keydown"] = function(editor, event)
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

Surface.prototype.on["keyup"] = function(editor, event)
{
	if (this.points !== null && event.which === Key["alt"] && this.ortho)
	{
		event.preventDefault();

		this.ortho = false;
		editor.invalidate();
	}
}

Surface.prototype.on["toolactivate"] = function(editor, event)
{
	editor.setCursor("crosshair");
	// $(editor.ui.tools["surface"]).addClass("enabled");
}

Surface.prototype.on["tooldeactivate"] = function(editor, event)
{
	// $(editor.ui.tools["surface"]).removeClass("enabled");

	if (this.points !== null)
	{
		this.points = null;
		editor.invalidate();
	}
}

Surface.prototype.on["draw"] = function(editor, event)
{
	var points = this.points;

	if (points !== null)
	{
		var x = editor.cursor.mapX;
		var y = editor.cursor.mapY;

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
}

})();
