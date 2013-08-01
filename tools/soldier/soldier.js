// -----------------------------------------------------------------------------
// Parameters
// -----------------------------------------------------------------------------

var soldier_frame = {
	image: "soldier",
	cx: 169,
	cy: 330,
	eyes: {
		image: "eyes",
		cx: 82,
		cy: 7,
		miny: -290,
		maxy: -90,
		arc: { x: -30, y: -185, r: 180 }
	},
	arms: [
		{ x: -108, y: -30 },
		{ x:  108, y: -30 }
	]
};

var soldier_weapon = {
	image: "weapon",
	cx: 337,
	cy: 45,
	distances: [400, 300, 275, 275, 250],
	arms: [
		{
			grabx: 82,
			graby: 82,
			offsx: -20,
			offsy: 30
		},
		{
			grabx: 232,
			graby: 71,
			offsx: 20,
			offsy: 30
		}
	]
};

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

var svg = document.createElementNS("http://www.w3.org/2000/svg", "svg");
var canvas = null;
var context = null;
var SCALE = 0.5;
var W = 0;
var H = 0;
var CX = 0;
var CY = 0;
var images = {};
var timerID = null;
var soldier = new Soldier();

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------

function main()
{
	canvas = document.getElementsByTagName("canvas")[0];
	context = canvas.getContext("2d");

	images["soldier"] = document.getElementById("soldier");
	images["eyes"] = document.getElementById("eyes");
	images["weapon"] = document.getElementById("weapon");

	window.onresize    = resize;
	window.onmousemove = function(e) { mouse(e.clientX, e.clientY); };
	window.onmousedown = function(e) { click(true); };
	window.onmouseup   = function(e) { click(false); };

	resize();
}

// -----------------------------------------------------------------------------
// Events
// -----------------------------------------------------------------------------

function resize()
{
	W = canvas.offsetWidth;
	H = canvas.offsetHeight;

	CX = Math.floor(W / 2) + 0.5;
	CY = Math.floor(H / 2) + 0.5;

	canvas.width = W;
	canvas.height = H;

	draw();
}

function mouse(x, y)
{
	soldier.target.x = (x - CX) * (1.0 / SCALE);
	soldier.target.y = (y - CY) * (1.0 / SCALE);

	draw();
}

function click(pressed)
{
	if (pressed && timerID === null)
	{
		soldier.shooting = true;
		timerID = setInterval(draw, 1000 / 60);
	}
	else
	{
		soldier.shooting = false;
		clearInterval(timerID);
		timerID = null;
	}
}

// -----------------------------------------------------------------------------
// Draw
// -----------------------------------------------------------------------------

function draw()
{
	context.save();
	context.fillStyle = "#CCC";
	context.strokeStyle = "rgba(0, 0, 0, 0.1)";
	context.fillRect(0, 0, W, H);

	context.translate(CX, CY);
	context.scale(SCALE, SCALE);

	soldier.draw(context);

	// guides

	context.setTransform(1, 0, 0, 1, 0, 0);

	context.beginPath();
	context.moveTo(CX, 0);
	context.lineTo(CX, H);
	context.stroke();

	context.beginPath();
	context.moveTo(0, CY);
	context.lineTo(W, CY);
	context.stroke();

	var M = Math.max(W, H);

	context.beginPath();
	context.moveTo(-M + CX,  M + CY);
	context.lineTo( M + CX, -M + CY);
	context.stroke();

	context.beginPath();
	context.moveTo(-M + CX, -M + CY);
	context.lineTo( M + CX,  M + CY);
	context.stroke();

	context.restore();
}

// -----------------------------------------------------------------------------
// Soldier
// -----------------------------------------------------------------------------

function Soldier()
{
	this.frame = soldier_frame;
	this.weapon = soldier_weapon;
	this.target = { x: 0, y: 0 };
	this.weaponShake = { x: 0, y: 0 };
	this.shooting = false;
}

Soldier.prototype.draw = function(context)
{
	this.weaponShake.x = 0;
	this.weaponShake.y = 0;

	if (this.shooting)
	{
		var max = 5;

		this.weaponShake.x = rand(-max, max);
		this.weaponShake.y = rand(-max, max);
	}

	var angle = to_deg(Math.atan2(this.target.y, this.target.x));

	if (angle < -90)
		angle = -180 - angle;
	else if (angle > 90)
		angle =  180 - angle;

	if (this.target.x < 0)
		context.scale(-1, 1);

	// body

	context.save();
	context.translate(-this.frame.cx, -this.frame.cy);
	context.drawImage(images[this.frame.image], 0, 0);
	context.restore();

	// eyes

	var eyes = this.frame.eyes;

	var y = eyes.miny + ((angle + 90) / 180) * (eyes.maxy - eyes.miny);
	var x = Math.sqrt(Math.pow(eyes.arc.r, 2) - Math.pow(y - eyes.arc.y, 2)) + eyes.arc.x;

	context.save();
	context.translate(x, y);
	context.translate(-eyes.cx, -eyes.cy);
	context.drawImage(images[eyes.image], 0, 0);
	context.restore();

	context.save();
	context.beginPath();
	context.arc(eyes.arc.x, eyes.arc.y, eyes.arc.r, 0, 2 * Math.PI);
	context.stroke();
	context.restore();

	// arms/weapon

	this.drawArm(context, 1, angle);

	context.save();
	context.translate(this.weaponShake.x, this.weaponShake.y);
	context.rotate(to_rad(angle));
	context.translate(this.weaponDistance(angle), 0);
	context.translate(-this.weapon.cx, -this.weapon.cy);
	context.drawImage(images[this.weapon.image], 0, 0);
	context.restore();

	this.drawArm(context, 0, angle);
}

Soldier.prototype.drawArm = function(context, which, angle)
{
	var arm = this.weapon.arms[which];
	var x = this.frame.arms[which].x;
	var y = this.frame.arms[which].y;

	var interval = 6;

	var angle1 = Math.floor((angle + 90) / interval) * interval - 90;
	var angle2 = angle;

	var weapdist1 = this.weaponDistance(angle1);
	var weapdist2 = this.weaponDistance(angle2);

	var matrix1 = svg.createSVGMatrix();
	matrix1 = matrix1.rotate(angle1);
	matrix1 = matrix1.translate(weapdist1 - this.weapon.cx, -this.weapon.cy);

	var grabpoint1 = svg.createSVGPoint();
	grabpoint1.x = arm.grabx;
	grabpoint1.y = arm.graby;
	grabpoint1 = grabpoint1.matrixTransform(matrix1);
	grabpoint1.x -= x;
	grabpoint1.y -= y;

	var matrix2 = svg.createSVGMatrix();
	matrix2 = matrix2.translate(this.weaponShake.x, this.weaponShake.y);
	matrix2 = matrix2.rotate(angle2);
	matrix2 = matrix2.translate(weapdist2 - this.weapon.cx, -this.weapon.cy);

	var grabpoint2 = svg.createSVGPoint();
	grabpoint2.x = arm.grabx;
	grabpoint2.y = arm.graby;
	grabpoint2 = grabpoint2.matrixTransform(matrix2);
	grabpoint2.x -= x;
	grabpoint2.y -= y;

	var ang1 = Math.atan2(grabpoint1.y, grabpoint1.x);
	var ang2 = Math.atan2(grabpoint2.y, grabpoint2.x);

	var dist1 = Math.sqrt(Math.pow(grabpoint1.x, 2) + Math.pow(grabpoint1.y, 2));
	var dist2 = Math.sqrt(Math.pow(grabpoint2.x, 2) + Math.pow(grabpoint2.y, 2));

	var ctrlpoint = {
		x: grabpoint1.x * 0.5 + arm.offsx,
		y: grabpoint1.y * 0.5 + arm.offsy
	};

	context.save();
	context.strokeStyle = "black";
	context.lineWidth = 5;
	context.beginPath();
	context.translate(x, y);
	context.rotate(ang1);
	context.scale(dist2 / dist1, 1);
	context.rotate(-ang1);
	context.rotate(ang2 - ang1);
	context.moveTo(0, 0);
	context.quadraticCurveTo(ctrlpoint.x, ctrlpoint.y, grabpoint1.x, grabpoint1.y);
	context.stroke();
	context.restore();
}

Soldier.prototype.weaponDistance = function(angle)
{
	var theta = 180 / (this.weapon.distances.length - 1);
	var a = Math.floor((angle + 90) / theta);
	var b = Math.ceil((angle + 90) / theta);
	var percent = (angle - (a * theta - 90)) / theta;

	return lerp(this.weapon.distances[a], this.weapon.distances[b], percent);
}

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

function to_rad(x)
{
	return x * Math.PI / 180;
}

function to_deg(x)
{
	return x * 180 / Math.PI;
}

function lerp(a, b, p)
{
	return a + (b - a) * p;
}

function clamp(value, a, b)
{
	return Math.min(Math.max(value, a), b);
}

function rand(min, max)
{
	return Math.floor(Math.random() * (max - min + 1)) + min;
}
