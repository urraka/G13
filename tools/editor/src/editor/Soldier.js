(function() {

g13 = window.g13 || {};
g13["Soldier"] = Soldier;

function Soldier(x, y)
{
	this.x = 0;
	this.y = 0;
	this.bounds = {x: 0, y: 0, w: 0, h: 0};

	this.setPosition(x, y);
}

Soldier.prototype.setPosition = function(x, y)
{
	this.x = x;
	this.y = y;

	var size = 1.00390625 * 0.15 * 512;
	var L = this.x - size / 2;
	var T = this.y - size / 2;

	this.bounds = {x: L, y: T, w: size, h: size};
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
	var X = this.bounds.x;
	var Y = this.bounds.y;
	var W = this.bounds.w;
	var H = this.bounds.h;

	return X > x && X + W < x + w && Y > y && Y + H < y + h;
}

Soldier.prototype.intersects = function(x, y, w, h)
{
	var X = this.bounds.x;
	var Y = this.bounds.y;
	var W = this.bounds.w;
	var H = this.bounds.h;

	return X < x + w && X + W > x && Y < y + h && Y + H > y;
}

Soldier.prototype.hittest = function(x, y)
{
	var L = this.bounds.x;
	var R = this.bounds.x + this.bounds.w;
	var T = this.bounds.y;
	var B = this.bounds.y + this.bounds.h;

	return x > L && x < R && y > T && y < B;
}

})();
