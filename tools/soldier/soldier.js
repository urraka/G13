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

	var scale = 0.5;
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

	if (code == 107)
		soldier.speed += 50;

	if (code == 109)
		soldier.speed = Math.max(40, soldier.speed - 50);
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
	this.aimTime = 0;
	this.aimStartAngle = 0;
	this.aimCurrentAngle = 0;
	this.shake = { x: 0, y: 0 };
	this.weaponVisible = true;
	this.running = false;
	this.runningTime = 0;
	this.speed = 140;

	this.root = new Kinetic.Group({ id: "soldier" });

	this.addLegs();
	this.addBody();
	this.addHead();
	this.addWeapon();
	this.addArms();

	this.sprites = this.generateSprites();

	this.initSprites();

	this.animationCount = 0;
	this.animation = new Kinetic.Animation(
		function(frame) { self.update(frame); },
		parent.getLayer()
	);

	parent.add(this.root);

	this.animation.start();
}

Soldier.prototype.generateSprites = function()
{
	var scale = 1;
	var angle = 0;
	var padding = 1;
	var blocks = [];
	var sprites = {};
	var frame = { time: 0, timeDiff: 0 };

	// set state

	this.run(false);
	this.shoot(false);
	this.aimTime = 0;

	var getTargetAngle = this.getTargetAngle;
	this.getTargetAngle = function() { return angle; };

	// arms

	var interval = 6;

	var arms = [
		this.root.get("#arm-front")[0],
		this.root.get("#arm-back")[0]
	];

	sprites[arms[0].getId()] = [];
	sprites[arms[1].getId()] = [];

	for (var i = 0; i <= 180 / interval; i++)
	{
		angle = i * interval - 90;

		this.update(frame);

		for (var j = 0; j < 2; j++)
		{
			var arm = arms[j];

			var x0 = arm.getStartPointX();
			var y0 = arm.getStartPointY();
			var x1 = arm.getEndPointX();
			var y1 = arm.getEndPointY();

			var curve = clone_node(arm, {
				offsetX: x0,
				offsetY: y0,
				rotation: -Math.atan2(y1 - y0, x1 - x0)
			});

			var sprite = node_image(curve, scale);

			blocks.push({
				w: sprite.image.width + padding,
				h: sprite.image.height + padding,
				sprite: sprite
			});

			sprites[arm.getId()].push(sprite);
		}
	}

	// body

	var body = this.root.get("#body")[0];
	var sprite = node_image(clone_node(body, { fill: "white" }), scale);

	blocks.push({
		w: sprite.image.width + padding,
		h: sprite.image.height + padding,
		sprite: sprite
	});

	sprites["body"] = sprite;

	this.run(true);
	this.update(frame);

	var sprite = node_image(clone_node(body, { fill: "white" }), scale);

	blocks.push({
		w: sprite.image.width + padding,
		h: sprite.image.height + padding,
		sprite: sprite
	});

	sprites["body-running"] = sprite;

	// head

	var head = this.root.get("#head")[0];
	var ellipse = head.get("Ellipse")[0];
	var sprite = node_image(clone_node(ellipse, { x: 0, y: 0, fill: "white" }), scale);

	blocks.push({
		w: sprite.image.width + padding,
		h: sprite.image.height + padding,
		sprite: sprite
	});

	sprites["head"] = sprite;

	// eye

	var eyes = head.get("#eyes")[0];
	var circle = eyes.get("Circle")[0];
	var sprite = node_image(clone_node(circle, { x: 0, y: 0 }), scale);

	blocks.push({
		w: sprite.image.width + padding,
		h: sprite.image.height + padding,
		sprite: sprite
	});

	sprites["eye"] = sprite;

	// legs

	var leg = this.root.get("#leg-right")[0];

	frame.time = 0;
	frame.timeDiff = 1000 / 60;

	this.speed = 140;
	this.run(true);

	sprites["leg"] = [];

	for (var i = 0; i < 60; i++)
	{
		this.update(frame);

		var x = leg.getStartPointX();
		var y = leg.getStartPointY();

		var curve = clone_node(leg, { offsetX: x, offsetY: y });
		var sprite = node_image(curve, scale);

		blocks.push({
			w: sprite.image.width + padding,
			h: sprite.image.height + padding,
			sprite: sprite
		});

		sprites["leg"].push(sprite);
	}

	this.run(false);
	this.update(frame);

	var x = leg.getStartPointX();
	var y = leg.getStartPointY();

	var curve = clone_node(leg, { offsetX: x, offsetY: y });
	var sprite = node_image(curve, scale);

	blocks.push({
		w: sprite.image.width + padding,
		h: sprite.image.height + padding,
		sprite: sprite
	});

	sprites["leg-standing"] = sprite;

	// weapon

	var weaponColor = this.weaponColor;
	this.weaponColor = "#FFFFFF";

	var image = this.generateWeaponImage();
	var offset = crop_image(image);

	var sprite = {
		image: image,
		cx: 337 - offset.x,
		cy: 45 - offset.y
	};

	blocks.push({
		w: sprite.image.width + padding,
		h: sprite.image.height + padding,
		sprite: sprite
	});

	sprites["weapon"] = sprite;

	// generate atlas

	blocks.sort(function(a, b) { return Math.max(b.w, b.h) - Math.max(a.w, a.h); });

	var packer = new GrowingPacker();
	packer.fit(blocks);

	var w = packer.root.w + padding;
	var h = packer.root.h + padding;

	var atlas = document.createElement("canvas");
	var context = atlas.getContext("2d");

	atlas.width = w;
	atlas.height = h;

	for (var i = 0; i < blocks.length; i++)
	{
		var x = blocks[i].fit.x + padding;
		var y = blocks[i].fit.y + padding;
		var image = blocks[i].sprite.image;
		var imageData = image.getContext("2d").getImageData(0, 0, image.width, image.height);

		context.putImageData(imageData, x, y, 0, 0, image.width, image.height);

		blocks[i].sprite.x = x;
		blocks[i].sprite.y = y;
		blocks[i].sprite.width = image.width;
		blocks[i].sprite.height = image.height;

		delete blocks[i].sprite.image;
	}

	console.log(atlas.toDataURL());

	// restore state

	this.getTargetAngle = getTargetAngle;
	this.update({ time: 0, timeDiff: 0 });

	return { image: atlas, info: sprites };
}

Soldier.prototype.showWeapon = function(show)
{
	this.weaponVisible = show;

	if (!show && this.shooting)
		this.shoot(false);

	this.root.get("#weapon")[0].setVisible(show);
}

Soldier.prototype.run = function(run)
{
	if (this.running == run)
		return;

	this.running = run;
	this.runningTime = 0;

	this.aimStartAngle = this.aimCurrentAngle;
	this.aimTime = 1;
}

Soldier.prototype.shoot = function(shoot)
{
	if (!this.weaponVisible)
		shoot = false;

	if (this.shooting == shoot)
		return;

	this.shooting = shoot;

	this.aimStartAngle = this.aimCurrentAngle;
	this.aimTime = 1;
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
	this.updateWeapon(frame, true);
	this.updateWeapon(frame, false);
	this.updateArms();
	this.updateLegs(frame);
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

Soldier.prototype.updateWeapon = function(frame, dummy)
{
	var distances = [400, 300, 275, 275, 250];

	var angle = 0;

	if (!this.running || this.shooting)
		angle = this.getTargetAngle();
	else
		angle = (Math.cos(this.runningTime * Math.PI * 2) - 0.5) * 2;

	if (this.aimTime > 0)
	{
		var angle0 = this.aimStartAngle;
		var percent = 1 - this.aimTime;

		angle = lerp(angle0, angle, percent);

		this.aimTime = Math.max(0, this.aimTime - frame.timeDiff / (this.shooting ? 100 : 200));
	}

	this.aimCurrentAngle = angle;

	if (dummy && this.sprites)
	{
		var interval = 180 / (this.sprites.info["arm-back"].length - 1);
		angle = Math.floor((angle + 90) / interval) * interval - 90;
	}

	var theta = 180 / (distances.length - 1);
	var a = Math.floor((angle + 90) / theta);
	var b = Math.ceil((angle + 90) / theta);
	var percent = (angle - (a * theta - 90)) / theta;
	var distance = lerp(distances[a], distances[b], percent);

	var weapon = dummy ? this.root.get("#dummy-weapon")[0] : this.root.get("#weapon")[0];

	var offsetX = 337 - distance;
	var offsetY = 45;

	if (!dummy)
	{
		offsetX += this.shake.x;
		offsetY += this.shake.y;
	}

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

		var grabpoint = [grabpoints[i][0], grabpoints[i][1]];
		var ctrlpoint = [ctrlpoints[i][0], ctrlpoints[i][1]];

		if (this.weaponVisible)
		{
			transform.multiplyPoint(grabpoint);

			ctrlpoint[0] = x + (grabpoint[0] - x) * 0.5 + ctrlpoint[0];
			ctrlpoint[1] = y + (grabpoint[1] - y) * 0.5 + ctrlpoint[1];
		}

		arm.setControlPoint(ctrlpoint);
		arm.setEndPoint(grabpoint);
	}

	if (this.sprites)
	{
		var dummy = this.root.get("#dummy-weapon")[0];
		var transform = dummy.getTransform();

		var angle = Math.round(dummy.getRotationDeg());
		var interval = 180 / (this.sprites.info["arm-back"].length - 1);
		var index = (angle + 90) / interval;

		var armSprites = [
			this.root.get("#arm-front-sprite")[0],
			this.root.get("#arm-back-sprite")[0]
		];

		var spritesInfo = [
			this.sprites.info["arm-front"][index],
			this.sprites.info["arm-back"][index]
		];

		for (var i = 0; i < 2; i++)
		{
			var arm = arms[i];
			var armSprite = armSprites[i];
			var info = spritesInfo[i];
			var grabpoint = grabpoints[i];

			transform.multiplyPoint(grabpoint);

			var x0 = arm.getStartPointX();
			var y0 = arm.getStartPointY();
			var x1 = arm.getEndPointX();
			var y1 = arm.getEndPointY();
			var x1b = grabpoint[0];
			var y1b = grabpoint[1];

			var rotation = Math.atan2(y1 - y0, x1 - x0);

			var dist1 = Math.sqrt(Math.pow(x1b - x0, 2) + Math.pow(y1b - y0, 2));
			var dist2 = Math.sqrt(Math.pow(x1  - x0, 2) + Math.pow(y1  - y0, 2));

			armSprite.setCrop({ x: info.x, y: info.y, width: info.width, height: info.height });
			armSprite.setWidth(info.width);
			armSprite.setHeight(info.height);
			armSprite.setOffset(info.cx, info.cy);
			armSprite.setRotation(rotation);
			armSprite.setScaleX(dist2 / dist1);
		}
	}
}

Soldier.prototype.updateLegs = function(frame)
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

	var n = keyframes.length;
	var t = 3 + this.runningTime * n;

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

	// 5 fps -> 140 units/s -> frameTime = 140 / (this.speed * 5) = 28 / this.speed

	var frameTime = 28 / this.speed;
	var totalTime = n * frameTime;

	this.runningTime += (frame.timeDiff / 1000) / totalTime;

	if (this.runningTime > 1)
		this.runningTime -= 1;
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
		strokeWidth: this.strokeWidth
	});

	var front = new Kinetic.Curve({
		id: "arm-front",
		startPoint: [-x, y],
		stroke: this.strokeColor,
		strokeWidth: this.strokeWidth
	});

	var backSprite = new Kinetic.Image({
		id: "arm-back-sprite",
		image: null,
		visible: false,
		x: x,
		y: y
	});

	var frontSprite = new Kinetic.Image({
		id: "arm-front-sprite",
		image: null,
		visible: false,
		x: -x,
		y: y
	});

	this.root.add(back);
	this.root.add(backSprite);
	this.root.add(front);
	this.root.add(frontSprite);

	back.moveDown();
	backSprite.moveDown();
}

Soldier.prototype.initSprites = function()
{
	var sprites = [
		this.root.get("#arm-back-sprite")[0],
		this.root.get("#arm-front-sprite")[0]
	];

	for (var i = 0; i < sprites.length; i++)
	{
		sprites[i].setImage(this.sprites.image);
		sprites[i].setVisible(true)
	}

	this.root.get("#arm-back")[0].setVisible(false);
	this.root.get("#arm-front")[0].setVisible(false);
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
// Kinetic.Curve
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
// Kinetic.Transform
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
// Kinetic getBounds
// -----------------------------------------------------------------------------

function get_points_bounds(points, transform)
{
	if (points.length === 0)
		return { x0: 0, y0: 0, x1: 0, y1: 0 };

	var transformedPoints = [];
	var len = points.length;

	for (var i = 0; i < len; i++)
	{
		var point = [points[i].x, points[i].y];

		transform.multiplyPoint(point);
		transformedPoints.push({ x: point[0], y: point[1] });
	}

	var bounds = {
		x0: transformedPoints[0].x,
		y0: transformedPoints[0].y,
		x1: transformedPoints[0].x,
		y1: transformedPoints[0].y
	};

	for (var i = 1; i < len; i++)
	{
		if (transformedPoints[i].x < bounds.x0) bounds.x0 = transformedPoints[i].x;
		if (transformedPoints[i].y < bounds.y0) bounds.y0 = transformedPoints[i].y;
		if (transformedPoints[i].x > bounds.x1) bounds.x1 = transformedPoints[i].x;
		if (transformedPoints[i].y > bounds.y1) bounds.y1 = transformedPoints[i].y;
	}

	bounds.x0 = Math.floor(bounds.x0);
	bounds.y0 = Math.floor(bounds.y0);
	bounds.x1 = Math.floor(bounds.x1);
	bounds.y1 = Math.floor(bounds.y1);

	return bounds;
}

Kinetic.Util.addMethods(Kinetic.Polygon, {
	getBounds: function()
	{
		return get_points_bounds(this.getPoints(), this.getTransform());
	}
});

Kinetic.Util.addMethods(Kinetic.Ellipse, {
	getBounds: function()
	{
		var rx = this.getRadiusX();
		var ry = this.getRadiusY();

		var points = [
			{ x: -rx, y:  ry },
			{ x:  rx, y: -ry }
		];

		return get_points_bounds(points, this.getTransform());
	}
});

Kinetic.Util.addMethods(Kinetic.Circle, {
	getBounds: function()
	{
		var r = this.getRadius();

		var points = [
			{ x: -r, y:  r },
			{ x:  r, y: -r }
		];

		return get_points_bounds(points, this.getTransform());
	}
});

Kinetic.Util.addMethods(Kinetic.Curve, {
	getBounds: function()
	{
		var points = [
			this.getStartPoint(),
			this.getEndPoint(),
			this.getControlPoint()
		];

		return get_points_bounds(points, this.getTransform());
	}
});

// -----------------------------------------------------------------------------
// Kinetic helpers
// -----------------------------------------------------------------------------

function clone_node(node, config)
{
	var id = null;

	if ("id" in node.attrs)
	{
		id = node.attrs.id;
		delete node.attrs.id;
	}

	var result = node.clone(config);

	if (id !== null)
		node.attrs.id = id;

	return result;
}

function node_image(node, scale)
{
	var bounds = node.getBounds();
	var margin = node.getStrokeWidth() + 1 || 0;

	var x = margin - bounds.x0;
	var y = margin - bounds.y0;

	node.move(x, y);

	var scene = new Kinetic.SceneCanvas({
		width: Math.ceil((bounds.x1 - bounds.x0 + 1 + 2 * margin) * scale),
		height: Math.ceil((bounds.y1 - bounds.y0 + 1 + 2 * margin) * scale),
		pixelRatio: 1
	});

	var wrapper = new Kinetic.Group({ scaleX: scale, scaleY: scale });
	wrapper.add(node);

	node.drawScene(scene);

	var image = scene.element;
	var offset = crop_image(image);

	var result = {
		image: image,
		cx: x * scale - offset.x0,
		cy: y * scale - offset.y0
	};

	node.move(-x, -y);

	return result;
}

function crop_image(image)
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
