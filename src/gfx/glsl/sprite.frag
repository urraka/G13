varying lowp vec2 texcoords;
varying lowp vec4 color;
uniform sampler2D sampler;

void main()
{
	gl_FragColor = texture2D(sampler, texcoords) * color;
}
