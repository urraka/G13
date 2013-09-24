/**
 *  voronoi_polygons(diagram, cell_solid)
 *
 *  Returns a list of polygons from a voronoi diagram, given a function
 *  that returns true for solid cells.
 */

(function() {

// Expands a polygon defined by an array of halfedges recursively
// to contain the adjacent solid cells

function expand(polygon, cell, cells, cell_solid, visited)
{
	var neighbors = [];
	visited[cell.site.voronoiId] = true;

	while (cell !== null)
	{
		if (polygon.length === 0)
		{
			polygon = cell.halfedges.slice(0);
		}
		else
		{
			var a = polygon;        // polygon to expand
			var b = cell.halfedges; // polygon to expand with

			var aSize = a.length;
			var bSize = b.length;

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

			polygon = c;
		}

		// add neighbors

		var ids = cell.getNeighborIds();

		for (var i = 0; i < ids.length; i++)
		{
			if (!visited[ids[i]] && cell_solid(cells[ids[i]]))
			{
				visited[ids[i]] = true;
				neighbors.push(cells[ids[i]]);
			}
		}

		cell = null;

		if (neighbors.length > 0)
			cell = neighbors.pop();
	}

	return polygon;
}

function voronoi_polygons(diagram, cell_solid)
{
	var visited = [];
	var polygons = [];

	var cells = diagram.cells;
	var N = cells.length;

	for (var i = 0; i < N; i++)
		visited.push(false);

	for (var i = 0; i < N; i++)
	{
		if (visited[i] || !cell_solid(cells[i]))
			continue;

		var polygon = expand([], cells[i], cells, cell_solid, visited);

		for (var j = 0; j < polygon.length; j++)
			polygon[j] = polygon[j].getStartpoint();

		polygons.push(polygon_refine(polygon));
	}

	return polygons;
}

window.voronoi_polygons = voronoi_polygons;

})();
