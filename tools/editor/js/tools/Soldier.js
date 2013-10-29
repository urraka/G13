(function() {

g13["tools"] = g13["tools"] || {};
g13["tools"]["Soldier"] = Soldier;

function Soldier()
{
	this.sprite = new gfx.Sprite();
	this.soldier = new g13.Soldier(0, 0);
}

Soldier.prototype.on = {};

Soldier.prototype.on["toolactivate"] = function(editor, event)
{
	editor.setCursor("none");
	$(editor.ui.tools["soldier"]).addClass("enabled");
	editor.invalidate();
}

Soldier.prototype.on["tooldeactivate"] = function(editor)
{
	$(editor.ui.tools["soldier"]).removeClass("enabled");
	editor.invalidate();
}

Soldier.prototype.on["mousedown"] = function(editor, event)
{
	if (event.which === 1)
	{
		var x = editor.cursor.mapX;
		var y = editor.cursor.mapY;

		var objects = [new g13.Soldier(x, y)];

		editor.execute({
			undo: {func: "remove_objects", data: {objects: objects}},
			redo: {func: "add_objects", data: {objects: objects, select: false}}
		});
	}
	else if (event.which === 3)
	{
		editor.setTool("selection");
	}
}

Soldier.prototype.on["mousemove"] = function(editor)
{
	if (editor.isCursorActive())
		editor.invalidate();
}

Soldier.prototype.on["mouseleave"] = function(editor)
{
	editor.invalidate();
}

Soldier.prototype.on["draw"] = function(editor)
{
	if (!editor.isCursorActive())
		return;

	this.soldier.moveTo(editor.cursor.mapX, editor.cursor.mapY);
	this.soldier.sprite(this.sprite);
	this.sprite.a = 0.8;

	gfx.bind(editor.getResource("soldier"));
	this.sprite.draw();
}

})();
