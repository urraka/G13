// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------

var stage = null;
var soldier = null;

function main()
{
	// background

	var backgroundLayer = new Kinetic.Layer({ id: "backgroundLayer" });
	var background = new Kinetic.Rect({ id: "background", fill: "#CCC" });

	backgroundLayer.add(background);

	// scene

	var scale = 1;
	var sceneLayer = new Kinetic.Layer({ id: "sceneLayer" });
	var scene = new Kinetic.Group({ id: "scene", scale: [scale, scale] });

	sceneLayer.add(scene);
	soldier = new Soldier(scene);

	// overlay

	var overlayLayer = new Kinetic.Layer({ id: "overlayLayer" });

	for (var i = 0; i < 4; i++)
	{
		overlayLayer.add(new Kinetic.Line({
			points: [0, 0, 0, 0],
			stroke: "black",
			strokeWidth: 1,
			opacity: 0.1
		}));
	}

	// stage

	stage = new Kinetic.Stage({ container: "container" });
	stage.getContent().style.display = "block";

	stage.add(backgroundLayer);
	stage.add(sceneLayer);
	stage.add(overlayLayer);

	stage.on("mousemove", mousemove);
	stage.on("mousedown", mousedown);
	stage.on("mouseup", mouseup);

	window.addEventListener("resize", resize);
	window.addEventListener("keydown", keydown);
	window.addEventListener("keyup", keyup);

	resize();
	stage.draw();
}

// -----------------------------------------------------------------------------
// Events
// -----------------------------------------------------------------------------

function resize()
{
	var w = window.innerWidth;
	var h = window.innerHeight;

	// update background

	var background = stage.get("#background")[0];

	background.setWidth(w);
	background.setHeight(h);

	// update scene

	var sceneLayer = stage.get("#sceneLayer")[0];

	sceneLayer.setOffsetX(-w / 2);
	sceneLayer.setOffsetY(-h / 2);

	// update overlay

	var lines = stage.get("#overlayLayer")[0].get("Line");

	var m  = Math.max(w, h);
	var cx = Math.floor(w / 2) + 0.5;
	var cy = Math.floor(h / 2) + 0.5;

	lines[0].setPoints([[0, cy], [w, cy]]);
	lines[1].setPoints([[cx, 0], [cx, h]]);
	lines[2].setPoints([[cx - m, cy - m], [cx + m, cy + m]]);
	lines[3].setPoints([[cx - m, cy + m], [cx + m, cy - m]]);

	// update stage

	stage.setWidth(w);
	stage.setHeight(h);
}

function mousemove(evt)
{
	soldier.setTarget(evt.clientX, evt.clientY);
}

function mousedown()
{
	soldier.shoot(true);
}

function mouseup()
{
	soldier.shoot(false);
}

function keydown(evt)
{
	var code = evt.keyCode || evt.which;

	if (code == "W".charCodeAt(0))
		soldier.showWeapon(!soldier.weaponVisible);

	if (code == "R".charCodeAt(0))
		soldier.run(!soldier.running);
}

function keyup(evt)
{
}

// -----------------------------------------------------------------------------
// Soldier
// -----------------------------------------------------------------------------

function Soldier(parent)
{
	var self = this;

	this.strokeWidth = 5;
	this.strokeColor = "black";
	this.bodyColor = "blue";
	this.headColor = "#FFCC99";
	this.weaponColor = "#4C4C4C";

	this.target = { x: 0, y: 0 };
	this.shooting = false;
	this.shake = { x: 0, y: 0 };
	this.weaponVisible = true;
	this.running = false;

	this.root = new Kinetic.Group({ id: "soldier" });

	this.addLegs();
	this.addBody();
	this.addHead();
	this.addWeapon();
	this.addArms();

	this.animationCount = 0;
	this.animation = new Kinetic.Animation(
		function(frame) { self.update(frame); },
		parent.getLayer()
	);

	this.update();

	parent.add(this.root);

	this.run(true);
}

Soldier.prototype.redraw = function()
{
	var layer = this.root.getLayer();

	if (layer)
		layer.batchDraw();
}

Soldier.prototype.showWeapon = function(show)
{
	this.weaponVisible = show;

	if (!show && this.shooting)
		this.shoot(false);

	this.root.get("#weapon")[0].setVisible(show);

	this.update();
	this.redraw();
}

Soldier.prototype.run = function(run)
{
	if (this.running == run)
		return;

	this.running = run;

	if (run)
		this.play();
	else
		this.stop();

	this.update();
	this.redraw();
}

Soldier.prototype.shoot = function(shoot)
{
	if (!this.weaponVisible)
		shoot = false;

	if (this.shooting == shoot)
		return;

	this.shooting = shoot;

	if (shoot)
		this.play();
	else
		this.stop();
}

Soldier.prototype.play = function()
{
	if (this.animationCount === 0)
		this.animation.start();

	this.animationCount++;
}

Soldier.prototype.stop = function()
{
	this.animationCount--;

	if (this.animationCount === 0)
		this.animation.stop();
}

Soldier.prototype.update = function(frame)
{
	if (this.shooting)
	{
		var max = 5;
		this.shake.x = rand(-max, max);
		this.shake.y = rand(-max, max);
	}
	else
	{
		this.shake.x = 0;
		this.shake.y = 0;
	}

	this.updateBody();
	this.updateEyes();
	this.updateWeapon(true);
	this.updateWeapon(false);
	this.updateArms();
	this.updateLegs(frame ? frame.time / 1000 : 0);
}

Soldier.prototype.updateEyes = function()
{
	var miny = -290;
	var maxy = -90;
	var arcx = -30;
	var arcy = -185;
	var arcr = 180;

	var angle = this.getTargetAngle();

	var y = miny + ((angle + 90) / 180) * (maxy - miny);
	var x = Math.sqrt(Math.pow(arcr, 2) - Math.pow(y - arcy, 2)) + arcx;

	var eyes = this.root.get("#eyes")[0];

	eyes.setX(x);
	eyes.setY(y);
}

Soldier.prototype.updateWeapon = function(dummy)
{
	var distances = [400, 300, 275, 275, 250];
	var angle = this.getTargetAngle();

	if (dummy)
	{
		var interval = 6;
		angle = Math.floor((angle + 90) / interval) * interval - 90;
	}

	var theta = 180 / (distances.length - 1);
	var a = Math.floor((angle + 90) / theta);
	var b = Math.ceil((angle + 90) / theta);
	var percent = (angle - (a * theta - 90)) / theta;
	var distance = lerp(distances[a], distances[b], percent);

	var weapon = dummy ? this.root.get("#dummy-weapon")[0] : this.root.get("#weapon")[0];

	var offsetX = this.shake.x + 337 - distance;
	var offsetY = this.shake.y + 45;

	weapon.setRotationDeg(angle);
	weapon.setOffsetX(offsetX);
	weapon.setOffsetY(offsetY);
}

Soldier.prototype.updateArms = function()
{
	var grabpoints = null;
	var ctrlpoints = null;
	var transform = null;

	if (this.weaponVisible)
	{
		var weapon = this.root.get("#weapon")[0];

		transform = weapon.getTransform(true);

		grabpoints = [
			[ 82, 82],
			[232, 71]
		];

		ctrlpoints = [
			[-20, 30],
			[ 20, 30]
		];
	}
	else
	{
		grabpoints = [
			[-108, 30],
			[ 108, 30]
		];

		ctrlpoints = [
			[-160, 0],
			[ 160, 0]
		];
	}

	var arms = [
		this.root.get("#arm-front")[0],
		this.root.get("#arm-back")[0]
	];

	for (var i = 0; i < 2; i++)
	{
		var arm = arms[i];

		var x = arm.getStartPointX();
		var y = arm.getStartPointY();

		var grabpoint = grabpoints[i];
		var ctrlpoint = ctrlpoints[i];

		if (this.weaponVisible)
		{
			transform.multiplyPoint(grabpoint);

			ctrlpoint[0] = x + (grabpoint[0] - x) * 0.5 + ctrlpoint[0];
			ctrlpoint[1] = y + (grabpoint[1] - y) * 0.5 + ctrlpoint[1];
		}

		arm.setControlPoint(ctrlpoint);
		arm.setEndPoint(grabpoint);
	}
}

Soldier.prototype.updateLegs = function(time)
{
	var L = 110;

	var legs = [
		this.root.get("#leg-right")[0],
		this.root.get("#leg-left")[0]
	];

	if (!this.running)
	{
		for (var i = 0; i < 2; i++)
		{
			var leg = legs[i];

			var x = leg.getStartPointX();
			var y = leg.getStartPointY();

			leg.setControlPoint(x + 5, y + L / 4);
			leg.setEndPoint(x, y + L);
		}

		return;
	}

	var keyframes = [
		{ end: [70, L],         ctrl: [30, L / 2]},
		{ end: [0, L * 0.8],    ctrl: [70, L / 2]},
		{ end: [-70, L],        ctrl: [-30, L / 2]},
		{ end: [-100, L * 0.4], ctrl: [-30, L / 2]},
		{ end: [50, L * 0.7],    ctrl: [90, L * 0.3]}
	];

	var t = time / 0.2;
	var n = keyframes.length;

	for (var i = 0; i < 2; i++)
	{
		t += i * n / 2;

		var leg = legs[i];

		var x = leg.getStartPointX();
		var y = leg.getStartPointY();

		var a = Math.floor(t) % n;
		var b = Math.ceil(t) % n;
		var p = t - Math.floor(t);

		var fa = keyframes[a];
		var fb = keyframes[b];

		var endx = lerp(fa.end[0], fb.end[0], p);
		var endy = lerp(fa.end[1], fb.end[1], p);

		var ctrlx = lerp(fa.ctrl[0], fb.ctrl[0], p);
		var ctrly = lerp(fa.ctrl[1], fb.ctrl[1], p);

		leg.setControlPoint(x + ctrlx, y + ctrly);
		leg.setEndPoint(x + endx, y + endy);

		if (a < 2)
			this.root.setY(L - endy);
	}
}

Soldier.prototype.updateBody = function()
{
	var offsetTop = this.running ? 8 : 0;
	var offsetBottom = this.running ? -12 : 0;

	var points = [
		[-108 + offsetTop, -80],
		[ 108 + offsetTop, -80],
		[ 108 + offsetBottom, 66],
		[-108 + offsetBottom, 66]
	];

	this.root.get("#body")[0].setPoints(points);
	this.root.get("#head")[0].setX(6 + offsetTop);
	this.root.get("#leg-right")[0].setX(offsetBottom);
	this.root.get("#leg-left")[0].setX(offsetBottom);
}

Soldier.prototype.setTarget = function(x, y)
{
	this.target.x = x;
	this.target.y = y;

	var position = this.root.getAbsolutePosition();

	var x = x - position.x;
	var y = y - position.y;

	this.root.setScaleX(x >= 0 ? 1 : -1);

	this.update();
	this.redraw();
}

Soldier.prototype.getTargetAngle = function()
{
	var position = this.root.getAbsolutePosition();

	var x = this.target.x - position.x;
	var y = this.target.y - position.y;

	var angle = to_deg(Math.atan2(y, x));

	if (angle < -90)
		angle = -180 - angle;
	else if (angle > 90)
		angle =  180 - angle;

	return angle;
}

Soldier.prototype.addBody = function()
{
	var points = [
		[-108, -80],
		[ 108, -80],
		[ 108,  66],
		[-108,  66]
	];

	var polygon = new Kinetic.Polygon({ id: "body", points: points });

	polygon.setStroke(this.strokeColor);
	polygon.setStrokeWidth(this.strokeWidth);
	polygon.setFill(this.bodyColor);

	this.root.add(polygon);
}

Soldier.prototype.addLegs = function()
{
	var x = 100;
	var y = 66;
	var L = 110;

	this.root.add(new Kinetic.Curve({
		id: "leg-right",
		startPoint: [x, y],
		controlPoint: [x + 5, y + L / 4],
		endPoint: [x, y + L],
		stroke: this.strokeColor,
		strokeWidth: this.strokeWidth,
		lineCap: "round"
	}));

	this.root.add(new Kinetic.Curve({
		id: "leg-left",
		startPoint: [-x, y],
		controlPoint: [-x + 5, y + L / 4],
		endPoint: [-x, y + L],
		stroke: this.strokeColor,
		strokeWidth: this.strokeWidth,
		lineCap: "round"
	}));
}

Soldier.prototype.addHead = function()
{
	var head = new Kinetic.Group({ id: "head" });

	var ellipse = new Kinetic.Ellipse({
		x: 6,
		y: -180,
		radius: [174, 174 * 0.85]
	});

	ellipse.setStroke(this.strokeColor);
	ellipse.setStrokeWidth(this.strokeWidth);
	ellipse.setFill(this.headColor);

	var eyes = new Kinetic.Group({ id: "eyes" });

	eyes.add(new Kinetic.Circle({ radius: 7, fill: "black" }));
	eyes.add(new Kinetic.Circle({ x: -74, y: 12, radius: 7, fill: "black" }));

	head.add(ellipse);
	head.add(eyes);

	this.root.add(head);
}

Soldier.prototype.addWeapon = function()
{
	this.root.add(new Kinetic.Shape({
		id: "dummy-weapon",
		image: null,
		visible: false
	}));

	this.root.add(new Kinetic.Image({
		id: "weapon",
		image: this.generateWeaponImage()
	}));
}

Soldier.prototype.addArms = function()
{
	var x = 108;
	var y = -30;

	var back = new Kinetic.Curve({
		id: "arm-back",
		startPoint: [x, y],
		stroke: this.strokeColor,
		strokeWidth: this.strokeWidth,
		lineCap: "round"
	});

	var front = new Kinetic.Curve({
		id: "arm-front",
		startPoint: [-x, y],
		stroke: this.strokeColor,
		strokeWidth: this.strokeWidth
	});

	this.root.add(back);
	this.root.add(front);

	back.moveDown();
}

Soldier.prototype.generateWeaponImage = function()
{
	var source = document.getElementById("weapon");
	var dest = document.createElement("canvas");
	var context = dest.getContext("2d");

	var w = source.width;
	var h = source.height;

	dest.width = w;
	dest.height = h;

	context.drawImage(source, 0, 0);

	var imageData = context.getImageData(0, 0, w, h);
	var data = imageData.data;

	var color = Kinetic.Util.getRGB(this.weaponColor);
	var r = color.r / 255;
	var g = color.g / 255;
	var b = color.b / 255;

	for (var i = 0; i < w * h * 4; i += 4)
	{
		data[i + 0] *= r;
		data[i + 1] *= g;
		data[i + 2] *= b;
	}

	context.putImageData(imageData, 0, 0);

	return dest;
}

// -----------------------------------------------------------------------------
// Curve
// -----------------------------------------------------------------------------

Kinetic.Curve = function(config)
{
	this.___init(config);
}

Kinetic.Curve.prototype.___init = function(config)
{
	Kinetic.Shape.call(this, config);
	this.className = 'Curve';
}

Kinetic.Curve.prototype.drawFunc = function(canvas)
{
	var context = canvas.getContext();

	var x0 = this.getStartPointX();
	var y0 = this.getStartPointY();

	var xc = this.getControlPointX();
	var yc = this.getControlPointY();

	var x1 = this.getEndPointX();
	var y1 = this.getEndPointY();

	context.beginPath();
	context.moveTo(x0, y0);
	context.quadraticCurveTo(xc, yc, x1, y1);

	canvas.stroke(this);
}

Kinetic.Util.extend(Kinetic.Curve, Kinetic.Shape);
Kinetic.Node.addPointGetterSetter(Kinetic.Curve, "startPoint", 0);
Kinetic.Node.addPointGetterSetter(Kinetic.Curve, "controlPoint", 0);
Kinetic.Node.addPointGetterSetter(Kinetic.Curve, "endPoint", 0);

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

function rand(min, max)
{
	return Math.floor(Math.random() * (max - min + 1)) + min;
}

Kinetic.Util.addMethods(Kinetic.Transform, {
	multiplyPoint: function(point)
	{
		var x = point[0];
		var y = point[1];
		var m = this.m;

		point[0] = m[0] * x + m[2] * y + m[4];
		point[1] = m[1] * x + m[3] * y + m[5];

		return point;
	}
});
