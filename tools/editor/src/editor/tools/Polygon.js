(function() {

g13 = window.g13 || {};
g13["tools"] = g13["tools"] || {};
g13["tools"]["Polygon"] = Polygon;

function Polygon()
{
	this.polygon = null;
	this.vbo = new gfx.VBO(50, gfx.Dynamic);
	// this.lines = new gfx.LineBatch();
}

Polygon.prototype.on = {};

Polygon.prototype.on["mousedown"] = function(editor, event)
{
	if (event.which === 1)
	{
		if (this.polygon === null)
			this.polygon = [];

		this.polygon.push({x: editor.cursor.mapX, y: editor.cursor.mapY});

		var index = this.polygon.length - 1;

		if (this.vbo.size <= (index + 1))
		{
			var size = this.vbo.size * 2;
			this.vbo.resize(size, gfx.Dynamic, true);
		}

		this.vbo.set(index, editor.cursor.mapX, editor.cursor.mapY, 0, 0, 0, 0, 0, 1);

		editor.invalidate();
	}
	else if (event.which === 3 && this.polygon !== null)
	{
		this.polygon = null;
		editor.invalidate();
	}
}

Polygon.prototype.on["mousemove"] = function(editor, event)
{
	if (this.polygon !== null)
		editor.invalidate();
}

Polygon.prototype.on["toolactivate"] = function(editor, event)
{
	editor.setCursor("res/select.cur");
	$(editor.ui.tb_polygon).addClass("enabled");
}

Polygon.prototype.on["tooldeactivate"] = function(editor, event)
{
	$(editor.ui.tb_polygon).removeClass("enabled");

	if (this.polygon !== null)
	{
		this.polygon = null;
		editor.invalidate();
	}
}

Polygon.prototype.on["draw"] = function(editor, event)
{
	if (this.polygon !== null)
	{
		// this.lines.clear();
		// this.lines.width(1.5 / editor.getZoom());
		// this.lines.color(0, 0, 0, 1);
		// this.lines.moveTo(this.polygon[0].x, this.polygon[0].y);

		// for (var i = 1; i < this.polygon.length; i++)
		// 	this.lines.lineTo(this.polygon[i].x, this.polygon[i].y);

		// this.lines.lineTo(editor.cursor.mapX, editor.cursor.mapY);
		// this.lines.upload();
		// this.lines.draw();

		this.vbo.set(this.polygon.length, editor.cursor.mapX, editor.cursor.mapY, 0, 0, 0, 0, 0, 1);
		this.vbo.upload();

		gfx.bind(gfx.White);
		gfx.pixelAlign(true);

		this.vbo.mode = gfx.LineStrip;
		gfx.draw(this.vbo, this.polygon.length + 1);

		this.vbo.mode = gfx.Points;
		gfx.draw(this.vbo, this.polygon.length + 1);

		gfx.pixelAlign(false);
	}
}

})();
