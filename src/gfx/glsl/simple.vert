uniform   mat3 mvp;
attribute vec4 in_position;

void main()
{
	gl_Position.xyw = mvp * in_position.xyw;
	gl_Position.z = 0;
}
