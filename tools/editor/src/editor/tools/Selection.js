(function() {

g13 = window.g13 || {};
g13["tools"] = g13["tools"] || {};
g13["tools"]["Selection"] = Selection;

function Selection()
{
	this.dragging = false;
	this.draghook = {x: 0, y: 0};
}

Selection.prototype.toolactivate = function(editor)
{
	editor.setCursor("res/select.cur");
}

Selection.prototype.mousedown = function(editor, event)
{
	var x = editor.cursorMapPosition.x;
	var y = editor.cursorMapPosition.y;

	var selchange = false;

	if (event.which === 1)
	{
		var object = this.hittest(editor.map.objects, x, y);

		if (object !== null)
		{
			var selIndex = editor.selection.indexOf(object);

			if (selIndex === -1)
			{
				if (!event.shiftKey)
					editor.selection = [];

				editor.selection.push(object);
				selchange = true;
			}
			else if (event.shiftKey)
			{
				editor.selection.splice(selIndex, 1);
				selchange = true;
			}
			else
			{
				this.dragging = true;
				this.draghook.x = x;
				this.draghook.y = y;

				editor.setCursor("res/move.cur");
				ui.capture(editor.renderer.canvas);
			}
		}
		else if (editor.selection.length > 0)
		{
			editor.selection = [];
			selchange = true;
		}
	}

	if (selchange)
		editor.invalidate();
}

Selection.prototype.mousemove = function(editor, event)
{
	if (this.dragging)
	{
		var dx = editor.cursorMapPosition.x - this.draghook.x;
		var dy = editor.cursorMapPosition.y - this.draghook.y;

		for (var i = 0; i < editor.selection.length; i++)
		{
			var x = editor.selection[i].x + dx;
			var y = editor.selection[i].y + dy;

			editor.selection[i].setPosition(x, y);
		}

		this.draghook.x = editor.cursorMapPosition.x;
		this.draghook.y = editor.cursorMapPosition.y;

		editor.invalidate();
	}
	else if (this.hittest(editor.selection, editor.cursorMapPosition.x, editor.cursorMapPosition.y))
	{
		editor.setCursor("res/move.cur");
	}
	else
	{
		editor.setCursor("res/select.cur");
	}
}

Selection.prototype.mouseup = function(editor, event)
{
	if (event.which === 1 && this.dragging)
	{
		this.dragging = false;
		ui.capture(null);

		if (!this.hittest(editor.selection, editor.cursorMapPosition.x, editor.cursorMapPosition.y))
			editor.setCursor("res/select.cur");
	}
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

})();
