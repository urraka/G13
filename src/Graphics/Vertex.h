#pragma once

#if !defined(VBO_TEMPLATE_INSTANCE)
	#define VBO_TEMPLATE_INSTANCE(VertexT)
#endif

#define VBO_TEMPLATE_INSTANCES() \
	VBO_TEMPLATE_INSTANCE(Vertex); \
	VBO_TEMPLATE_INSTANCE(ColorVertex);

struct VertexAttribute
{
	VertexAttribute(const char *_name, GLint _size, GLenum _type, GLboolean _normalized, GLsizei _stride, GLvoid *_pointer);

	const char *name;
	GLint size;
	GLenum type;
	GLboolean normalized;
	GLsizei stride;
	const GLvoid *pointer;
};

typedef const VertexAttribute& (*AttribCallback)(int);

struct Vertex
{
	Vertex();

	vec2 position;
	vec2 uv;

	enum { AttributesCount = 2 };
	static const VertexAttribute &attrib(int index);
};

struct ColorVertex
{
	ColorVertex();

	vec2 position;
	vec2 uv;
	vec4 color;

	enum { AttributesCount = 3 };
	static const VertexAttribute &attrib(int index);
};
