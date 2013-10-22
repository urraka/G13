(function() {

g13 = window.g13 || {};
g13["Object"] = Object;

function warn(obj, method)
{
	console.warn(obj.constructor.name + "." + method + "() not implemented.");
}

function Object()
{
	this.bounds = {x: 0, y: 0, w: 0, h: 0};

	this.x   = 0;
	this.y   = 0;
	this.rot = 0;
	this.sx  = 1;
	this.sy  = 1;
	this.kx  = 0;
	this.ky  = 0;
	this.cx  = 0;
	this.cy  = 0;
}

Object.prototype.move       = function(dx, dy)     { warn(this, "move"); }
Object.prototype.hittest    = function(x, y)       { warn(this, "hittest"); }
Object.prototype.intersects = function(x, y, w, h) { warn(this, "intersects"); }
Object.prototype.contained  = function(x, y, w, h) { warn(this, "contained"); }

Object.prototype.moveTo = function(x, y)
{
	this.move(x - this.x, y - this.y);
}

})();
