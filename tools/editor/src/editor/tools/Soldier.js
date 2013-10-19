(function() {

g13 = window.g13 || {};
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
	editor.setCursor(null);
	$(editor.ui.tb_soldier).addClass("enabled");
	editor.invalidate();
}

Soldier.prototype.on["tooldeactivate"] = function(editor)
{
	$(editor.ui.tb_soldier).removeClass("enabled");
	editor.invalidate();
}

Soldier.prototype.on["mousedown"] = function(editor, event)
{
	if (event.which === 1)
	{
		var x = editor.cursor.mapX;
		var y = editor.cursor.mapY;

		editor.addObject(new g13.Soldier(x, y));
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

	this.soldier.setPosition(editor.cursor.mapX, editor.cursor.mapY);
	this.soldier.sprite(this.sprite);
	this.sprite.a = 0.8;

	gfx.bind(editor.getTexture("soldier"));
	this.sprite.draw();
}

})();
