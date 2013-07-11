varying lowp vec2  texcoords;
varying lowp float opacity;
uniform sampler2D sampler;

void main()
{
	gl_FragColor = texture2D(sampler, texcoords);
	gl_FragColor.a *= opacity;
}
