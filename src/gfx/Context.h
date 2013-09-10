#pragma once

#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "mat2d.h"

namespace gfx {

class Shader;
class Texture;
class VBO;
class IBO;

struct Context
{
	Context()
	{
		memset(texture, 0, sizeof(texture));

		freetype = 0;
		shader = 0;
		pvbo = 0;
		vbo = 0;
		ibo = 0;
		attr = 0;
		unit = 0;
		lineWidth = 1.0f;
		mvpModified = true;
		maxTextureSize = 0;
		shdrcolor = 0;
		shdrsprite = 0;
		shdrtext = 0;
		shdrtext_color = -1;
		shdrtext_texsize = -1;
		vbosprite = 0;
	}

	FT_Library freetype;

	// bindings
	Shader  *shader;
	Texture *texture[2];
	VBO     *pvbo;
	VBO     *vbo;
	IBO     *ibo;
	int      attr;
	int      unit;

	// state
	float lineWidth;

	// ModelViewProjection matrix
	std::vector<Shader*> shaders;
	mat2d matrix;
	mat2d projection;
	bool mvpModified;

	// constants
	int maxTextureSize;

	// default shaders
	Shader *shdrcolor;
	Shader *shdrsprite;
	Shader *shdrtext;

	// text shader locations
	GLint shdrtext_color;
	GLint shdrtext_texsize;

	// vbo for a single sprite
	VBO *vbosprite;
};

extern Context *const context;

} // gfx
