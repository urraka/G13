(function() {


g13["tools"] = g13["tools"] || {};
g13["tools"]["Polygon"] = Polygon;

inherit(Polygon, g13.tools.Linestrip);

Polygon.prototype.on = object_copy(g13.tools.Linestrip.prototype.on);

function Polygon()
{
	this.base.call(this);
}

Polygon.prototype.on["finish"] = function(editor)
{
	var p = this.points;

	if (p.length < 3)
		return true;

	if (this.intersects(p[0].x, p[0].y))
		return false;

	var polygon = new g13.Polygon(p);
	var objects = [polygon];

	editor.execute({
		undo: {func: "remove_objects", data: {objects: objects}},
		redo: {func: "add_objects", data: {objects: objects, select: false}}
	});

	return true;
}

Polygon.prototype.on["addpoint"] = function(editor, event)
{
	var p = this.points;

	if (p === null)
		return true;

	var x = event.x;
	var y = event.y;

	var N = p.length;

	if (N >= 3 && x === p[0].x && y === p[0].y)
	{
		this.on["finish"].call(this, editor, event);
		this.points = null;
		return false;
	}

	return !this.intersects(x, y) && (x !== p[N - 1].x || y !== p[N - 1].y);
}

})();
