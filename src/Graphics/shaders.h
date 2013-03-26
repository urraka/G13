#pragma once

namespace
{

struct shader
{
	const char *vert;
	const char *frag;
};

// -----------------------------------------------------------------------------
// Texture (default)

const char *textureVert =
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
	"void main() {"
	"	fragTexCoords = texCoords;"
	"	gl_Position = projection * matrix * vec4(position, 0.0, 1.0);"
	"}";

const char *textureFrag =
	#if defined(IOS)
		"#version 100\n"
		"precision mediump float;"
		"varying lowp vec2 fragTexCoords;"
	#else
		"#version 110\n"
		"varying vec2 fragTexCoords;"
	#endif
	"uniform sampler2D sampler;"
	"void main() {"
	"	gl_FragColor = texture2D(sampler, fragTexCoords);"
	"}";

// -----------------------------------------------------------------------------
// Texture + Color

const char *colorVert =
	#if defined(IOS)
		"#version 100\n"
	#else
		"#version 110\n"
	#endif
	"uniform mat4 matrix;"
	"uniform mat4 projection;"
	"attribute vec2 position;"
	"attribute vec2 texCoords;"
	"attribute vec4 color;"
	"varying vec2 fragTexCoords;"
	"varying vec4 fragColor;"
	"void main() {"
	"	fragColor = color;"
	"	fragTexCoords = texCoords;"
	"	gl_Position = projection * matrix * vec4(position, 0.0, 1.0);"
	"}";

const char *colorFrag =
	#if defined(IOS)
		"#version 100\n"
		"precision mediump float;"
		"varying lowp vec2 fragTexCoords;"
		"varying lowp vec4 fragColor;"
	#else
		"#version 110\n"
		"varying vec2 fragTexCoords;"
		"varying vec4 fragColor;"
	#endif
	"uniform sampler2D sampler;"
	"uniform float textureEnabled;"
	"void main() {"
	"	gl_FragColor = mix(fragColor, texture2D(sampler, fragTexCoords) * fragColor, textureEnabled);"
	"}";

}

namespace shaders
{
	shader texture = { textureVert, textureFrag };
	shader color = { colorVert, colorFrag };
}
