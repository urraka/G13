uniform   mat3 mvp;
attribute vec2 in_position;
attribute vec4 in_color;
varying   vec4 color;

void main()
{
	color = in_color;
	gl_Position = vec4(vec2(mvp * vec3(in_position, 1.0)), 0.0, 1.0);
}
