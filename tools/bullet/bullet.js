var canvas = null;
var gl = null;

// vertex -> (x, y), (u, v), (r, g, b, a)

var VERTEX_SIZE = (2 + 2 + 4) * Float32Array.BYTES_PER_ELEMENT;

var emitter = {
	max: 500,
	bullets: [],
	count: 0,
	rate: 10,
	time: 0
};

var shader = {
	id: 0,
	mvp: new Float32Array(3 * 3),
	loc: {
		mvp: -1,
		sampler: - 1,
		position: -1,
		texcoords: -1,
		color: -1
	}
};

var batch = {
	vbo: 0,
	ibo: 0,
	texture: 0,
	buffer: new ArrayBuffer((4 * VERTEX_SIZE) * emitter.max)
};

var framebuffer = {
	id: 0,
	texture: 0,
	depth: 0
};

var cache = {
	matrix: new Float32Array(3 * 2),
	quad: new Float32Array(4 * VERTEX_SIZE / Float32Array.BYTES_PER_ELEMENT)
};

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------

function main()
{
	window.requestAnimationFrame = requestAnimationFrame || mozRequestAnimationFrame;
	window.addEventListener("resize", resize);

	initialize();

	start();
}

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------

function initialize()
{
	// create context

	canvas = document.getElementsByTagName("canvas")[0];
	gl = canvas.getContext("webgl") || canvas.getContext("experimental-webgl");

	var W = window.innerWidth;
	var H = window.innerHeight;

	canvas.width = W;
	canvas.height = H;
	canvas.style.width = W + "px";
	canvas.style.height = H + "px";

	// set gl state

	gl.enable(gl.BLEND);
	gl.viewport(0, 0, canvas.width, canvas.height);

	// initialize shader

	shader.id = create_program("shader-vs", "shader-fs");
	shader.loc.mvp = gl.getUniformLocation(shader.id, "mvp");
	shader.loc.sampler = gl.getUniformLocation(shader.id, "sampler");
	shader.loc.position = gl.getAttribLocation(shader.id, "vposition");
	shader.loc.texcoords = gl.getAttribLocation(shader.id, "vtexcoords");
	shader.loc.color = gl.getAttribLocation(shader.id, "vcolor");

	ortho3(canvas.width, canvas.height, shader.mvp);

	gl.uniform1i(shader.loc.sampler, 0);
	gl.uniformMatrix3fv(shader.loc.mvp, false, shader.mvp);

	// create vbo/ibo

	var indices = new Uint16Array(6 * emitter.max);

	for (var i = 0; i < emitter.max; i++)
	{
		indices[i * 6 + 0] = (i + 1) * 4 + 0;
		indices[i * 6 + 1] = (i + 1) * 4 + 1;
		indices[i * 6 + 2] = (i + 1) * 4 + 2;
		indices[i * 6 + 3] = (i + 1) * 4 + 2;
		indices[i * 6 + 4] = (i + 1) * 4 + 3;
		indices[i * 6 + 5] = (i + 1) * 4 + 0;
	}

	batch.vbo = gl.createBuffer();
	batch.ibo = gl.createBuffer();

	gl.bindBuffer(gl.ARRAY_BUFFER, batch.vbo);
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, batch.ibo);

	gl.bufferData(gl.ARRAY_BUFFER, batch.buffer.byteLength + 4 * VERTEX_SIZE, gl.DYNAMIC_DRAW);
	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, indices, gl.STATIC_DRAW);

	gl.enableVertexAttribArray(shader.loc.position);
	gl.enableVertexAttribArray(shader.loc.texcoords);
	gl.enableVertexAttribArray(shader.loc.color);

	var szFloat = Float32Array.BYTES_PER_ELEMENT;

	gl.vertexAttribPointer(shader.loc.position, 2, gl.FLOAT, false, VERTEX_SIZE, 0);
	gl.vertexAttribPointer(shader.loc.texcoords, 2, gl.FLOAT, false, VERTEX_SIZE, 2 * szFloat);
	gl.vertexAttribPointer(shader.loc.color, 4, gl.FLOAT, false, VERTEX_SIZE, (2 + 2) * szFloat);

	// create framebuffer

	framebuffer.id = gl.createFramebuffer();
	framebuffer.texture = create_texture(canvas.width, canvas.height);
	framebuffer.depth = gl.createRenderbuffer();

	gl.bindRenderbuffer(gl.RENDERBUFFER, framebuffer.depth);
	gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, canvas.width, canvas.height);

	gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer.id);
	gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, framebuffer.texture, 0);
	gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, framebuffer.depth);

	gl.clearColor(0, 0, 0, 0);
	gl.clear(gl.COLOR_BUFFER_BIT);

	// load bullet texture

	batch.texture = load_texture("bullet");

	// create bullets

	for (var i = 0; i < emitter.max; i++)
		emitter.bullets.push(new Bullet());
}

// -----------------------------------------------------------------------------
// resize
// -----------------------------------------------------------------------------

function resize()
{
	var W = window.innerWidth;
	var H = window.innerHeight;

	canvas.width = W;
	canvas.height = H;
	canvas.style.width = W + "px";
	canvas.style.height = H + "px";

	gl.viewport(0, 0, W, H);
	gl.bindTexture(gl.TEXTURE_2D, framebuffer.texture);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, W, H, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
	gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, W, H);

	ortho3(W, H, shader.mvp);
	gl.uniformMatrix3fv(shader.loc.mvp, false, shader.mvp);
}

// -----------------------------------------------------------------------------
// start
// -----------------------------------------------------------------------------

function start()
{
	var t0 = time();
	var t1 = 0;

	function frame()
	{
		t1 = time();
		draw((t1 - t0) / 1000);
		t0 = t1;
		requestAnimationFrame(frame);
	}

	frame();
}

// -----------------------------------------------------------------------------
// draw
// -----------------------------------------------------------------------------

function draw(dt)
{
	// update emitter

	emitter.time += Math.min(dt, 1);

	while (emitter.time >= (1 / emitter.rate) && emitter.count < emitter.max)
	{
		emitter.bullets[emitter.count++].spawn();
		emitter.time -= 1 / emitter.rate;
	}

	// update bullets

	for (var i = 0; i < emitter.count; i++)
	{
		var bullet = emitter.bullets[i];

		bullet.update(dt);

		if (!bullet.active)
		{
			emitter.bullets[i] = emitter.bullets[emitter.count - 1];
			emitter.bullets[emitter.count - 1] = bullet;
			emitter.count--;
			i--;
		}
	}

	// update batch

	var offset = VERTEX_SIZE / Float32Array.BYTES_PER_ELEMENT;
	var buffer = new Float32Array(batch.buffer, 0, emitter.count * (4 * offset));

	for (var i = 0; i < emitter.count; i++)
	{
		var bullet = emitter.bullets[i];
		var base = 4 * i * offset;

		update_sprite(bullet, buffer, base);
	}

	gl.bufferSubData(gl.ARRAY_BUFFER, 4 * VERTEX_SIZE, buffer);

	// render big quad to multiply framebuffer alpha

	gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer.id);
	gl.bindTexture(gl.TEXTURE_2D, batch.texture);

	identity(cache.matrix);
	update_vertex(cache.matrix, -1, -1, 0, 0, 1, 1, 1, 0.6, cache.quad, offset * 0);
	update_vertex(cache.matrix,  1, -1, 0, 0, 1, 1, 1, 0.6, cache.quad, offset * 1);
	update_vertex(cache.matrix,  1,  1, 0, 0, 1, 1, 1, 0.6, cache.quad, offset * 2);
	update_vertex(cache.matrix, -1,  1, 0, 0, 1, 1, 1, 0.6, cache.quad, offset * 3);
	gl.bufferSubData(gl.ARRAY_BUFFER, 0, cache.quad);

	identity3(shader.mvp);
	gl.blendFuncSeparate(gl.ZERO, gl.ONE, gl.DST_COLOR, gl.ZERO);
	gl.uniformMatrix3fv(shader.loc.mvp, false, shader.mvp);
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

	// render bullets

	ortho3(canvas.width, canvas.height, shader.mvp);

	if (false)
	{
		gl.clearColor(0, 0, 0, 0);
		gl.clear(gl.COLOR_BUFFER_BIT);
	}

	// gl.blendFuncSeparate(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA, gl.ONE, gl.ZERO);
	gl.blendFunc(gl.ONE, gl.ONE);
	gl.uniformMatrix3fv(shader.loc.mvp, false, shader.mvp);
	gl.drawElements(gl.TRIANGLES, emitter.count * 6, gl.UNSIGNED_SHORT, 0);

	// render to screen

	identity(cache.matrix);
	update_vertex(cache.matrix, -1, -1, 0, 0, 1, 1, 1, 0.5, cache.quad, offset * 0);
	update_vertex(cache.matrix,  1, -1, 1, 0, 1, 1, 1, 0.5, cache.quad, offset * 1);
	update_vertex(cache.matrix,  1,  1, 1, 1, 1, 1, 1, 0.5, cache.quad, offset * 2);
	update_vertex(cache.matrix, -1,  1, 0, 1, 1, 1, 1, 0.5, cache.quad, offset * 3);
	gl.bufferSubData(gl.ARRAY_BUFFER, 0, cache.quad);

	identity3(shader.mvp);
	gl.bindFramebuffer(gl.FRAMEBUFFER, null);
	gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
	// gl.clearColor(0.0, 0.7, 0.7, 1);
	gl.clearColor(0, 0, 0, 1);
	gl.clear(gl.COLOR_BUFFER_BIT);
	gl.bindTexture(gl.TEXTURE_2D, framebuffer.texture);
	gl.uniformMatrix3fv(shader.loc.mvp, false, shader.mvp);
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
}

// -----------------------------------------------------------------------------
// vertices
// -----------------------------------------------------------------------------

function update_sprite(bullet, buffer, index)
{
	var w = 90;
	var h = 9;
	var cx = 84;
	var cy = 4;
	var sx = 0.8 * bullet.size;
	var sy = 0.35;
	var u0 = 16 / 128;
	var v0 = 11 / 32;
	var u1 = (16 + w) / 128;
	var v1 = (11 + h) / 32;

	var m = cache.matrix;

	transform(bullet.x, bullet.y, bullet.rotation, sx, sy, cx, cy, 0, 0, m);

	var offset = VERTEX_SIZE / Float32Array.BYTES_PER_ELEMENT;

	update_vertex(m, 0, 0, u0, v0, 1, 1, 1, 1, buffer, index + offset * 0);
	update_vertex(m, w, 0, u1, v0, 1, 1, 1, 1, buffer, index + offset * 1);
	update_vertex(m, w, h, u1, v1, 1, 1, 1, 1, buffer, index + offset * 2);
	update_vertex(m, 0, h, u0, v1, 1, 1, 1, 1, buffer, index + offset * 3);
}

function update_vertex(m, x, y, u, v, r, g, b, a, buffer, index)
{
	buffer[index + 0] = mulx(m, x, y);
	buffer[index + 1] = muly(m, x, y);
	buffer[index + 2] = u;
	buffer[index + 3] = v;
	buffer[index + 4] = r;
	buffer[index + 5] = g;
	buffer[index + 6] = b;
	buffer[index + 7] = a;
}

// -----------------------------------------------------------------------------
// Bullet
// -----------------------------------------------------------------------------

function Bullet()
{
	this.x = 0;
	this.y = 0;
	this.vx = 0;
	this.vy = 0;
	this.rotation = 0;
	this.size = 0;
	this.active = false;
}

Bullet.prototype.update = function(dt)
{
	this.vy += 1470 * dt;
	this.x += this.vx * dt;
	this.y += this.vy * dt;
	this.rotation = Math.atan2(this.vy, this.vx);

	if (distance(this.x, this.y, 100, 100) >= 200)
		this.size = 120 / 90;

	// this.size = clamp(distance(this.x, this.y, 100, 100) - 90, 0, 120) / 90;

	if (this.x > canvas.width + 200 || this.y > canvas.height + 200)
		this.active = false;
}

Bullet.prototype.spawn = function()
{
	this.x = 100;
	this.y = 100;
	this.vx = 1500;
	this.vy = rand(-200, 200);
	this.rotation = 0;
	this.size = 0;
	this.active = true;
}

// -----------------------------------------------------------------------------
// GL helpers
// -----------------------------------------------------------------------------

function create_program(vs, fs)
{
	var program = gl.createProgram();

	gl.attachShader(program, load_shader(vs, gl.VERTEX_SHADER));
	gl.attachShader(program, load_shader(fs, gl.FRAGMENT_SHADER));
	gl.linkProgram(program);

	if (!gl.getProgramParameter(program, gl.LINK_STATUS))
		console.log("Error linking shader program.");

	gl.useProgram(program);

	return program;
}

function load_shader(id, type)
{
	var script = document.getElementById(id);
	var source = "";

	var node = script.firstChild;

	while (node)
	{
		if (node.nodeType === node.TEXT_NODE)
			source += node.textContent;

		node = node.nextSibling;
	}

	var shader = gl.createShader(type);

	gl.shaderSource(shader, source);
	gl.compileShader(shader);

	if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS))
		console.log("Shader error: " + gl.getShaderInfoLog(shader));

	return shader;
}

function load_texture(id)
{
	var image = document.getElementById(id);
	var texture = gl.createTexture();

	gl.bindTexture(gl.TEXTURE_2D, texture);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);

	return texture;
}

function create_texture(width, height)
{
	var texture = gl.createTexture();

	gl.bindTexture(gl.TEXTURE_2D, texture);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);

	return texture;
}

// -----------------------------------------------------------------------------
// matrix
// -----------------------------------------------------------------------------

function ortho3(w, h, m)
{
	m[0] = 2 / w;  m[3] = 0;       m[6] = -1;
	m[1] = 0;      m[4] = 2 / -h;  m[7] =  1;
	m[2] = 0;      m[5] = 0;       m[8] =  1;
}

function identity3(m)
{
	m[0] = 1; m[3] = 0; m[6] = 0;
	m[1] = 0; m[4] = 1; m[7] = 0;
	m[2] = 0; m[5] = 0; m[8] = 1;
}

function identity(m)
{
	m[0] = 1; m[2] = 0; m[4] = 0;
	m[1] = 0; m[3] = 1; m[5] = 0;
}

function transform(x, y, rotation, sx, sy, ox, oy, kx, ky, m)
{
	var c = Math.cos(rotation);
	var s = Math.sin(rotation);

	m[0] = c * sx - kx * s * sy;
	m[1] = s * sx + c * kx * sy;
	m[2] = c * ky * sx - s * sy;
	m[3] = ky * s * sx + c * sy;
	m[4] = x - ox * m[0] - oy * m[2];
	m[5] = y - ox * m[1] - oy * m[3];
}

function mulx(m, x, y)
{
	return m[0] * x + m[2] * y + m[4];
}

function muly(m, x, y)
{
	return m[1] * x + m[3] * y + m[5];
}

// -----------------------------------------------------------------------------
// general helpers
// -----------------------------------------------------------------------------

function rand(min, max)
{
	return Math.floor(Math.random() * (max - min) + min);
}

function time()
{
	return new Date().getTime();
}

function sqr(x)
{
	return x * x;
}

function distance(ax, ay, bx, by)
{
	return Math.sqrt(sqr(bx - ax) + sqr(by - ay));
}

function clamp(x, min, max)
{
	return Math.min(max, Math.max(min, x));
}
