#include <System/platform.h>
#include <Graphics/Graphics.h>

#define offs(obj, member) (GLvoid*)((size_t)&obj.member - (size_t)&obj)

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
	static Vertex v;
	static VertexAttribute attributes[AttributesCount] = {
		VertexAttribute("position", 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offs(v, position)),
		VertexAttribute("texCoords", 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offs(v, uv))
	};

	return attributes[index];
}

// -----------------------------------------------------------------------------
// ColorVertex

const VertexAttribute &ColorVertex::attrib(int index)
{
	static ColorVertex v;
	static VertexAttribute attributes[AttributesCount] = {
		VertexAttribute("position", 2, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), offs(v, position)),
		VertexAttribute("texCoords", 2, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), offs(v, uv)),
		VertexAttribute("color", 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColorVertex), offs(v, color))
	};

	return attributes[index];
}
