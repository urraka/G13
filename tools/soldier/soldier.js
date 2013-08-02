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
var soldier = null;

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

	soldier = new Soldier();

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

	context.drawImage(images["atlas"], 0, 0);

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
	this.armsAtlas = this.generateArmsAtlas();

	images["atlas"] = this.armsAtlas.image;
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
	var interval = 6;

	var arm = this.weapon.arms[which];
	var armIndex = Math.floor((angle + 90) / interval);
	var armSprite = this.armsAtlas.sprites[which][armIndex];

	var x = this.frame.arms[which].x;
	var y = this.frame.arms[which].y;

	var angle1 = armIndex * interval - 90;
	var angle2 = angle;

	var grabpoint1 = this.grabpoint(arm.grabx, arm.graby, angle1, 0, 0);
	grabpoint1.x -= x;
	grabpoint1.y -= y;

	var grabpoint2 = this.grabpoint(arm.grabx, arm.graby, angle2, this.weaponShake.x, this.weaponShake.y);
	grabpoint2.x -= x;
	grabpoint2.y -= y;

	var ang1 = Math.atan2(grabpoint1.y, grabpoint1.x);
	var ang2 = Math.atan2(grabpoint2.y, grabpoint2.x);

	var dist1 = Math.sqrt(Math.pow(grabpoint1.x, 2) + Math.pow(grabpoint1.y, 2));
	var dist2 = Math.sqrt(Math.pow(grabpoint2.x, 2) + Math.pow(grabpoint2.y, 2));

	var w = armSprite.width;
	var h = armSprite.height;

	context.save();
	context.translate(x, y);
	context.rotate(ang1);
	context.scale(dist2 / dist1, 1);
	context.rotate(-ang1 + ang2);
	context.translate(-armSprite.cx, -armSprite.cy);
	context.drawImage(this.armsAtlas.image, armSprite.x, armSprite.y, w, h, 0, 0, w, h);
	context.restore();
}

Soldier.prototype.generateArmSprite = function(which, angle, thickness, scale)
{
	var arm = this.weapon.arms[which];
	var x = this.frame.arms[which].x;
	var y = this.frame.arms[which].y;

	var grabpoint = this.grabpoint(arm.grabx, arm.graby, angle, 0, 0);
	grabpoint.x -= x;
	grabpoint.y -= y;

	var ctrlpoint = {
		x: grabpoint.x * 0.5 + arm.offsx,
		y: grabpoint.y * 0.5 + arm.offsy
	};

	var armAngle = to_deg(Math.atan2(grabpoint.y, grabpoint.x));

	var point = svg.createSVGPoint();
	var matrix = svg.createSVGMatrix();

	matrix = matrix.rotate(-armAngle);

	point.x = grabpoint.x;
	point.y = grabpoint.y;
	point = point.matrixTransform(matrix);

	grabpoint.x = point.x;
	grabpoint.y = point.y;

	point.x = ctrlpoint.x;
	point.y = ctrlpoint.y;
	point = point.matrixTransform(matrix);

	ctrlpoint.x = point.x;
	ctrlpoint.y = point.y;

	var xmin = Math.min(0, Math.min(grabpoint.x, ctrlpoint.x));
	var ymin = Math.min(0, Math.min(grabpoint.y, ctrlpoint.y));
	var xmax = Math.max(0, Math.max(grabpoint.x, ctrlpoint.x));
	var ymax = Math.max(0, Math.max(grabpoint.y, ctrlpoint.y));

	var image = document.createElement("canvas");
	var context = image.getContext("2d");

	image.width = Math.ceil((xmax - xmin + 2 * (thickness + 1)) * scale);
	image.height = Math.ceil((ymax - ymin + 2 * (thickness + 1)) * scale);

	context.strokeStyle = "black";
	context.lineCap = "round";
	context.lineWidth = thickness;
	context.scale(scale, scale);
	context.translate(thickness + 1 + Math.abs(xmin), thickness + 1 + Math.abs(ymin));
	context.beginPath();
	context.moveTo(0, 0);
	context.quadraticCurveTo(ctrlpoint.x, ctrlpoint.y, grabpoint.x, 0);
	context.stroke();

	return {
		image: image,
		cx: (thickness + 1 + Math.abs(xmin)) * scale,
		cy: (thickness + 1 + Math.abs(ymin)) * scale
	};
}

Soldier.prototype.generateArmsAtlas = function()
{
	var sprites = [[],[]];
	var blocks = [];
	var interval = 6;
	var thickness = 5;

	for (var iArm = 0; iArm < 2; iArm++)
	{
		for (var i = 0; i <= (180 / 6); i++)
		{
			var angle = i * interval - 90;
			var sprite = this.generateArmSprite(iArm, angle, thickness, 1);
			var offset = crop(sprite.image);

			sprite.cx -= offset.x0;
			sprite.cy -= offset.y0;

			blocks.push({
				w: sprite.image.width + 1,
				h: sprite.image.height + 1,
				sprite: sprite
			});

			sprites[iArm].push(sprite);
		}
	}

	blocks.sort(function(a, b) { return Math.max(b.w, b.h) < Math.max(a.w, a.h); });

	var packer = new GrowingPacker();
	packer.fit(blocks);

	var w = packer.root.w + 1;
	var h = packer.root.h + 1;

	var atlas = document.createElement("canvas");
	var context = atlas.getContext("2d");

	atlas.width = w;
	atlas.height = h;

	for (var i = 0; i < blocks.length; i++)
	{
		var x = blocks[i].fit.x + 1;
		var y = blocks[i].fit.y + 1;
		var image = blocks[i].sprite.image;
		var imageData = image.getContext("2d").getImageData(0, 0, image.width, image.height);

		context.putImageData(imageData, x, y, 0, 0, image.width, image.height);

		blocks[i].sprite.x = x;
		blocks[i].sprite.y = y;
		blocks[i].sprite.width = image.width;
		blocks[i].sprite.height = image.height;

		delete blocks[i].sprite.image;
	}

	return { image: atlas, sprites: sprites };
}

Soldier.prototype.grabpoint = function(x, y, angle, shakex, shakey)
{
	var matrix = svg.createSVGMatrix();
	matrix = matrix.translate(shakex, shakey);
	matrix = matrix.rotate(angle);
	matrix = matrix.translate(this.weaponDistance(angle) - this.weapon.cx, -this.weapon.cy);

	var result = svg.createSVGPoint();
	result.x = x;
	result.y = y;

	return result.matrixTransform(matrix);
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

function crop(image)
{
	var w = image.width;
	var h = image.height;

	var context = image.getContext("2d");
	var imageData = context.getImageData(0, 0, w, h);
	var data = imageData.data;

	var size = w * h * 4;

	var x0 = 0;
	var x1 = w - 1;
	var y0 = 0;
	var y1 = h - 1;

	// x0

	for (var x = 0; x < w; x++)
	{
		for (var y = 0; y < h; y++)
		{
			if (data[(y * w + x) * 4 + 3] != 0)
			{
				x0 = x;
				break;
			}
		}

		if (y < h) break;
	}

	// x1

	for (var x = w - 1; x >= 0; x--)
	{
		for (var y = 0; y < h; y++)
		{
			if (data[(y * w + x) * 4 + 3] != 0)
			{
				x1 = x;
				break;
			}
		}

		if (y < h) break;
	}

	// y0

	for (var i = 0; i < size; i += 4)
	{
		if (data[i + 3] != 0)
		{
			y0 = Math.floor(i / (w * 4));
			break;
		}
	}

	// y1

	for (var i = size - 4; i >= 0; i -= 4)
	{
		if (data[i + 3] != 0)
		{
			y1 = Math.floor(i / (w * 4));
			break;
		}
	}

	// crop

	image.width = x1 - x0 + 1;
	image.height = y1 - y0 + 1;
	context.putImageData(imageData, -x0, -y0);

	return { "x0": x0, "y0": y0 };
}
