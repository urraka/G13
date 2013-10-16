var map = null;
selectedCell = null;

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

	var resizeTimer = null;

	window.addEventListener("resize", function() {
		if (resizeTimer) clearTimeout(resizeTimer);
		resizeTimer = setTimeout(function() { resizeTimer = null; resize(); }, 1000);
	});

	canvas.addEventListener("mousedown", mousedown);
	canvas.addEventListener("mouseup", mouseup);
	canvas.addEventListener("mousemove", mousemove);

	generateButton.addEventListener("click", generate);
	zoomInButton.addEventListener("click", zoomIn);
	zoomOutButton.addEventListener("click", zoomOut);
	exportButton.addEventListener("click", exportMap);

	gfx.initialize(canvas, { antialias: true, alpha: false, preserveDrawingBuffer: true });
	gfx.bgcolor(255, 255, 255, 1);
	gfx.clear();

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

	draw(true);
}

function zoomIn()
{
	viewport.zoom *= 2;
	draw(false);
}

function zoomOut()
{
	viewport.zoom /= 2;
	draw(false);
}

function exportMap()
{
	window.open("data:text/octet-stream," + encodeURIComponent(JSON.stringify(map_export(map))));
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

		draw(false);
	}
	else if (map)
	{
		var prev = selectedCell;

		selectedCell = null;

		var canvas = document.getElementsByTagName("canvas")[0];

		var x = (evt.clientX - canvas.width/2) * (1/viewport.zoom) + map.width/2 - viewport.position.x;
		var y = (evt.clientY - canvas.height/2) * (1/viewport.zoom) + map.height/2 - viewport.position.y;

		var cells = map.diagram.cells;

		for (var i = 0; i < cells.length; i++)
		{
			if (cells[i].pointIntersection(x, y) > 0)
			{
				selectedCell = cells[i];
				break;
			}
		}

		if (selectedCell !== prev)
			draw(false);
	}
}

function resize()
{
	var w = window.innerWidth;
	var h = window.innerHeight;

	var canvas = document.getElementsByTagName("canvas")[0];

	canvas.width = w;
	canvas.height = h;

	gfx.viewport(w, h);
	gfx.clear();

	draw(false);
}

// draw

function draw(update)
{
	if (!map)
		return;

	var canvas = document.getElementsByTagName("canvas")[0];

	var x = viewport.position.x + viewport.offset.x;
	var y = viewport.position.y + viewport.offset.y;

	if (true)
	{
		// gfx.bgcolor(255, 255, 255, 1);
		// gfx.clear();

		gfx.identity();
		gfx.translate(canvas.width / 2, canvas.height / 2);
		gfx.scale(viewport.zoom, viewport.zoom);
		gfx.translate(x, y);

		map_render(map, update);
	}
	else
	{
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
}
