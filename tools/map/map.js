var map = new Map();

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
	var width = parseFloat(document.getElementById("width").value);
	var height = parseFloat(document.getElementById("height").value);
	var offset = parseFloat(document.getElementById("offset").value);
	var noiseScale = parseFloat(document.getElementById("noise-scale").value);
	var relaxation = parseInt(document.getElementById("relaxation").value);

	if (isNaN(width) || isNaN(height) || isNaN(offset) || isNaN(noiseScale) || isNaN(relaxation))
		return;

	map.generate(width, height, offset, noiseScale, relaxation);
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

	text.value = JSON.stringify(map.export());
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
	map.draw(context);
	context.restore();
}

// Map

function Map()
{
	this.width = 0;
	this.height = 0;
	this.diagram = null;
	this.sites = null;
	this.polygons = null;
	this.outlines = null;
}

Map.prototype.generate = function(width, height, offset, noiseScale, relaxation)
{
	this.width = width;
	this.height = height;

	var voronoi = new Voronoi();
	var bbox = { xl: 0, xr: width, yt: 0, yb: height };
	var sites = [];

	var n = (width * height) / (offset * offset);

	for (var i = 0; i < n; i++)
		sites.push({ x: rand(0, width), y: rand(0, height) });

	var diagram = voronoi.compute(sites, bbox);

	for (var i = 0; i < relaxation; i++)
	{
		sites = lloyd_relax(diagram);
		diagram = voronoi.compute(sites, bbox);
	}

	this.sites = sites;
	this.diagram = diagram;

	// perlin noise to select ground cells

	var perlin = new Perlin();
	var seed = { x: rand(0, width), y: rand(0, height) };

	var cells = diagram.cells;

	for (var i = 0; i < cells.length; i++)
	{
		var point = cells[i].site;
		var border = false;

		for (var j = 0; j < cells[i].halfedges.length; j++)
		{
			var p1 = cells[i].halfedges[j].getStartpoint();
			var p2 = cells[i].halfedges[j].getEndpoint();

			if (p1.x < offset || p1.x > width - offset || p1.y < offset || p1.y > height - offset ||
				p2.x < offset || p2.x > width - offset || p2.y < offset || p2.y > height - offset)
			{
				border = true;
				break;
			}
		}

		if (border)
		{
			cells[i].ground = true;
			continue;
		}

		var noise = perlin.noise(seed.x + point.x * noiseScale, seed.y + point.y * noiseScale, 0);

		var dx = point.x - width / 2;
		var dy = point.y - height / 2;

		var distance = dx * dx + dy * dy;

		cells[i].ground = noise > (0.55 - 0.2 * distance / (width * height));
	}

	// expands polygon recursively to contain the adjacent cells

	function expand(polygon, cell, visited)
	{
		visited.push(cell);

		var a = polygon;        // polygon to expand
		var b = cell.halfedges; // polygon to expand with

		var aSize = a.length;
		var bSize = b.length;

		if (aSize === 0)
		{
			for (var i = 0; i < bSize; i++)
				a.push(b[i]);
		}
		else
		{
			// find indices of the first common edges

			var aIndex = -1;
			var bIndex = -1;

			for (var i = 0; aIndex === -1 && i < aSize; i++)
			{
				var aEdge = a[i].edge;

				for (var j = 0; j < bSize; j++)
				{
					var bEdge = b[j].edge;

					if (aEdge === bEdge)
					{
						aIndex = i;
						bIndex = j;
						break;
					}
				}
			}

			if (aIndex === 0)
			{
				// this might not be the first common edge, so go back to find out

				var n = 0;

				for (var i = 0; i < aSize - 1; i++)
				{
					var ii = aSize - i - 1;
					var jj = (bIndex + i + 1) % bSize;

					var aEdge = a[ii].edge;
					var bEdge = b[jj].edge;

					if (aEdge !== bEdge)
						break;

					n++;
				}

				aIndex = (aIndex - n + aSize) % aSize;
				bIndex = (bIndex + n + bSize) % bSize;
			}

			// find how many consecutive common edges we have

			var nCommon = 1;
			var N = Math.min(bSize, aSize);

			for (var i = 0; i < N - 1; i++)
			{
				var ii = (aIndex + i + 1) % aSize;
				var jj = (bIndex - i - 1 + bSize) % bSize;

				var aEdge = a[ii].edge;
				var bEdge = b[jj].edge;

				if (aEdge !== bEdge)
					break;

				nCommon++;
			}

			// build the new polygon

			var c = a.slice(Math.max(0, aIndex + nCommon - aSize), Math.max(0, aIndex));

			for (var i = 0; i < bSize - nCommon; i++)
			{
				var ii = (bIndex + i + 1) % bSize;
				c.push(b[ii]);
			}

			for (var i = aIndex + nCommon; i < aSize; i++)
				c.push(a[i]);

			a = c;
		}

		// expand through neighbors

		var ids = cell.getNeighborIds();

		for (var i = 0; i < ids.length; i++)
		{
			var neighborCell = cells[ids[i]];

			if (visited.indexOf(neighborCell) === -1 && neighborCell.ground)
				a = expand(a, neighborCell, visited);
		}

		return a;
	}

	// removes some unnecessary lines from the polygon

	function refine(polygon)
	{
		var size = polygon.length;

		if (size < 2)
			return polygon;

		for (var i = 0; i < size - 1; i++)
		{
			var a = polygon[i + 0].edge;
			var b = polygon[i + 1].edge;

			if (a === b)
			{
				var c = polygon.slice(0, i);

				for (var j = i + 2; j < size; j++)
					c.push(polygon[j]);

				return refine(c);
			}
		}

		var a = polygon[size - 1].edge;
		var b = polygon[0].edge;

		if (a === b)
			return refine(polygon.slice(1, size - 1));

		return polygon;
	}

	// returns all outlines of a polygon

	function outlines(polygon)
	{
		var result = [];
		var linestrips = [];
		var linestrip = [];

		var N = polygon.length;

		for (var i = 0; i < N; i++)
		{
			var inner = false;

			for (var j = 0; j < N; j++)
			{
				if (i !== j && polygon[i].edge === polygon[j].edge)
				{
					inner = true;
					break;
				}
			}

			if (inner && linestrip.length > 0)
			{
				var len = linestrip.length;

				if (linestrip[0].getStartpoint() === linestrip[len - 1].getEndpoint())
					result.push(linestrip);
				else
					linestrips.push(linestrip);

				linestrip = [];
			}
			else if (!inner)
			{
				linestrip.push(polygon[i]);
			}
		}

		if (linestrip.length > 0)
		{
			var len = linestrip.length;

			if (linestrip[0].getStartpoint() === linestrip[len - 1].getEndpoint())
				result.push(linestrip);
			else
				linestrips.push(linestrip);
		}

		function equal(a, b)
		{
			return a === b; // || (a.x === b.x && a.y === b.y);
		}

		while (linestrips.length > 0)
		{
			var N = linestrips.length;

			for (var i = 0; i < N; i++)
			{
				var stop = false;

				for (var j = i + 1; j < N; j++)
				{
					var a = linestrips[i];
					var b = linestrips[j];

					var aSize = a.length;
					var bSize = b.length;

					if (equal(a[aSize - 1].getEndpoint(), b[0].getStartpoint()))
					{
						for (var k = 0; k < bSize; k++)
							a.push(b[k]);

						linestrips.splice(j, 1);

						if (equal(a[a.length - 1].getEndpoint(), a[0].getStartpoint()))
						{
							result.push(a);
							linestrips.splice(i, 1);
						}

						stop = true;
						break;
					}
					else if (equal(b[bSize - 1].getEndpoint(), a[0].getStartpoint()))
					{
						for (var k = 0; k < aSize; k++)
							b.push(a[k]);

						if (equal(b[b.length - 1].getEndpoint(), b[0].getStartpoint()))
						{
							result.push(b);
							linestrips.splice(j, 1);
						}

						linestrips.splice(i, 1);

						stop = true;
						break;
					}
				}

				if (stop)
					break;
			}

			// the next is necessary because bounds have different instance of vertices, making it
			// fail to generate an outline

			if (N === linestrips.length)
				break;
		}

		return result;
	}

	var visited = [];
	var polygons = [];

	for (var i = 0; i < cells.length; i++)
	{
		var cell = cells[i];

		if (visited.indexOf(cell) !== -1 || !cell.ground)
			continue;

		polygons.push(refine(expand([], cell, visited)));
	}

	this.outlines = [];

	for (var i = 0; i < polygons.length; i++)
	{
		var result = outlines(polygons[i]);

		for (var j = 0; j < result.length; j++)
			this.outlines.push(result[j]);
	}

	this.polygons = polygons;
}

Map.prototype.export = function()
{
	var hw = this.width / 2;
	var hh = this.height / 2;

	var data = {
		bounds: {
			left: -hw,
			right: hw,
			top: -hh,
			bottom: hh
		},
		vertices: [],
		polygons: [],
		outlines: [],
		diagram: {
			edges: [],
			cells: []
		}
	};

	// vertices

	for (var i = 0; i < this.diagram.vertices.length; i++)
	{
		var v = this.diagram.vertices[i];

		v.vid_ = i;

		data.vertices.push({
			x: v.x - hw,
			y: v.y - hh
		});
	}

	// polygons

	for (var i = 0; i < this.polygons.length; i++)
	{
		var src = this.polygons[i];
		var dst = [];

		for (var j = 0; j < src.length; j++)
			dst.push(src[j].getStartpoint().vid_);

		data.polygons.push(dst);
	}

	// outlines

	for (var i = 0; i < this.outlines; i++)
	{
		var src = this.outlines[i];
		var dst = [];

		for (var j = 0; j < src.length; j++)
			dst.push(src[j].getStartpoint().vid_);

		data.outlines.push(dst);
	}

	// diagram edges

	for (var i = 0; i < this.diagram.edges.length; i++)
	{
		var edge = this.diagram.edges[i];

		data.diagram.edges.push({
			a: edge.va.vid_,
			b: edge.vb.vid_
		});
	}

	// diagram cells

	for (var i = 0; i < this.diagram.cells.length; i++)
	{
		var edges = this.diagram.cells[i].halfedges;
		var dst = [];

		for (var j = 0; j < edges.length; j++)
			dst.push(edges[j].getStartpoint().vid_);

		data.diagram.cells.push(dst);
	}

	return data;
}

Map.prototype.draw = function(context)
{
	if (!this.diagram)
		return;

	context.translate(-this.width / 2, -this.height / 2);

	context.fillStyle = "#8080FF";
	context.fillRect(0, 0, this.width, this.height);

	context.fillStyle = "#000";
	context.strokeStyle = "#333";

	var edges = this.diagram.edges;
	var cells = this.diagram.cells;

	for (var i = 0; i < cells.length; i++)
	{
		if (!cells[i].ground || cells[i].halfedges.length === 0)
			continue;

		var p = cells[i].halfedges[0].getStartpoint();

		context.beginPath();
		context.moveTo(p.x, p.y);

		for (var j = 0; j < cells[i].halfedges.length; j++)
		{
			p = cells[i].halfedges[j].getEndpoint();
			context.lineTo(p.x, p.y);
		}

		context.fill();
	}

	for (var i = 0; i < edges.length; i++)
	{
		context.beginPath()
		context.moveTo(edges[i].va.x, edges[i].va.y);
		context.lineTo(edges[i].vb.x, edges[i].vb.y);
		context.stroke();
	}

	context.fillStyle = "rgba(0, 255, 0, 0.5)";
	context.strokeStyle = "#000";
	context.lineWidth = 3;

	for (var i = 0; i < this.polygons.length; i++)
	{
		var polygon = this.polygons[i];
		var p = polygon[0].getStartpoint();

		context.beginPath();
		context.moveTo(p.x, p.y);

		for (var j = 0; j < polygon.length; j++)
		{
			p = polygon[j].getEndpoint();
			context.lineTo(p.x, p.y);
		}

		context.fill();
	}

	for (var i = 0; i < this.outlines.length; i++)
	{
		var outline = this.outlines[i];
		var p = outline[0].getStartpoint();

		context.beginPath();
		context.moveTo(p.x, p.y);

		for (var j = 0; j < outline.length; j++)
		{
			p = outline[j].getEndpoint();
			context.lineTo(p.x, p.y);
		}

		context.stroke();
	}

	context.strokeRect(0, 0, this.width, this.height);
}

// Perlin

function Perlin()
{
	const p = [151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,
		37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,
		33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,
		231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,
		1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,
		173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,
		16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
		172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,
		193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,
		157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,
		128,195,78,66,215,61,156,180,151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,
		103,30,69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
		35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,
		166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,
		65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,
		100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,
		59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,
		155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,
		181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,
		72,243,141,128,195,78,66,215,61,156,180];

	var iOctaves = 1;
	var fPersistence = .5;

	var aOctFreq; // frequency per octave
	var aOctPers; // persistence per octave
	var fPersMax; // 1 / max persistence

	var iSeed = 1323;

	var iXoffset;
	var iYoffset;
	var iZoffset;

	const baseFactor = 1 / 64;

	var initialized = false;

	function init()
	{
		seedOffset();
		octFreqPers();
		initialized = true;
	}

	function octFreqPers()
	{
		var fFreq, fPers;
		aOctFreq = [];
		aOctPers = [];
		fPersMax = 0;

		for (var i = 0; i < iOctaves; i++)
		{
			fFreq = Math.pow(2, i);
			fPers = Math.pow(fPersistence, i);
			fPersMax += fPers;
			aOctFreq.push(fFreq);
			aOctPers.push(fPers);
		}

		fPersMax = 1 / fPersMax;
	}

	function seedOffset()
	{
		iXoffset = iSeed = (iSeed * 16807) % 2147483647;
		iYoffset = iSeed = (iSeed * 16807) % 2147483647;
		iZoffset = iSeed = (iSeed * 16807) % 2147483647;
	}

	this.noise = function(x, y, z)
	{
		x = x || 0;
		y = y || 0;
		z = z || 0;

		if (!initialized)
			init();

		var s = 0;
		var fFreq, fPers, x, y, z;
		var xf, yf, zf, u, v, w;
		var x1, y1, z1;
		var X, Y, Z, A, B, AA, AB, BA, BB, hash;
		var g1, g2, g3, g4, g5, g6, g7, g8;

		x += iXoffset;
		y += iYoffset;
		z += iZoffset;

		var int = Math.round;

		for (var i = 0; i < iOctaves; i++)
		{
			fFreq = aOctFreq[i];
			fPers = aOctPers[i];

			x = x * fFreq;
			y = y * fFreq;
			z = z * fFreq;

			xf = Math.floor(x);
			yf = Math.floor(y);
			zf = Math.floor(z);

			X = xf & 255;
			Y = yf & 255;
			Z = zf & 255;

			x -= xf;
			y -= yf;
			z -= zf;

			u = x * x * x * (x * (x * 6 - 15) + 10);
			v = y * y * y * (y * (y * 6 - 15) + 10);
			w = z * z * z * (z * (z * 6 - 15) + 10);

			A  = int(p[X]) + Y;
			AA = int(p[A]) + Z;
			AB = int(p[int(A + 1)]) + Z;
			B  = int(p[int(X + 1)]) + Y;
			BA = int(p[B]) + Z;
			BB = int(p[int(B + 1)]) + Z;

			x1 = x - 1;
			y1 = y - 1;
			z1 = z - 1;

			hash = int(p[int(BB + 1)]) & 15;
			g1 = ((hash&1) == 0 ? (hash<8 ? x1 : y1) : (hash<8 ? -x1 : -y1)) + ((hash&2) == 0 ? hash<4 ? y1 : (hash==12 ? x1 : z1) : hash<4 ? -y1 : (hash==14 ? -x1 : -z1));

			hash = int(p[int(AB + 1)]) & 15;
			g2 = ((hash&1) == 0 ? (hash<8 ? x  : y1) : (hash<8 ? -x  : -y1)) + ((hash&2) == 0 ? hash<4 ? y1 : (hash==12 ? x  : z1) : hash<4 ? -y1 : (hash==14 ? -x : -z1));

			hash = int(p[int(BA + 1)]) & 15;
			g3 = ((hash&1) == 0 ? (hash<8 ? x1 : y ) : (hash<8 ? -x1 : -y )) + ((hash&2) == 0 ? hash<4 ? y  : (hash==12 ? x1 : z1) : hash<4 ? -y  : (hash==14 ? -x1 : -z1));

			hash = int(p[int(AA + 1)]) & 15;
			g4 = ((hash&1) == 0 ? (hash<8 ? x  : y ) : (hash<8 ? -x  : -y )) + ((hash&2) == 0 ? hash<4 ? y  : (hash==12 ? x  : z1) : hash<4 ? -y  : (hash==14 ? -x  : -z1));

			hash = int(p[BB]) & 15;
			g5 = ((hash&1) == 0 ? (hash<8 ? x1 : y1) : (hash<8 ? -x1 : -y1)) + ((hash&2) == 0 ? hash<4 ? y1 : (hash==12 ? x1 : z) : hash<4 ? -y1 : (hash==14 ? -x1 : -z));

			hash = int(p[AB]) & 15;
			g6 = ((hash&1) == 0 ? (hash<8 ? x  : y1) : (hash<8 ? -x  : -y1)) + ((hash&2) == 0 ? hash<4 ? y1 : (hash==12 ? x  : z) : hash<4 ? -y1 : (hash==14 ? -x  : -z));

			hash = int(p[BA]) & 15;
			g7 = ((hash&1) == 0 ? (hash<8 ? x1 : y ) : (hash<8 ? -x1 : -y )) + ((hash&2) == 0 ? hash<4 ? y  : (hash==12 ? x1 : z) : hash<4 ? -y  : (hash==14 ? -x1 : -z));

			hash = int(p[AA]) & 15;
			g8 = ((hash&1) == 0 ? (hash<8 ? x  : y ) : (hash<8 ? -x  : -y )) + ((hash&2) == 0 ? hash<4 ? y  : (hash==12 ? x  : z) : hash<4 ? -y  : (hash==14 ? -x  : -z));

			g2 += u * (g1 - g2);
			g4 += u * (g3 - g4);
			g6 += u * (g5 - g6);
			g8 += u * (g7 - g8);

			g4 += v * (g2 - g4);
			g8 += v * (g6 - g8);

			s += (g8 + w * (g4 - g8)) * fPers;
		}

		return (s * fPersMax + 1) * .5;
	}
}

// Lloyd's relaxation

function lloyd_relax(diagram)
{
	function area(cell)
	{
		var area = 0;
		var edges = cell.halfedges;
		var N = edges.length;

		for (var i = 0; i < N; i++)
		{
			var p1 = edges[i].getStartpoint();
			var p2 = edges[i].getEndpoint();

			area += p1.x * p2.y;
			area -= p1.y * p2.x;
		}

		return area / 2;
	}

	function centroid(cell)
	{
		var edges = cell.halfedges;
		var N = edges.length;

		var x = 0;
		var y = 0;

		for (var i = 0; i < N; i++)
		{
			var p1 = edges[i].getStartpoint();
			var p2 = edges[i].getEndpoint();
			var v = p1.x * p2.y - p2.x * p1.y;

			x += (p1.x + p2.x) * v;
			y += (p1.y + p2.y) * v;
		}

		var v = area(cell) * 6;

		return { x: x / v, y: y / v };
	}

	function distance(a, b)
	{
		var dx = a.x - b.x;
		var dy = a.y - b.y;

		return Math.sqrt(dx * dx + dy * dy);
	}

	var cells = diagram.cells;
	var N = cells.length;
	var sites = [];

	for (var i = 0; i < N; i++)
	{
		var cell = cells[i];
		var p = Math.random();

		if (p < 0.02)
			continue;

		var site = centroid(cell);
		var dist = distance(site, cell.site);

		if (dist > 2)
		{
			site.x = (site.x + cell.site.x) / 2;
			site.y = (site.y + cell.site.y) / 2;
		}

		if (p > 0.98)
		{
			dist /= 2;

			sites.push({
				x: site.x + (site.x - cell.site.x) / dist,
				y: site.y + (site.y - cell.site.y) / dist
			});
		}

		sites.push(site);
	}

	return sites;
}

// Helpers

function rand(min, max)
{
	return Math.floor(Math.random() * (max - min + 1)) + min;
}
