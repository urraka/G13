(function() {

g13 = window.g13 || {};
g13["tools"] = g13["tools"] || {};
g13["tools"]["Soldier"] = Soldier;

function Soldier()
{
	this.vbo = new gfx.VBO(4, gfx.Dynamic);
	this.vbo.mode = gfx.TriangleFan;
	this.prevTool = null;
}

Soldier.prototype.toolactivate = function(editor, event)
{
	editor.setCursor(null);
	editor.invalidate();

	this.prevTool = event.prevTool;
}

Soldier.prototype.tooldeactivate = function(editor)
{
	editor.invalidate();
}

Soldier.prototype.mousedown = function(editor, event)
{
	if (event.which === 1)
	{
		var x = editor.cursorMapPosition.x;
		var y = editor.cursorMapPosition.y;

		editor.addObject(new g13.Soldier(x, y));
	}
	else if (event.which === 3 && this.prevTool !== null)
	{
		editor.setCurrentTool(this.prevTool);
	}
}

Soldier.prototype.mousemove = function(editor)
{
	if (editor.isCursorActive())
		editor.invalidate();
}

Soldier.prototype.mouseleave = function(editor)
{
	editor.invalidate();
}

Soldier.prototype.draw = function(editor)
{
	if (!editor.isCursorActive())
		return;

	var texture = editor.renderer.textures["soldier"];

	var w = 1.00390625 * 0.15 * texture.width;
	var h = 1.00390625 * 0.15 * texture.height;

	var x = editor.cursorMapPosition.x;
	var y = editor.cursorMapPosition.y;

	var alpha = 0.8;

	this.vbo.set(0, -w/2 + x, -w/2 + y, 0, 0, 255, 255, 255, alpha);
	this.vbo.set(1,  w/2 + x, -w/2 + y, 1, 0, 255, 255, 255, alpha);
	this.vbo.set(2,  w/2 + x,  w/2 + y, 1, 1, 255, 255, 255, alpha);
	this.vbo.set(3, -w/2 + x,  w/2 + y, 0, 1, 255, 255, 255, alpha);

	this.vbo.upload();

	gfx.bind(texture);
	gfx.draw(this.vbo);
}

})();
