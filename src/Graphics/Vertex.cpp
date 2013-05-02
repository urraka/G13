#include "Vertex.h"

#define offs(vertex, member) (GLvoid*)((size_t)glm::value_ptr(vertex.member) - (size_t)&vertex)

// -----------------------------------------------------------------------------
// VertexAttribute
// -----------------------------------------------------------------------------

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
// ColorVertex
// -----------------------------------------------------------------------------

const VertexAttribute &ColorVertex::attrib(int index)
{
	static ColorVertex v;
	static VertexAttribute attributes[AttributesCount] = {
		VertexAttribute("position", 2, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), offs(v, position)),
		VertexAttribute("color", 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColorVertex), offs(v, color))
	};

	return attributes[index];
}

// -----------------------------------------------------------------------------
// TextureVertex
// -----------------------------------------------------------------------------

const VertexAttribute &TextureVertex::attrib(int index)
{
	static TextureVertex v;
	static VertexAttribute attributes[AttributesCount] = {
		VertexAttribute("position", 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), offs(v, position)),
		VertexAttribute("texCoords", 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), offs(v, uv))
	};

	return attributes[index];
}

// -----------------------------------------------------------------------------
// MixedVertex
// -----------------------------------------------------------------------------

const VertexAttribute &MixedVertex::attrib(int index)
{
	static MixedVertex v;
	static VertexAttribute attributes[AttributesCount] = {
		VertexAttribute("position", 2, GL_FLOAT, GL_FALSE, sizeof(MixedVertex), offs(v, position)),
		VertexAttribute("texCoords", 2, GL_FLOAT, GL_FALSE, sizeof(MixedVertex), offs(v, uv)),
		VertexAttribute("color", 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(MixedVertex), offs(v, color))
	};

	return attributes[index];
}
