#pragma once

#include "gl.h"
#include <vector>

namespace gfx {

struct Attribute
{
	const char   *name;
	GLint         size;
	GLenum        type;
	GLboolean     normalized;
	GLsizei       stride;
	const GLvoid *pointer;
};

typedef std::vector<Attribute> Attributes;

} // gfx
