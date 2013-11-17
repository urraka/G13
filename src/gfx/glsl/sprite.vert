uniform   mat3 mvp;
attribute vec2 in_position;
attribute vec2 in_texcoords;
attribute vec4 in_color;
varying   vec2 texcoords;
varying   vec4 color;

void main()
{
	color = in_color;
	texcoords = in_texcoords;
	gl_Position = vec4(vec2(mvp * vec3(in_position, 1.0)), 0.0, 1.0);
}
