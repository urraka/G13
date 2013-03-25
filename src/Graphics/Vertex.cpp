#include <pch.h>
#include <Graphics/OpenGL.h>
#include <Graphics/Vertex.h>

// -----------------------------------------------------------------------------
// VertexAttribute

VertexAttribute::VertexAttribute(const char *_name, GLint _size, GLenum _type, GLboolean _normalized, GLsizei _stride, GLvoid *_pointer)
	:	name(_name),
		size(_size),
		type(_type),
		normalized(_normalized),
		stride(_stride),
		pointer(_pointer)
{
}

// -----------------------------------------------------------------------------
// Vertex

const VertexAttribute &Vertex::attrib(int index)
{
	static VertexAttribute attributes[AttributesCount] = {
		VertexAttribute("position", 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position)),
		VertexAttribute("texCoords", 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv))
	};

	return attributes[index];
}
