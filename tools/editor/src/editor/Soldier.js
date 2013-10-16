(function() {

g13 = window.g13 || {};
g13["Soldier"] = Soldier;

function Soldier(x, y)
{
	this.x = x;
	this.y = y;

	var size = 1.00390625 * 0.15 * 512;
	var L = this.x - size / 2;
	var T = this.y - size / 2;

	this.bounds = {x: L, y: T, w: size, h: size};
}

Soldier.prototype.sprite = function()
{
	return new gfx.Sprite({
		x: this.x,
		y: this.y,
		w: 512,
		h: 512,
		cx: 512 / 2,
		cy: 512 / 2,
		sx: 1.00390625 * 0.15,
		sy: 1.00390625 * 0.15
	});
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
