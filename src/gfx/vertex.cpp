#define VERTEX_INSTANCE(T) \
	template const Attributes *attributes<T>(); \
	template Shader *default_shader<T>();

#include "vertex.h"
#include "attributes.h"
#include "gfx.h"

namespace gfx {

namespace {
static Attribute attr(const char *name, GLint sz, GLenum type, GLboolean norm, GLsizei stride, GLvoid *ptr);
static Attributes color_attr();
static Attributes sprite_attr();
}

// -----------------------------------------------------------------------------
// attributes<T>()
// -----------------------------------------------------------------------------

template<> const Attributes *attributes<ColorVertex>()
{
	static Attributes attribs = color_attr();
	return &attribs;
}

template<> const Attributes *attributes<SpriteVertex>()
{
	static Attributes attribs = sprite_attr();
	return &attribs;
}

// -----------------------------------------------------------------------------
// default_shader<T>()
// -----------------------------------------------------------------------------

template<> Shader *default_shader<ColorVertex>()
{
	return ColorShader;
}

template<> Shader *default_shader<SpriteVertex>()
{
	return SpriteShader;
}

VERTEX_INSTANCES();

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

namespace {

Attribute attr(const char *name, GLint sz, GLenum type, GLboolean norm, GLsizei stride, GLvoid *ptr)
{
	Attribute attribute;

	attribute.name       = name;
	attribute.size       = sz;
	attribute.type       = type;
	attribute.normalized = norm;
	attribute.stride     = stride;
	attribute.pointer    = ptr;

	return attribute;
}

Attributes color_attr()
{
	Attributes a(2);

	GLsizei sz = sizeof(ColorVertex);

	a[0] = attr("in_position", 2, GL_FLOAT, GL_FALSE, sz, (GLvoid*)offsetof(ColorVertex, x));
	a[1] = attr("in_color", 4, GL_UNSIGNED_BYTE, GL_TRUE, sz, (GLvoid*)offsetof(ColorVertex, r));

	return a;
}

Attributes sprite_attr()
{
	Attributes a(3);

	GLsizei sz = sizeof(SpriteVertex);

	a[0] = attr("in_position", 2, GL_FLOAT, GL_FALSE, sz, (GLvoid*)offsetof(SpriteVertex, x));
	a[1] = attr("in_texcoords", 2, GL_FLOAT, GL_FALSE, sz, (GLvoid*)offsetof(SpriteVertex, u));
	a[2] = attr("in_opacity", 1, GL_UNSIGNED_BYTE, GL_TRUE, sz, (GLvoid*)offsetof(SpriteVertex, opacity));

	return a;
}

} // unnamed

} // gfx
