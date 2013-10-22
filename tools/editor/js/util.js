function inherit(sub, base)
{
	function tmp() {}
	tmp.prototype = base.prototype;
	sub.prototype = new tmp();
	sub.prototype.constructor = sub;
	sub.prototype.base = base;
}

function array_remove(array, item)
{
	var index = array.indexOf(item);

	if (index !== -1)
		array.splice(index, 1);
}

function sqr(x)
{
	return x * x;
}

function distance(ax, ay, bx, by)
{
	return Math.sqrt(distance2(ax, ay, bx, by));
}

function distance2(ax, ay, bx, by)
{
	return sqr(bx - ax) + sqr(by - ay);
}

function rect_expand(a, b)
{
	var R = a.x + a.w;
	var B = a.y + a.h;

	a.x = Math.min(a.x, b.x);
	a.y = Math.min(a.y, b.y);
	a.w = Math.max(R, b.x + b.w) - a.x;
	a.h = Math.max(B, b.y + b.h) - a.y;
}

function rect_assign(a, b)
{
	a.x = b.x;
	a.y = b.y;
	a.w = b.w;
	a.h = b.h;
}

function rect_contained(ax, ay, aw, ah, bx, by, bw, bh)
{
	return ax > bx && ax + aw < bx + bw && ay > by && ay + ah < by + bh;
}

function rects_intersect(ax, ay, aw, ah, bx, by, bw, bh)
{
	return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}

(function() {
    function ccw(ax, ay, bx, by, cx, cy)
    {
        return (cy - ay) * (bx - ax) > (by - ay) * (cx - ax);
    }

    function segments_intersect(ax, ay, bx, by, cx, cy, dx, dy)
    {
        return ccw(ax, ay, cx, cy, dx, dy) !== ccw(bx, by, cx, cy, dx, dy) &&
            ccw(ax, ay, bx, by, cx, cy) !== ccw(ax, ay, bx, by, dx, dy);
    }

    window.segments_intersect = segments_intersect;
})();

function rect_contains(x, y, w, h, px, py)
{
	return px > x && px < x + w && py > y && py < y + h;
}

function rect_intersects_triangle(x, y, w, h, ax, ay, bx, by, cx, cy)
{
	return (
		triangle_contains(ax, ay, bx, by, cx, cy,     x,     y)        ||
		triangle_contains(ax, ay, bx, by, cx, cy, x + w,     y)        ||
		triangle_contains(ax, ay, bx, by, cx, cy, x + w, y + h)        ||
		triangle_contains(ax, ay, bx, by, cx, cy,     x, y + h)        ||
		rect_contains(x, y, w, h, ax, ay)                              ||
		rect_contains(x, y, w, h, bx, by)                              ||
		rect_contains(x, y, w, h, cx, cy)                              ||
		segments_intersect(    x,     y, x + w,     y, ax, ay, bx, by) ||
		segments_intersect(    x,     y, x + w,     y, bx, by, cx, cy) ||
		segments_intersect(    x,     y, x + w,     y, cx, cy, ax, ay) ||
		segments_intersect(x + w,     y, x + w, y + h, ax, ay, bx, by) ||
		segments_intersect(x + w,     y, x + w, y + h, bx, by, cx, cy) ||
		segments_intersect(x + w,     y, x + w, y + h, cx, cy, ax, ay) ||
		segments_intersect(x + w, y + h,     x, y + h, ax, ay, bx, by) ||
		segments_intersect(x + w, y + h,     x, y + h, bx, by, cx, cy) ||
		segments_intersect(x + w, y + h,     x, y + h, cx, cy, ax, ay) ||
		segments_intersect(    x, y + h,     x,     y, ax, ay, bx, by) ||
		segments_intersect(    x, y + h,     x,     y, bx, by, cx, cy) ||
		segments_intersect(    x, y + h,     x,     y, cx, cy, ax, ay)
	);
}

function triangle_contains(ax, ay, bx, by, cx, cy, x, y)
{
	var A = 0.5 * (-by * cx + ay * (-bx + cx) + ax * (by - cy) + bx * cy);
	var sign = A < 0 ? -1 : 1;

	var s = (ay * cx - ax * cy + (cy - ay) * x + (ax - cx) * y) * sign;
	var t = (ax * by - ay * bx + (ay - by) * x + (bx - ax) * y) * sign;

	return s >= 0 && t >= 0 && (s + t) <= (2 * A * sign);
}
