#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

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
		matrix = glm::mat4(1.0f);
		projection = glm::mat4(1.0f);
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

	// ModelViewProjection matrix
	std::vector<Shader*> shaders;
	glm::mat4 matrix;
	glm::mat4 projection;

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
