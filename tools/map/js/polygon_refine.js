/**
 *  polygon_refine(polygon)
 *
 *  Removes unnecessary edges from a polygon defined by an array of vertices.
 */

function polygon_refine(polygon)
{
	var N = polygon.length;

	if (N <= 2)
		return polygon;

	for (var i = 0; i < N; i++)
	{
		var end = (i + 1) % N;
		var j = 1;

		while (polygon[(end - j + N) % N] === polygon[(end + j + N) % N])
			j++;

		var count = j - 1;

		if (count > 0)
		{
			var a = (end - count + N) % N;
			var b = (end + count + N) % N;

			if (a < b)
			{
				polygon.splice(a + 1, b - a);
				i = a - 1;
			}
			else
			{
				polygon.splice(a + 1, N - (a + 1));
				polygon.splice(0, b + 1);
				i = -1;
			}

			N = polygon.length;
		}
	}

	return polygon;
}
