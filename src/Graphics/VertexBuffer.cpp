#include <pch.h>
#include <Graphics/Graphics.h>

VertexBuffer::VertexBuffer(Graphics *graphics)
	:	graphics_(graphics),
		vboId_(0),
		iboId_(0),
		vboSize_(0),
		iboSize_(0)
{
}

VertexBuffer::~VertexBuffer()
{
	if (vboId_ != 0)
		glDeleteBuffers(1, &vboId_);

	if (iboId_ != 0)
		glDeleteBuffers(1, &iboId_);
}

void VertexBuffer::create(Mode mode, Usage vboUsage, Usage iboUsage, size_t vboSize, size_t iboSize)
{
	assert(iboSize == 0 || vboSize <= 0x10000);

	mode_ = mode;
	vboSize_ = vboSize;
	iboSize_ = iboSize;

	glGenBuffers(1, &vboId_);
	glBindBuffer(GL_ARRAY_BUFFER, vboId_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vboSize, 0, vboUsage);

	if (iboSize > 0)
	{
		glGenBuffers(1, &iboId_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * iboSize, 0, iboUsage);
	}
}

void VertexBuffer::mode(Mode mode)
{
	mode_ = mode;
}

VertexBuffer::Mode VertexBuffer::mode() const
{
	return mode_;
}

void VertexBuffer::bind()
{
	graphics_->buffer(this);
}

void VertexBuffer::set(Vertex *vertices, size_t offset, size_t count)
{
	assert(graphics_->buffer() == this);
	assert(offset + count <= vboSize_);

	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * offset, sizeof(Vertex) * count, vertices);
}

void VertexBuffer::set(uint16_t *indices, size_t offset, size_t count)
{
	assert(graphics_->buffer() == this);
	assert(offset + count <= iboSize_);

	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * offset, sizeof(uint16_t) * count, indices);
}

GLuint VertexBuffer::id(Type type) const
{
	return type == VBO ? vboId_ : iboId_;
}

size_t VertexBuffer::size() const
{
	return iboId_ != 0 ? iboSize_ : vboSize_;
}
