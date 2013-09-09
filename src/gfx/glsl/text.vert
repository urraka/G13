uniform   mat3x2  mvp;
uniform   vec2  texsize;
attribute vec2  in_position;
attribute vec2  in_texcoords;
varying   vec2  texcoords;
varying   float shift;

void main()
{
	texcoords = in_texcoords / texsize;

	vec2 pos = vec2(floor(in_position.x), in_position.y);
	shift = fract(in_position.x);

	gl_Position = vec4(mvp * vec3(pos, 1.0), 0.0, 1.0);
}
