uniform   mat4 mvp;
attribute vec2 in_position;
attribute vec4 in_color;
varying   vec4 color;

void main()
{
	color = in_color;
	gl_Position = mvp * vec4(in_position, 0.0, 1.0);
}
