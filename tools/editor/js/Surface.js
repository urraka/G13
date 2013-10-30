(function() {

g13["Surface"] = Surface;

inherit(Surface, g13.Object);

var cache = {
	matrix1: mat3.create()
};

function surface_elements(sprites)
{
	return {
		floors: [
			sprites["floor-1.png"],
			sprites["floor-2.png"],
			sprites["floor-3.png"],
			sprites["floor-4.png"],
			sprites["floor-5.png"]
		],
		bushes: [
			sprites["bush-1.png"],
			sprites["bush-2.png"],
			sprites["bush-3.png"],
			sprites["bush-4.png"],
			sprites["bush-5.png"],
			sprites["bush-6.png"]
		],
		grassFront: [
			sprites["grass-a-1.0.png"],
			sprites["grass-a-2.0.png"],
			sprites["grass-a-3.0.png"],
			sprites["grass-a-4.0.png"],
			sprites["grass-a-5.0.png"]
		],
		grassBack: [
			sprites["grass-a-1.1.png"],
			sprites["grass-a-2.1.png"],
			sprites["grass-a-3.1.png"],
			sprites["grass-a-4.1.png"],
			sprites["grass-a-5.1.png"]
		],
		grassBack45: [
			sprites["grass-b-1.0.png"],
			sprites["grass-b-2.0.png"],
			sprites["grass-b-3.0.png"],
			sprites["grass-b-4.0.png"],
			sprites["grass-b-5.0.png"]
		],
		grassFront45: [
			sprites["grass-b-1.1.png"],
			sprites["grass-b-2.1.png"],
			sprites["grass-b-3.1.png"],
			sprites["grass-b-4.1.png"],
			sprites["grass-b-5.1.png"]
		],
		grassAlt: [
			sprites["grass-c-01.png"],
			sprites["grass-c-02.png"],
			sprites["grass-c-03.png"],
			sprites["grass-c-04.png"],
			sprites["grass-c-05.png"],
			sprites["grass-c-06.png"],
			sprites["grass-c-07.png"],
			sprites["grass-c-08.png"],
			sprites["grass-c-09.png"],
			sprites["grass-c-10.png"]
		]
	}
}

function Surface(points, spritesheet)
{
	this.base.call(this);
	this.batch = null;
	this.points = points.slice(0);
	this.sprites = null;

	this.updateLocalBounds();
	this.updateBounds();

	var scale = 0.15;
	var elements = surface_elements(spritesheet.sprites);
	var sprites = [];

	var spritesGrass = {front: [], back: []};

	for (var i = 0; i < points.length - 1; i++)
	{
		var total = distance(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
		var percent = 0;

		var dx = points[i + 1].x - points[i].x;
		var dy = points[i + 1].y - points[i].y;

		var rotation = Math.atan2(dy, dx);

		var grassBack = elements.grassBack;
		var grassFront = elements.grassFront;

		var flip = false;

		if (dx === 0 || Math.abs(dy / dx) > 1)
		{
			grassBack = elements.grassBack45;
			grassFront = elements.grassFront45;

			if (dy / dx < 0)
				flip = true;
		}

		while (percent < 1)
		{
			var index = rand(0, grassBack.length - 1);

			var back = grassBack[index];
			var front = grassFront[index];

			var length = scale * distance(back.ax, back.ay, back.bx, back.by);
			var overflow = total * Math.max(0, (percent + length / total) - 1);

			var x = lerp(points[i].x, points[i + 1].x, percent);
			var y = lerp(points[i].y, points[i + 1].y, percent);

			if (overflow > 0)
			{
				x = lerp(points[i].x, points[i + 1].x, 1 - length / total);
				y = lerp(points[i].y, points[i + 1].y, 1 - length / total);
			}

			var sy = rand(90, 120) / 100;

			var spriteBack = new gfx.Sprite();

			spriteBack.x = x;
			spriteBack.y = y;
			spriteBack.w = back.w;
			spriteBack.h = back.h;
			spriteBack.cx = flip ? back.bx : back.ax;
			spriteBack.cy = back.h;
			spriteBack.sx = scale * (flip ? -1 : 1);
			spriteBack.sy = scale * sy;
			spriteBack.u0 = back.tl.x / spritesheet.width;
			spriteBack.v0 = back.tl.y / spritesheet.height;
			spriteBack.u1 = back.br.x / spritesheet.width;
			spriteBack.v1 = back.br.y / spritesheet.height;
			spriteBack.uvrot = back.br.x - back.tl.x < 0;
			spriteBack.rotation = rotation;

			var spriteFront = new gfx.Sprite();

			spriteFront.x = x;
			spriteFront.y = y;
			spriteFront.w = front.w;
			spriteFront.h = front.h;
			spriteFront.cx = flip ? front.bx : front.ax;
			spriteFront.cy = front.h;
			spriteFront.sx = scale * (flip ? -1 : 1);
			spriteFront.sy = scale * sy;
			spriteFront.u0 = front.tl.x / spritesheet.width;
			spriteFront.v0 = front.tl.y / spritesheet.height;
			spriteFront.u1 = front.br.x / spritesheet.width;
			spriteFront.v1 = front.br.y / spritesheet.height;
			spriteFront.uvrot = front.br.x - front.tl.x < 0;
			spriteFront.rotation = rotation;

			spritesGrass.back.push(spriteBack);
			spritesGrass.front.push(spriteFront);

			percent += length / total;
		}
	}

	sprites = sprites.concat(spritesGrass.back, spritesGrass.front);

	for (var i = 0; i < points.length - 1; i++)
	{
		var total = distance(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
		var percent = 0;

		var rotation = Math.atan2(points[i + 1].y - points[i].y, points[i + 1].x - points[i].x);

		while (percent < 1)
		{
			var sprite = new gfx.Sprite();

			var floor = elements.floors[rand(0, elements.floors.length - 1)];
			var length = scale * distance(floor.ax, floor.ay, floor.bx, floor.by);

			var overflow = total * Math.max(0, (percent + length / total) - 1);
			var cut = overflow / scale;

			if (cut > 0)
				cut += floor.w - floor.bx;

			sprite.x = lerp(points[i].x, points[i + 1].x, percent);
			sprite.y = lerp(points[i].y, points[i + 1].y, percent);
			sprite.w = floor.w - cut;
			sprite.h = floor.h;
			sprite.cx = floor.ax;
			sprite.cy = floor.ay;
			sprite.sx = scale;
			sprite.sy = scale;
			sprite.u0 = floor.tl.x / spritesheet.width;
			sprite.v0 = floor.tl.y / spritesheet.height;
			sprite.u1 = (floor.br.x - cut) / spritesheet.width;
			sprite.v1 = floor.br.y / spritesheet.height;
			sprite.uvrot = floor.br.x - floor.tl.x < 0;
			sprite.rotation = rotation;

			sprites.push(sprite);

			percent += length / total;
		}
	}

	for (var i = 0; i < points.length; i++)
	{
		var sprite = new gfx.Sprite();

		var bush = elements.bushes[rand(0, elements.bushes.length - 1)];

		sprite.x = points[i].x;
		sprite.y = points[i].y;
		sprite.w = bush.w;
		sprite.h = bush.h;
		sprite.cx = bush.cx;
		sprite.cy = bush.cy;
		sprite.sx = scale;
		sprite.sy = scale;
		sprite.u0 = bush.tl.x / spritesheet.width;
		sprite.v0 = bush.tl.y / spritesheet.height;
		sprite.u1 = bush.br.x / spritesheet.width;
		sprite.v1 = bush.br.y / spritesheet.height;
		sprite.uvrot = bush.br.x - bush.tl.x < 0;

		sprites.push(sprite);
	}

	this.sprites = sprites;

	this.batch = new gfx.SpriteBatch(sprites.length, gfx.Static);
	this.batch.texture = spritesheet.texture;

	for (var i = 0; i < sprites.length; i++)
		this.batch.add(sprites[i]);

	this.batch.upload();
}

Surface.prototype.updateLocalBounds = function()
{
	this.localBounds.x = 0;
	this.localBounds.y = 0;
	this.localBounds.w = 0;
	this.localBounds.h = 0;

	var points = this.points;

	if (points.length > 0)
	{
		this.localBounds.x = points[0].x;
		this.localBounds.y = points[0].y;

		var b = {x: 0, y: 0, w: 0, h: 0};

		for (var i = 1; i < points.length; i++)
		{
			b.x = points[i].x;
			b.y = points[i].y;

			rect_expand(this.localBounds, b);
		}
	}
}

Surface.prototype.snaptest = function(x, y, r, p)
{
	var bx = this.bounds.x - r;
	var by = this.bounds.y - r;
	var bw = this.bounds.w + r * 2;
	var bh = this.bounds.h + r * 2;

	if (!rect_contains(bx, by, bw, bh, x, y))
		return false;

	var points = this.points;
	var N = points.length;

	r *= r;

	for (var i = 0; i < N; i++)
	{
		var px = points[i].x + this.x;
		var py = points[i].y + this.y;

		if (distance2(x, y, px, py) < r)
		{
			p.x = px;
			p.y = py;

			return true;
		}
	}

	return false;
}

Surface.prototype.hittest = function(x, y)
{
	var p = this.points;

	var D = 20*20;

	for (var i = 0; i < p.length - 1; i++)
	{
		var ax = p[i + 0].x + this.x;
		var ay = p[i + 0].y + this.y;
		var bx = p[i + 1].x + this.x;
		var by = p[i + 1].y + this.y;

		if (distance_to_segment2(x, y, ax, ay, bx, by) < D)
			return true;
	}

	return false;
}

Surface.prototype.intersects = function(x, y, w, h)
{
	var p = this.points;

	for (var i = 0; i < p.length - 1; i++)
	{
		var ax = p[i + 0].x + this.x;
		var ay = p[i + 0].y + this.y;
		var bx = p[i + 1].x + this.x;
		var by = p[i + 1].y + this.y;

		if (rect_intersects_segment(x, y, w, h, ax, ay, bx, by))
			return true;
	}

	return false;
}

Surface.prototype.contained = function(x, y, w, h)
{
	var a = this.bounds;

	return rect_contained(a.x, a.y, a.w, a.h, x, y, w, h);
}

Surface.prototype.draw = function()
{
	var matrix = mat3.copy(gfx.transform(), cache.matrix1);

	gfx.translate(this.x, this.y);
	this.batch.draw();
	gfx.transform(matrix);
}

})();
