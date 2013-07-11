#include "IBO.h"
#include "Context.h"
#include "gfx.h"
#include <assert.h>

namespace gfx {

IBO::IBO() : id_(0), size_(0) {}

IBO::IBO(size_t size, Usage usage) : id_(0), size_(0)
{
	allocate(size, usage);
}

IBO::~IBO()
{
	if (context->ibo == this)
		context->ibo = 0;

	if (id_ != 0)
		glDeleteBuffers(1, &id_);
}

void IBO::allocate(size_t size, Usage usage)
{
	size_ = size;

	if (id_ == 0)
		glGenBuffers(1, &id_);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * size, 0, usage);

	context->ibo = this;
}

void IBO::set(uint16_t *indices, size_t offset, size_t count)
{
	assert(offset + count <= size_);

	gfx::bind(this);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * offset, sizeof(uint16_t) * count, indices);
}

GLuint IBO::id() const
{
	return id_;
}

size_t IBO::size() const
{
	return size_;
}

} // gfx
