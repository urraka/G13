#pragma once

namespace shaders
{

const char *vertex =

	#if defined(IOS)
		"#version 100\n"
	#else
		"#version 110\n"
	#endif

	"uniform mat4 matrix;"
	"uniform mat4 projection;"

	"attribute vec2 position;"
	"attribute vec2 texCoords;"

	"varying vec2 fragTexCoords;"

	"void main()"
	"{"
	"	fragTexCoords = texCoords;"
	"	gl_Position = projection * matrix * vec4(position, 0.0, 1.0);"
	"}";

const char *fragment =

	#if defined(IOS)
		"#version 100\n"
		"precision mediump float;"
		"varying lowp vec2 fragTexCoords;"
	#else
		"#version 110\n"
		"varying vec2 fragTexCoords;"
	#endif

	"uniform sampler2D sampler;"

	"void main()"
	"{"
	"	gl_FragColor = texture2D(sampler, fragTexCoords);"
	"}";

}
