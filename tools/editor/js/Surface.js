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
		grassBack: [
			sprites["grass-a-1.0"],
			sprites["grass-a-2.0"],
			sprites["grass-a-3.0"],
			sprites["grass-a-4.0"],
			sprites["grass-a-5.0"]
		],
		grassFront: [
			sprites["grass-a-1.1"],
			sprites["grass-a-2.1"],
			sprites["grass-a-3.1"],
			sprites["grass-a-4.1"],
			sprites["grass-a-5.1"]
		],
		grassBack45: [
			sprites["grass-b-1.0"],
			sprites["grass-b-2.0"],
			sprites["grass-b-3.0"],
			sprites["grass-b-4.0"],
			sprites["grass-b-5.0"]
		],
		grassFront45: [
			sprites["grass-b-1.1"],
			sprites["grass-b-2.1"],
			sprites["grass-b-3.1"],
			sprites["grass-b-4.1"],
			sprites["grass-b-5.1"]
		],
		grassAlt: [
			sprites["grass-c-01"],
			sprites["grass-c-02"],
			sprites["grass-c-03"],
			sprites["grass-c-04"],
			sprites["grass-c-05"],
			sprites["grass-c-06"],
			sprites["grass-c-07"],
			sprites["grass-c-08"],
			sprites["grass-c-09"],
			sprites["grass-c-10"]
		]
	}
}

function Surface(points, spritesheet)
{
	this.base.call(this);
	this.batch = null;
	this.points = points.slice(0);

	this.updateLocalBounds();
	this.updateBounds();

	var scale = 0.15;
	var elements = surface_elements(spritesheet.sprites);
	var sprites = [];

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

		sprites.push(sprite);
	}

	// for (var i = 0; i < points.length - 1; i++)
	// {
	// 	var total = distance(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
	// 	var percent = 0;

	// 	var rotation = Math.atan2(points[i + 1].y - points[i].y, points[i + 1].x - points[i].x);

	// 	while (percent < 1)
	// 	{
	// 		var sprite = new gfx.Sprite();

	// 		var grass = elements.grassBack[rand(0, elements.grassBack.length - 1)];
	// 		var length = scale * distance(grass.ax, grass.ay, grass.bx, grass.by);

	// 		var overflow = total * Math.max(0, (percent + length / total) - 1);
	// 		var cut = overflow / scale;

	// 		if (cut > 0)
	// 			cut += grass.w - grass.bx;

	// 		sprite.x = lerp(points[i].x, points[i + 1].x, percent);
	// 		sprite.y = lerp(points[i].y, points[i + 1].y, percent);
	// 		sprite.w = grass.w - cut;
	// 		sprite.h = grass.h;
	// 		sprite.cx = grass.ax;
	// 		sprite.cy = grass.ay;
	// 		sprite.sx = scale;
	// 		sprite.sy = scale;
	// 		sprite.u0 = grass.tl.x / spritesheet.width;
	// 		sprite.v0 = grass.tl.y / spritesheet.height;
	// 		sprite.u1 = (grass.br.x - cut) / spritesheet.width;
	// 		sprite.v1 = grass.br.y / spritesheet.height;
	// 		sprite.rotation = rotation;

	// 		sprites.push(sprite);

	// 		percent += length / total;
	// 	}
	// }

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
			sprite.rotation = rotation;

			sprites.push(sprite);

			percent += length / total;
		}
	}

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
