uniform   mat3 mvp;
attribute vec4 in_position;
attribute vec2 in_texcoords;
attribute vec4 in_color;
varying   vec2 texcoords;
varying   vec4 color;

void main()
{
	color = in_color;
	texcoords = in_texcoords;

	gl_Position.xyw = mvp * in_position.xyw;
	gl_Position.z = 0;
}
