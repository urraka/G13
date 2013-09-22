var map = null;

var viewport = {
	moving: false,
	position: { x: 0, y: 0 },
	offset: { x: 0, y: 0 },
	hook: { x: 0, y: 0 },
	zoom: 1
};

function main()
{
	var canvas = document.getElementsByTagName("canvas")[0];
	var generateButton = document.getElementById("generate");
	var zoomInButton = document.getElementById("zoomIn");
	var zoomOutButton = document.getElementById("zoomOut");
	var exportButton = document.getElementById("export");
	var exportCloseButton = document.getElementById("export-close");

	window.addEventListener("resize", resize);

	canvas.addEventListener("mousedown", mousedown);
	canvas.addEventListener("mouseup", mouseup);
	canvas.addEventListener("mousemove", mousemove);

	generateButton.addEventListener("click", generate);
	zoomInButton.addEventListener("click", zoomIn);
	zoomOutButton.addEventListener("click", zoomOut);
	exportButton.addEventListener("click", exportMap);
	exportCloseButton.addEventListener("click", exportClose);

	resize();

	generate();
}

// events

function generate()
{
	var params = {};

	var vars = [
		{ name: "width",          "default":  1500 },
		{ name: "height",         "default":   800 },
		{ name: "offset",         "default":    30 },
		{ name: "noise-scale",    "default": 0.005 },
		{ name: "relaxation",     "default":    10 },
		{ name: "simplification", "default":     0 }
	];

	for (var i = 0; i < vars.length; i++)
	{
		params[vars[i].name] = parseFloat(document.getElementById(vars[i].name).value);

		if (isNaN(params[vars[i].name]))
			params[vars[i].name] = vars[i]["default"];
	}

	map = map_generate(params);

	draw();
}

function zoomIn()
{
	viewport.zoom *= 2;
	draw();
}

function zoomOut()
{
	viewport.zoom /= 2;
	draw();
}

function exportMap()
{
	var dlg = document.getElementById("export-dlg");
	var text = document.getElementById("export-text");

	text.value = JSON.stringify(map_export(map));
	dlg.style.display = "block";
}

function exportClose()
{
	var dlg = document.getElementById("export-dlg");
	dlg.style.display = "none";
}

function mousedown(evt)
{
	viewport.moving = true;
	viewport.hook.x = evt.clientX;
	viewport.hook.y = evt.clientY;
}

function mouseup(evt)
{
	viewport.moving = false;
	viewport.position.x += viewport.offset.x;
	viewport.position.y += viewport.offset.y;
	viewport.offset.x = 0;
	viewport.offset.y = 0;
}

function mousemove(evt)
{
	if (viewport.moving)
	{
		viewport.offset.x = (evt.clientX - viewport.hook.x) * (1 / viewport.zoom);
		viewport.offset.y = (evt.clientY - viewport.hook.y) * (1 / viewport.zoom);

		draw();
	}
}

function resize()
{
	var w = window.innerWidth;
	var h = window.innerHeight;

	var canvas = document.getElementsByTagName("canvas")[0];

	canvas.width = w;
	canvas.height = h;

	draw();
}

// draw

function draw()
{
	var canvas = document.getElementsByTagName("canvas")[0];
	var context = canvas.getContext("2d");

	var x = viewport.position.x + viewport.offset.x;
	var y = viewport.position.y + viewport.offset.y;

	context.save();
	context.clearRect(0, 0, canvas.width, canvas.height);
	context.translate(canvas.width / 2, canvas.height / 2);
	context.scale(viewport.zoom, viewport.zoom);
	context.translate(x, y);

	map_draw(map, context);

	context.restore();
}
