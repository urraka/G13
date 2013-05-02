#define VBO_TEMPLATE_INSTANCE(VertexT) \
	template class VBO<VertexT>;

#include "VBO.h"
#include "Graphics.h"

template<class VertexT> VBO<VertexT>::VBO(Graphics *graphics)
	:	graphics_(graphics),
		mode_(Triangles),
		id_(),
		size_()
{
}

template<class VertexT> VBO<VertexT>::~VBO()
{
	if (id_[Vertices] != 0)
		glDeleteBuffers(1, &id_[Vertices]);

	if (id_[Elements] != 0)
		glDeleteBuffers(1, &id_[Elements]);
}

template<class VertexT> GLuint VBO<VertexT>::id(Type type) const
{
	return id_[type];
}

template<class VertexT> size_t VBO<VertexT>::size() const
{
	return id_[Elements] != 0 ? size_[Elements] : size_[Vertices];
}

template<class VertexT> void VBO<VertexT>::mode(Mode mode)
{
	mode_ = mode;
}

template<class VertexT> vbo_t::Mode VBO<VertexT>::mode() const
{
	return mode_;
}

template<class VertexT> void VBO<VertexT>::set(uint16_t *indices, size_t offset, size_t count)
{
	assert(offset + count <= size_[Elements]);

	graphics_->bind(this, Elements);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * offset, sizeof(uint16_t) * count, indices);
}

template<class VertexT> void VBO<VertexT>::set(VertexT *vertices, size_t offset, size_t count)
{
	assert(offset + count <= size_[Vertices]);

	graphics_->bind(this, Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(VertexT) * offset, sizeof(VertexT) * count, vertices);
}

template<class VertexT> void VBO<VertexT>::create(Mode mode, Usage vboUsage, Usage iboUsage, size_t vboSize, size_t iboSize)
{
	assert(iboSize == 0 || vboSize <= 0x10000); // glDrawElements is called with GL_UNSIGNED_SHORT

	mode_ = mode;
	size_[Vertices] = vboSize;
	size_[Elements] = iboSize;

	glGenBuffers(1, &id_[Vertices]);
	glBindBuffer(GL_ARRAY_BUFFER, id_[Vertices]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexT) * vboSize, 0, vboUsage);

	if (iboSize > 0)
	{
		glGenBuffers(1, &id_[Elements]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_[Elements]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * iboSize, 0, iboUsage);
	}
}

VBO_TEMPLATE_INSTANCES();
