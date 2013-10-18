(function() {

g13 = window.g13 || {};
g13["tools"] = g13["tools"] || {};
g13["tools"]["Pan"] = Pan;

function Pan()
{
	this.active = false;
	this.hook = {x: 0, y: 0};
	this.position = {x: 0, y: 0};
	this.prevCursor = null;
}

Pan.prototype.on = {};

Pan.prototype.on["mousedown"] = function(editor, event)
{
	if (event.which === 2)
	{
		this.active = true;
		this.prevCursor = editor.cursor.current;
		this.hook.x = editor.cursor.absX;
		this.hook.y = editor.cursor.absY;
		this.position.x = editor.renderer.position.x;
		this.position.y = editor.renderer.position.y;
		editor.setCursor("res/pan.cur");

		ui.capture(editor.getCanvas());
	}
}

Pan.prototype.on["mouseup"] = function(editor, event)
{
	if (event.which === 2 && this.active)
	{
		ui.capture(null);

		this.active = false;
		editor.setCursor(this.prevCursor);
	}
}

Pan.prototype.on["mousemove"] = function(editor, event)
{
	if (this.active)
	{
		var x = (editor.cursor.absX - this.hook.x) * (1 / editor.getZoom());
		var y = (editor.cursor.absY - this.hook.y) * (1 / editor.getZoom());

		editor.renderer.position.x = this.position.x + x;
		editor.renderer.position.y = this.position.y + y;

		editor.updateCursorPosition();
		editor.invalidate();
	}
}

Pan.prototype.on["zoomchange"] = function(editor, event)
{
	if (this.active)
	{
		this.hook.x = editor.cursor.absX;
		this.hook.y = editor.cursor.absY;
		this.position.x = editor.renderer.position.x;
		this.position.y = editor.renderer.position.y;
	}
}

})();
