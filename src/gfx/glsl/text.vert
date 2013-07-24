uniform   mat4 mvp;
uniform   vec2 texsize;
attribute vec2 in_position;
attribute vec2 in_texcoords;
varying   vec2 texcoords;

void main()
{
	texcoords = in_texcoords / texsize;
	gl_Position = mvp * vec4(in_position, 0.0, 1.0);
}
