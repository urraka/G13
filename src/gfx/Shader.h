#pragma once

#include "gl.h"
#include "attributes.h"
#include <glm/glm.hpp>

namespace gfx {

class VBO;

class Shader
{
public:
	Shader();
	~Shader();

	template<typename T> void compile(const char *vert, const char *frag);

	GLuint program() const;
	GLint location(const char *name);

	void uniform(GLint location, int value);
	void uniform(GLint location, float value);
	void uniform(GLint location, const glm::vec2 &value);
	void uniform(GLint location, const glm::vec4 &value);
	void uniform(GLint location, const glm::mat4 &value);
	void uniform(const char *name, int value);
	void uniform(const char *name, float value);
	void uniform(const char *name, const glm::vec2 &value);
	void uniform(const char *name, const glm::vec4 &value);
	void uniform(const char *name, const glm::mat4 &value);

private:
	GLuint program_;
	GLint  mvp_;
	bool   mvpModified_;

	const Attributes *attributes_;

	friend void draw(VBO *vbo, size_t offset, size_t count);
	friend class VBO;
};

} // gfx
