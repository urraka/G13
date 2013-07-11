#pragma once

#include <vector>
#include <glm/glm.hpp>

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
			projection(1.0f)
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
};

extern Context *const context;

} // gfx
