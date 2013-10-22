(function() {

g13["Soldier"] = Soldier;

inherit(Soldier, g13.Object);

function Soldier(x, y)
{
	this.base.call(this);
	this.updateLocalBounds();
	this.updateBounds();
	this.move(x, y);
}

Soldier.prototype.updateLocalBounds = function()
{
	var size = 512 * 1.00390625 * 0.15;

	this.localBounds.x = -size / 2;
	this.localBounds.y = -size / 2;
	this.localBounds.w = size;
	this.localBounds.h = size;
}

Soldier.prototype.sprite = function(sprite)
{
	sprite = sprite || new gfx.Sprite();

	sprite.x = this.x;
	sprite.y = this.y;
	sprite.w = 512;
	sprite.h = 512;
	sprite.cx = 512 / 2;
	sprite.cy = 512 / 2;
	sprite.sx = 1.00390625 * 0.15;
	sprite.sy = 1.00390625 * 0.15;
	sprite.rotation = 0;
	sprite.kx = sprite.ky = 0;
	sprite.u0 = sprite.v0 = 0;
	sprite.u1 = sprite.v1 = 1;
	sprite.r = sprite.g = sprite.b = 255;
	sprite.a = 1;

	return sprite;
}

Soldier.prototype.contained = function(x, y, w, h)
{
	var a = this.bounds;

	return rect_contained(a.x, a.y, a.w, a.h, x, y, w, h);
}

Soldier.prototype.intersects = function(x, y, w, h)
{
	var a = this.bounds;

	return rects_intersect(a.x, a.y, a.w, a.h, x, y, w, h);
}

Soldier.prototype.hittest = function(x, y)
{
	var a = this.bounds;

	return rect_contains(a.x, a.y, a.w, a.h, x, y);
}

})();
