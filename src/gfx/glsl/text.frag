varying lowp  vec2 texcoords;
varying lowp float shift;
uniform       vec2 texsize;
uniform  sampler2D sampler;
uniform       vec4 color;

void main()
{
	vec4 curr = texture2D(sampler, texcoords);
	vec4 prev = texture2D(sampler, vec2(texcoords.x - 1.0 / texsize.x, texcoords.y));

	float r = curr.r;
	float g = curr.g;
	float b = curr.b;

	if (shift <= 1.0 / 3.0)
	{
		float z = 3.0 * shift;
		r = mix(curr.r, prev.b, z);
		g = mix(curr.g, curr.r, z);
		b = mix(curr.b, curr.g, z);
	}
	else if (shift <= 2.0 / 3.0)
	{
		float z = 3.0 * shift - 1.0;
		r = mix(prev.b, prev.g, z);
		g = mix(curr.r, prev.b, z);
		b = mix(curr.g, curr.r, z);
	}
	else if (shift < 1.0)
	{
		float z = 3.0 * shift - 2.0;
		r = mix(prev.g, prev.r, z);
		g = mix(prev.b, prev.g, z);
		b = mix(curr.r, prev.b, z);
	}

	// gl_FragColor = vec4(r, g, b, 1.0);

	vec4 result = vec4(color.rgb, (r + g + b) / 3.0);

	float mn = min(min(r, g), b);
	float mx = max(max(r, g), b);

	result = mx * result + (1.0 - mx) * vec4(r, g, b, mn);
	result.a *= color.a;

	gl_FragColor = result;
}
