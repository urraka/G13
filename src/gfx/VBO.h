#pragma once

#include "gl.h"
#include "enums.h"
#include "attributes.h"
#include <stddef.h>

namespace gfx {

class Shader;
class IBO;

class VBO
{
public:
	VBO();
	~VBO();

	template<typename T> void allocate(size_t size, Usage usage = Dynamic);
	template<typename T> void set(T *vertices, size_t offset, size_t count);

	void mode(DrawMode mode);
	void shader(Shader *shader);
	void ibo(IBO *ibo);

	GLuint   id    () const;
	DrawMode mode  () const;
	Shader  *shader() const;
	IBO     *ibo   () const;
	size_t   size  () const;

private:
	GLuint   id_;
	DrawMode mode_;
	size_t   size_;
	Shader  *shader_;
	IBO     *ibo_;

	const Attributes *attributes_;

	friend void draw(VBO *vbo, size_t offset, size_t count);
};

} // gfx
