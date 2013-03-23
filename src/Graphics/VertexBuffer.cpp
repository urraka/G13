#include <pch.h>
#include <Graphics/Graphics.h>

VertexBuffer::VertexBuffer(Graphics *graphics)
	:	graphics_(graphics),
		id_(),
		size_()
{
}

VertexBuffer::~VertexBuffer()
{
	if (id_[VBO] != 0)
		glDeleteBuffers(1, &id_[VBO]);

	if (id_[IBO] != 0)
		glDeleteBuffers(1, &id_[IBO]);
}

void VertexBuffer::create(Mode mode, Usage vboUsage, Usage iboUsage, size_t vboSize, size_t iboSize)
{
	assert(iboSize == 0 || vboSize <= 0x10000); // this is because i'm using GL_UNSIGNED_SHORT when calling glDrawElements

	mode_ = mode;
	size_[VBO] = vboSize;
	size_[IBO] = iboSize;

	glGenBuffers(1, &id_[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, id_[VBO]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vboSize, 0, vboUsage);

	if (iboSize > 0)
	{
		glGenBuffers(1, &id_[IBO]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_[IBO]);
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

void VertexBuffer::set(Vertex *vertices, size_t offset, size_t count)
{
	assert(offset + count <= size_[VBO]);

	graphics_->bind(this);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * offset, sizeof(Vertex) * count, vertices);
}

void VertexBuffer::set(uint16_t *indices, size_t offset, size_t count)
{
	assert(offset + count <= size_[IBO]);

	graphics_->bind(this);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * offset, sizeof(uint16_t) * count, indices);
}

GLuint VertexBuffer::id(Type type) const
{
	return id_[type];
}

size_t VertexBuffer::size() const
{
	return id_[IBO] != 0 ? size_[IBO] : size_[VBO];
}
