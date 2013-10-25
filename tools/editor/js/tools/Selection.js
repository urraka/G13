(function() {

g13["tools"] = g13["tools"] || {};
g13["tools"]["Selection"] = Selection;

var cache = {
	matrix: mat3.create()
};

function Selection()
{
	this.dragging = false;
	this.selecting = false;
	this.hook = {x: 0, y: 0};
	this.undoAction = null;
	this.vbo = new gfx.VBO(8, gfx.Dynamic);

	this.texture = new gfx.Texture(8, 8, gfx.RGBA, function(x, y, color) {
		color.a = 1;
		color.r = 255;
		color.g = 255;
		color.b = 255;

		if (Math.floor(x / 2) % 2 !== Math.floor(y / 2) % 2)
			color.a = 0;
	});

	this.texture.filter(gfx.Nearest, gfx.Nearest);
	this.texture.wrap(gfx.Repeat, gfx.Repeat);
}

Selection.prototype.hittest = function(objects, x, y)
{
	for (var i = objects.length - 1; i >= 0; i--)
	{
		if (objects[i].hittest(x, y))
			return objects[i];
	}

	return null;
}

Selection.prototype.performDragging = function(editor)
{
	var selection = editor.getSelection();

	var dx = editor.cursor.mapX - this.hook.x;
	var dy = editor.cursor.mapY - this.hook.y;

	for (var i = 0; i < selection.objects.length; i++)
	{
		var object = selection.objects[i];
		object.move(dx, dy);
	}

	selection.bounds.x += dx;
	selection.bounds.y += dy;

	this.hook.x = editor.cursor.mapX;
	this.hook.y = editor.cursor.mapY;

	editor.invalidate();
}

Selection.prototype.updateCursor = function(editor)
{
	var selection = editor.getSelection();
	var objects = editor.map.retrieve(editor.cursor.mapX, editor.cursor.mapY);

	if (objects.length > 0 && selection.contains(objects[0]))
		editor.setCursor("move");
	else
		editor.setCursor("pointer");
}

Selection.prototype.on = {};

Selection.prototype.on["toolactivate"] = function(editor)
{
	editor.setCursor("pointer");
	$(editor.ui.tools["select"]).addClass("enabled");

	if (!editor.getSelection().isEmpty())
		editor.invalidate();
}

Selection.prototype.on["tooldeactivate"] = function(editor)
{
	$(editor.ui.tools["select"]).removeClass("enabled");

	if (!editor.getSelection().isEmpty())
		editor.invalidate();
}

Selection.prototype.on["mousedown"] = function(editor, event)
{
	var x = editor.cursor.mapX;
	var y = editor.cursor.mapY;

	var selchange = false;

	var selection = editor.getSelection();

	if (event.which === 1)
	{
		var objects = editor.map.retrieve(x, y);

		if (objects.length > 0)
		{
			var object = objects[0];
			var selIndex = selection.find(object);

			if (selIndex === -1)
			{
				if (!event.shiftKey)
					selection.clear();

				selection.add(object);
				selchange = true;
			}
			else if (event.shiftKey)
			{
				selection.remove(selIndex);
				selchange = true;
			}
			else
			{
				this.dragging = true;
				this.hook.x = x;
				this.hook.y = y;

				editor.setCursor("move");
				ui.capture(editor.getCanvas());

				var undoData = {
					objects: selection.objects.slice(0),
					positions: []
				};

				for (var i = 0; i < undoData.objects.length; i++)
					undoData.positions.push({x: undoData.objects[i].x, y: undoData.objects[i].y});

				this.undoAction = {
					func: "move_objects",
					data: undoData
				};
			}
		}
		else
		{
			if (!event.shiftKey && selection.objects.length > 0)
			{
				selection.clear();
				selchange = true;
			}

			this.selecting = true;
			this.hook.x = x;
			this.hook.y = y;

			ui.capture(editor.getCanvas());
		}
	}

	if (selchange)
		editor.invalidate();
}

Selection.prototype.on["zoomchange"] = function(editor, event)
{
	if (this.dragging)
		this.performDragging(editor);
}

Selection.prototype.on["mousemove"] = function(editor, event)
{
	var selection = editor.getSelection();

	if (this.dragging)
	{
		this.performDragging(editor);
	}
	else if (this.selecting)
	{
		editor.invalidate();
	}
	else
	{
		this.updateCursor(editor);
	}
}

Selection.prototype.on["objectschange"] = function(editor)
{
	this.updateCursor(editor);
}

Selection.prototype.on["mouseup"] = function(editor, event)
{
	if (event.which === 1)
	{
		if (this.dragging)
		{
			this.dragging = false;
			ui.capture(null);

			var redoData = {
				objects: this.undoAction.data.objects,
				positions: []
			};

			for (var i = 0; i < redoData.objects.length; i++)
				redoData.positions.push({x: redoData.objects[i].x, y: redoData.objects[i].y});

			var redoAction = {
				func: "move_objects",
				data: redoData
			};

			editor.pushHistory({undo: this.undoAction, redo: redoAction});

			this.undoAction = null;

			if (!this.hittest(editor.getSelection().objects, editor.cursor.mapX, editor.cursor.mapY))
				editor.setCursor("pointer");
		}
		else if (this.selecting)
		{
			this.selecting = false;
			editor.invalidate();
			ui.capture(null);

			var x = this.hook.x;
			var y = this.hook.y;
			var w = editor.cursor.mapX - x;
			var h = editor.cursor.mapY - y;

			var contained = true;

			if (w < 0)
			{
				contained = false;
				w = -w;
				x -= w;
			}

			if (h < 0)
			{
				h = -h;
				y -= h;
			}

			var selection = editor.getSelection();
			var objects = editor.map.retrieve(x, y, w, h);

			if (!event.shiftKey && !selection.isEmpty())
				selection.clear();

			var selected = [];

			if (contained)
			{
				for (var i = 0; i < objects.length; i++)
				{
					if (objects[i].contained(x, y, w, h))
						selected.push(objects[i]);
				}
			}
			else
			{
				for (var i = 0; i < objects.length; i++)
				{
					if (objects[i].intersects(x, y, w, h))
						selected.push(objects[i]);
				}
			}

			selection.toggle(selected);
			editor.invalidate();
		}
	}
}

Selection.prototype.on["postdraw"] = function(editor)
{
	var selection = editor.getSelection();

	if (!this.selecting && selection.isEmpty())
		return;

	var vbo = this.vbo;
	var m = mat3.copy(gfx.transform(), cache.matrix);

	gfx.identity();

	if (!selection.isEmpty())
	{
		var x0a = selection.bounds.x;
		var y0a = selection.bounds.y;
		var x1a = selection.bounds.w + x0a;
		var y1a = selection.bounds.h + y0a;

		var x0 = Math.floor(mat3.mulx(m, x0a, y0a)) + 0.5;
		var y0 = Math.floor(mat3.muly(m, x0a, y0a)) + 0.5;
		var x1 = Math.floor(mat3.mulx(m, x1a, y1a)) + 0.5;
		var y1 = Math.floor(mat3.muly(m, x1a, y1a)) + 0.5;

		vbo.set(0, x0, y0, 0, 0, 255, 255, 255, 1);
		vbo.set(1, x1, y0, 0, 0, 255, 255, 255, 1);
		vbo.set(2, x1, y1, 0, 0, 255, 255, 255, 1);
		vbo.set(3, x0, y1, 0, 0, 255, 255, 255, 1);
	}

	if (this.selecting)
	{
		var x0a = this.hook.x;
		var y0a = this.hook.y;
		var x1a = editor.cursor.mapX;
		var y1a = editor.cursor.mapY;

		var x0 = Math.floor(mat3.mulx(m, x0a, y0a)) + 0.5;
		var y0 = Math.floor(mat3.muly(m, x0a, y0a)) + 0.5;
		var x1 = Math.floor(mat3.mulx(m, x1a, y1a)) + 0.5;
		var y1 = Math.floor(mat3.muly(m, x1a, y1a)) + 0.5;

		var u = (x1 - x0 + 0.5) / this.texture.width;
		var v = (y1 - y0 + 0.5) / this.texture.height;

		vbo.set(4, x0, y0, 0, 0, 0, 0, 0, 1);
		vbo.set(5, x1, y0, u, 0, 0, 0, 0, 1);
		vbo.set(6, x1, y1, u, v, 0, 0, 0, 1);
		vbo.set(7, x0, y1, 0, v, 0, 0, 0, 1);
	}

	vbo.upload();

	if (!selection.isEmpty())
	{
		vbo.mode = gfx.Points;
		gfx.bind(gfx.White);
		gfx.blend(gfx.OneMinusDstColor, gfx.OneMinusSrcColor);
		gfx.draw(vbo, 4);
		gfx.blend(gfx.Default);
	}

	if (this.selecting)
	{
		vbo.mode = gfx.LineLoop;
		gfx.bind(this.texture);
		gfx.draw(vbo, 4, 4);
	}

	gfx.transform(m);
}

})();
