(function() {

g13 = window.g13 || {};
g13["tools"] = g13["tools"] || {};
g13["tools"]["Selection"] = Selection;

function Selection()
{
}

Selection.prototype.toolactivate = function(editor)
{
	editor.setCursor("res/select.cur");
}

Selection.prototype.mousedown = function(editor, event)
{
	if (event.which === 2)
		return;

	var objects = editor.map.objects;

	var x = editor.cursorMapPosition.x;
	var y = editor.cursorMapPosition.y;

	if (!event.shiftKey)
		editor.selection = [];

	for (var i = 0; i < objects.length; i++)
	{
		if (objects[i].hittest(x, y))
			editor.selection.push(objects[i]);
	}

	editor.invalidate();
}

})();
