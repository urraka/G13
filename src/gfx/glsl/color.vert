uniform   mat3 mvp;
attribute vec4 in_position;
attribute vec4 in_color;
varying   vec4 color;

void main()
{
	color = in_color;
	gl_Position.xyw = mvp * in_position.xyw;
	gl_Position.z = 0;
}
