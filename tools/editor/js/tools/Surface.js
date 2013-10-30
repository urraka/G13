(function() {

g13["tools"] = g13["tools"] || {};
g13["tools"]["Surface"] = Surface;

inherit(Surface, g13.tools.Linestrip);

Surface.prototype.on = object_copy(g13.tools.Linestrip.prototype.on);

function Surface()
{
	this.base.call(this);
}

Surface.prototype.on["finish"] = function(editor)
{
	if (this.points.length < 2)
		return true;

	var surface = new g13.Surface(this.points, editor.getResource("grass"));
	var objects = [surface];

	editor.execute({
		undo: {func: "remove_objects", data: {objects: objects}},
		redo: {func: "add_objects", data: {objects: objects, select: false}}
	});

	return true;
}

Surface.prototype.on["addpoint"] = function(editor, event)
{
	return true;
}

})();
