function map_draw(map, context)
{
	if (!map || !map.diagram)
		return;

	context.translate(-map.width / 2, -map.height / 2);

	// background

	context.fillStyle = "#8080FF";
	context.fillRect(0, 0, map.width, map.height);

	// polygons (black background)

	context.fillStyle = "#000";

	for (var i = 0; i < map.polygons.length; i++)
	{
		var polygon = map.polygons[i];

		context.beginPath();
		context.moveTo(polygon[0].x, polygon[0].y);

		for (var j = 0; j < polygon.length; j++)
			context.lineTo(polygon[j].x, polygon[j].y);

		context.closePath();
		context.fill();
	}

	// voronoi diagram edges

	context.strokeStyle = "#333";

	for (var i = 0; i < map.diagram.edges.length; i++)
	{
		var edge = map.diagram.edges[i];

		context.beginPath()
		context.moveTo(edge.va.x, edge.va.y);
		context.lineTo(edge.vb.x, edge.vb.y);
		context.stroke();
	}

	// polygons

	context.fillStyle = "rgba(0, 255, 0, 0.5)";
	context.strokeStyle = "rgba(255, 255, 0, 0.5)";

	for (var i = 0; i < map.polygons.length; i++)
	{
		var polygon = map.polygons[i];

		context.beginPath();
		context.moveTo(polygon[0].x, polygon[0].y);

		for (var j = 0; j < polygon.length; j++)
			context.lineTo(polygon[j].x, polygon[j].y);

		context.closePath();
		context.fill();
		context.stroke();
	}

	// outlines

	context.strokeStyle = "#000";
	context.lineWidth = 3;

	for (var i = 0; i < map.outlines.length; i++)
	{
		var outline = map.outlines[i];

		context.beginPath();
		context.moveTo(outline[0].x, outline[0].y);

		for (var j = 0; j < outline.length; j++)
			context.lineTo(outline[j].x, outline[j].y);

		context.closePath();
		context.stroke();
	}
}
