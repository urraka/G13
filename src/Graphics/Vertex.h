#pragma once

#if !defined(VBO_TEMPLATE_INSTANCE)
	#define VBO_TEMPLATE_INSTANCE(VertexT)
#endif

#define VBO_TEMPLATE_INSTANCES() \
	VBO_TEMPLATE_INSTANCE(ColorVertex); \
	VBO_TEMPLATE_INSTANCE(TextureVertex); \
	VBO_TEMPLATE_INSTANCE(MixedVertex);

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

struct ColorVertex
{
	vec2 position;
	u8vec4 color;

	enum { AttributesCount = 2 };
	static const VertexAttribute &attrib(int index);
};

struct TextureVertex
{
	vec2 position;
	vec2 uv;

	enum { AttributesCount = 2 };
	static const VertexAttribute &attrib(int index);
};

struct MixedVertex
{
	vec2 position;
	vec2 uv;
	u8vec4 color;

	enum { AttributesCount = 3 };
	static const VertexAttribute &attrib(int index);
};
