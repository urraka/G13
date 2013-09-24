/**
 *  voronoi_relax(diagram)
 *
 *  Applies the Lloyd's relaxation algorithm to a voronoi diagram,
 *  returning a new list of sites.
 */

(function() {

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

function relax(diagram)
{
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

window.voronoi_relax = relax;

})();
