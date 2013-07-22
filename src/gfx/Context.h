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
		:	shader(0),
			texture(),
			pvbo(0),
			vbo(0),
			ibo(0),
			attr(0),
			unit(0),
			matrix(1.0f),
			projection(1.0f),
			freetype(0),
			maxTextureWidth(0),
			maxTextureHeight(0)
	{}

	Shader  *shader;
	Texture *texture[2];
	VBO     *pvbo;
	VBO     *vbo;
	IBO     *ibo;
	int      attr;
	int      unit;

	std::vector<Shader*> shaders;

	glm::mat4 matrix;
	glm::mat4 projection;

	FT_Library freetype;

	int maxTextureWidth;
	int maxTextureHeight;
};

extern Context *const context;

} // gfx
