uniform   mat4  mvp;
attribute vec2  in_position;
attribute vec2  in_texcoords;
attribute float in_opacity;
varying   vec2  texcoords;
varying   float opacity;

void main()
{
	opacity = in_opacity;
	texcoords = in_texcoords;
	gl_Position = mvp * vec4(in_position, 0.0, 1.0);
}
