#define VERTEX_INSTANCE(T) \
	template void VBO::allocate<T>(size_t size, Usage usage); \
	template void VBO::set<T>(T *vertices, size_t offset, size_t count);

#include "VBO.h"
#include "Context.h"
#include "gfx.h"
#include <assert.h>

namespace gfx {

VBO::VBO()
	:	id_(0),
		mode_(Triangles),
		size_(0),
		shader_(0),
		ibo_(0),
		attributes_(0)
{
}

VBO::VBO(IBO *ibo)
	:	id_(0),
		mode_(Triangles),
		size_(0),
		shader_(0),
		ibo_(ibo),
		attributes_(0)
{
}

VBO::~VBO()
{
	if (context->vbo == this)
		gfx::bind((VBO*)0);

	if (context->pvbo == this)
		context->pvbo = 0;

	if (id_ != 0)
		glDeleteBuffers(1, &id_);
}

template<typename T> void VBO::allocate(size_t size, Usage usage)
{
	assert(ibo_ == 0 || size <= 0x10000);

	size_ = size;

	if (attributes_ != gfx::attributes<T>())
	{
		attributes_ = gfx::attributes<T>();

		if (shader_ == 0 || shader_->attributes_ != attributes_)
			shader_ = gfx::default_shader<T>();

		if (context->pvbo == this)
			context->pvbo = 0;
	}

	if (id_ == 0)
		glGenBuffers(1, &id_);

	glBindBuffer(GL_ARRAY_BUFFER, id_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(T) * size, 0, usage);

	context->vbo = this;
}

template<typename T> void VBO::set(T *vertices, size_t offset, size_t count)
{
	assert(offset + count <= size_);
	assert(attributes<T>() == attributes_);

	gfx::bind(this);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(T) * offset, sizeof(T) * count, vertices);
}

void VBO::mode(DrawMode mode)
{
	mode_ = mode;
}

void VBO::shader(Shader *shader)
{
	assert(attributes_ == 0 || shader->attributes_ == attributes_);

	shader_ = shader;
}

void VBO::ibo(IBO *ibo)
{
	assert(size_ <= 0x10000);
	ibo_ = ibo;
}

GLuint VBO::id() const
{
	return id_;
}

DrawMode VBO::mode() const
{
	return mode_;
}

Shader *VBO::shader() const
{
	return shader_;
}

IBO *VBO::ibo() const
{
	return ibo_;
}

size_t VBO::size() const
{
	return size_;
}

VERTEX_INSTANCES();

} // gfx
