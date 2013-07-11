#pragma once

#include "gl.h"
#include "enums.h"
#include <stddef.h>
#include <stdint.h>

namespace gfx {

class IBO
{
public:
	IBO();
	IBO(size_t size, Usage usage);
	~IBO();

	void allocate(size_t size, Usage usage);
	void set(uint16_t *indices, size_t offset, size_t count);

	GLuint id  () const;
	size_t size() const;

private:
	GLuint id_;
	size_t size_;
};

} // gfx
