varying lowp vec2 texcoords;
uniform sampler2D sampler;
uniform vec4      color;

void main()
{
	gl_FragColor = color;
	gl_FragColor.a *= texture2D(sampler, texcoords).a;
}
