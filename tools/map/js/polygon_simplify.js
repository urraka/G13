/**
 *  polygon_simplify(polygon, epsilon)
 *
 *  Simplifies a polygon defined by an array of vertices, applying
 *  the Ramer-Douglas-Peucker algorithm.
 *
 *  TODO:
 *  - after simplification self-intersection can happen
 *  - simplify inner linestrips
 */

(function() {

function sqr(x)
{
	return x * x;
}

function dist2(v, w)
{
	return sqr(v.x - w.x) + sqr(v.y - w.y);
}

function distToSegmentSquared(p, v, w)
{
	var l2 = dist2(v, w);

	if (l2 == 0)
		return dist2(p, v);

	var t = ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;

	if (t < 0) return dist2(p, v);
	if (t > 1) return dist2(p, w);

	return dist2(p, { x: v.x + t * (w.x - v.x), y: v.y + t * (w.y - v.y) });
}

function simplify(linestrip, epsilon)
{
	var N = linestrip.length;

	var dmax = 0;
	var index = 0;

	for (var i = 1; i < N - 1; i++)
	{
		var d = distToSegmentSquared(linestrip[i], linestrip[0], linestrip[N - 1]);

		if (d > dmax)
		{
			index = i;
			dmax = d;
		}
	}

	if (Math.sqrt(dmax) > epsilon)
	{
		var a = simplify(linestrip.slice(0, index + 1), epsilon);
		var b = simplify(linestrip.slice(index, N), epsilon);

		for (var i = 1; i < b.length; i++)
			a.push(b[i]);

		return a;
	}

	return [linestrip[0], linestrip[N - 1]];
}

function polygon_simplify(polygon, epsilon)
{
	var ranges = [];
	var rangeStart = null;
	var N = polygon.length;

	// find ranges of the polygon which don't have shared edges

	for (var i = 0; i < N; i++)
	{
		var a = polygon[i];
		var b = polygon[(i + 1) % N];

		var inner = false;

		for (var j = 0; j < N; j++)
		{
			var c = polygon[j];
			var d = polygon[(j + 1) % N];

			if (a === d && b === c)
			{
				inner = true;
				break;
			}
		}

		if (inner && rangeStart !== null)
		{
			ranges.push({ start: rangeStart, end: i + 1 });
			rangeStart = null;
		}
		else if (!inner && rangeStart === null)
		{
			rangeStart = i;
		}
	}

	if (rangeStart !== null)
		ranges.push({ start: rangeStart, end: N });

	var result = [];
	var prevend = 0;

	for (var i = 0; i < ranges.length; i++)
	{
		result = result.concat(polygon.slice(prevend, Math.max(0, ranges[i].start)));
		result = result.concat(simplify(polygon.slice(ranges[i].start, ranges[i].end), epsilon));
		prevend = ranges[i].end;
	}

	return polygon_refine(result);
}

window.polygon_simplify = polygon_simplify;

})();
