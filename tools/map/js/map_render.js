(function() {

lineBatch = null;
vbosel = null;

function map_render(map, update)
{
	if (update)
		initialize(map);

	gfx.translate(-map.width / 2, -map.height / 2);

	if (false && selectedCell)
	{
		var cell = selectedCell;
		var n = cell.halfedges.length + 1;

		if (!vbosel)
		{
			vbosel = new gfx.VBO(n, gfx.Dynamic);
			vbosel.mode = gfx.TriangleFan;
		}

		if (vbosel.size < n)
			vbosel.resize(n, gfx.Dynamic);

		for (var i = 0; i < n; i++)
		{
			var p = cell.halfedges[i % (n - 1)].getStartpoint();
			vbosel.set(i, p.x, p.y, 0, 0, 0, 128, 0, 1);
		}

		vbosel.upload();
		gfx.bind(gfx.White);
		gfx.draw(vbosel, null, 0, n);
	}

	if (selectedCell)
	{
		var cell = selectedCell;
		var nEdges = cell.halfedges.length;

		var start = 0;
		var count = nEdges;

		for (var i = 0; i < nEdges; i++)
		{
			var edge = cell.halfedges[i].edge;
			var site = (edge.rSite !== cell.site ? edge.rSite : edge.lSite);

			if (site === null)
			{
				start = i + 1;
				count--;
			}
		}

		var add = (count === nEdges ? 1 : count - nEdges);

		var n = 1 + nEdges + add;

		if (!vbosel)
		{
			vbosel = new gfx.VBO(n, gfx.Dynamic);
			vbosel.mode = gfx.TriangleFan;
		}

		if (vbosel.size < n)
			vbosel.resize(n, gfx.Dynamic);

		vbosel.set(0, cell.site.x, cell.site.y, 0, 0, 0, 128, 0, 1);

		for (var i = 0; i < nEdges + add; i++)
		{
			var edge = cell.halfedges[(start + i) % nEdges].edge;
			var site = (edge.rSite !== cell.site ? edge.rSite : edge.lSite);

			vbosel.set(i + 1, site.x, site.y, 0, 0, 0, 128, 0, 1);
		}

		vbosel.upload();
		gfx.bind(gfx.White);
		gfx.draw(vbosel, null, 0, n);
	}

	lineBatch.draw();
}

function initialize(map)
{
	if (!lineBatch)
		lineBatch = new gfx.LineBatch();

	lineBatch.clear();
	lineBatch.width(1);
	lineBatch.color(0, 0, 0, 0.5);

	var edges = map.diagram.edges;

	for (var i = 0; i < edges.length; i++)
	{
		lineBatch.move_to(edges[i].va.x, edges[i].va.y);
		lineBatch.line_to(edges[i].vb.x, edges[i].vb.y);
	}

	lineBatch.upload();
}

window.map_render = map_render;

})();
