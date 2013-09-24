(function() {

// params:
// - seed (to be implemented)
// - width
// - height
// - offset
// - noise-scale
// - relaxation

function map_generate(params)
{
	var map = {
		width: params["width"],
		height: params["height"],
		diagram: null,
		polygons: null,
		outlines: null
	};

	var width = params["width"];
	var height = params["height"];
	var offset = params["offset"];

	params["noise-seed"] = {
		x: rand(0, width), // TODO: generate these values from seed
		y: rand(0, height)
	};

	var voronoi = new Voronoi();
	var bbox = { xl: 0, xr: width, yt: 0, yb: height };
	var n = (width * height) / (offset * offset);

	var sites = [];

	for (var i = 0; i < n; i++)
		sites.push({ x: rand(0, width), y: rand(0, height) }); // TODO: generate these values from seed

	var diagram = voronoi.compute(sites, bbox);

	for (var i = 0; i < params["relaxation"]; i++)
	{
		sites = voronoi_relax(diagram);
		diagram = voronoi.compute(sites, bbox);
	}

	var polygons = voronoi_polygons(diagram, function(cell) { return cell_solid(cell, params); });

	if (params["simplification"] > 0)
	{
		for (var i = 0; i < polygons.length; i++)
			polygons[i] = polygon_simplify(polygons[i], params["simplification"]);
	}

	var outlines = [];

	for (var i = 0; i < polygons.length; i++)
		outlines = outlines.concat(polygon_outlines(polygons[i]));

	map.diagram = diagram;
	map.polygons = polygons;
	map.outlines = outlines;

	return map;
}

function cell_solid(cell, params)
{
	var width = params["width"];
	var height = params["height"];
	var offset = params["offset"];
	var seedx = params["noise-seed"].x;
	var seedy = params["noise-seed"].y;
	var noiseScale = params["noise-scale"];

	var point = cell.site;
	var border = false;

	var N = cell.halfedges.length;

	for (var i = 0; i < N; i++)
	{
		var p1 = cell.halfedges[i].getStartpoint();
		var p2 = cell.halfedges[i].getEndpoint();

		if (p1.x < offset || p1.x > width - offset || p1.y < offset || p1.y > height - offset ||
			p2.x < offset || p2.x > width - offset || p2.y < offset || p2.y > height - offset)
		{
			border = true;
			break;
		}
	}

	if (border)
		return true;

	var noise = perlin_noise(seedx + point.x * noiseScale, seedy + point.y * noiseScale, 0);

	var dx = point.x - width / 2;
	var dy = point.y - height / 2;

	var distance = dx * dx + dy * dy;

	return noise > (0.55 - 0.2 * distance / (width * height));
}

window.map_generate = map_generate;

})();
