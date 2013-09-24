function map_export(map)
{
	var hw = map.width / 2;
	var hh = map.height / 2;

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

	for (var i = 0; i < map.diagram.vertices.length; i++)
	{
		var v = map.diagram.vertices[i];

		v.vid_ = i;

		// if (v.x < 0 || v.x > map.width || v.y < 0 || v.y > map.height)
		// 	console.log("out of bounds: [" + i + "] (" + v.x + "," + v.y + ")");

		data.vertices.push({
			x: v.x - hw,
			y: v.y - hh
		});
	}

	// polygons

	for (var i = 0; i < map.polygons.length; i++)
	{
		var src = map.polygons[i];
		var dst = [];

		for (var j = 0; j < src.length; j++)
			dst.push(src[j].vid_);

		data.polygons.push(dst);
	}

	// outlines

	for (var i = 0; i < map.outlines.length; i++)
	{
		var src = map.outlines[i];
		var dst = [];

		for (var j = 0; j < src.length; j++)
			dst.push(src[j].vid_);

		data.outlines.push(dst);
	}

	// diagram edges

	for (var i = 0; i < map.diagram.edges.length; i++)
	{
		var edge = map.diagram.edges[i];

		data.diagram.edges.push({
			a: edge.va.vid_,
			b: edge.vb.vid_
		});
	}

	// diagram cells

	for (var i = 0; i < map.diagram.cells.length; i++)
	{
		var edges = map.diagram.cells[i].halfedges;
		var dst = [];

		for (var j = 0; j < edges.length; j++)
			dst.push(edges[j].getStartpoint().vid_);

		data.diagram.cells.push(dst);
	}

	return data;
}
