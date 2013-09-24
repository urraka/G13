/**
 *  polygon_outlines(polygon)
 *
 *  Returns all outlines of a polygon defined by an array of vertices.
 *  Useful for the case of a polygon with holes.
 *  It is assumed that equal vertices share the same instance.
 */

function polygon_outlines(polygon)
{
	var outlines = [];
	var linestrips = [];
	var linestrip = [];

	var N = polygon.length;

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

		var len = linestrip.length;

		if (inner && len > 0)
		{
			linestrips.push(linestrip);
			linestrip = [];
		}
		else if (!inner)
		{
			linestrip.push(i);
		}
	}

	if (linestrip.length > 0)
		linestrips.push(linestrip);

	for (var i = 0; i < linestrips.length; i++)
	{
		var a = linestrips[i][0];
		var b = (linestrips[i][linestrips[i].length - 1] + 1) % N;

		if (polygon[a] === polygon[b])
		{
			outlines.push(linestrips[i]);
			linestrips.splice(i--, 1);
		}
	}

	var len = linestrips.length;

	for (var i = 0; i < len; i++)
	{
		for (var j = i + 1; j < len; j++)
		{
			var a = linestrips[i];
			var b = linestrips[j];

			var ab = (polygon[(a[a.length - 1] + 1) % N] === polygon[b[0]]);
			var ba = (polygon[(b[b.length - 1] + 1) % N] === polygon[a[0]]);

			if (ab || ba)
			{
				var c = ab ? a.concat(b) : b.concat(a);

				linestrips.splice(j--, 1);

				if (polygon[(c[c.length - 1] + 1) % N] === polygon[c[0]])
				{
					outlines.push(c);
					linestrips.splice(i, 1);
					j = i;
				}
				else
				{
					linestrips[i] = c;
				}

				len = linestrips.length;
			}
		}
	}

	for (var i = 0; i < outlines.length; i++)
	{
		for (var j = 0; j < outlines[i].length; j++)
			outlines[i][j] = polygon[outlines[i][j]];
	}

	return outlines;
}
