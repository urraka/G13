#define VERTEX_INSTANCE(T) \
	template const Attributes *attributes<T>(); \
	template Shader *default_shader<T>();

#include "vertex.h"
#include "Color.h"
#include "attributes.h"
#include "Context.h"
#include "gfx.h"

namespace gfx {

static Attribute attr(const char*, GLint, GLenum, GLboolean, GLsizei, GLvoid*);

static Attributes simple_attr();
static Attributes color_attr();
static Attributes sprite_attr();
static Attributes text_attr();

// -----------------------------------------------------------------------------
// Constructors
// -----------------------------------------------------------------------------

SimpleVertex simple_vertex(float x, float y)
{
	SimpleVertex vertex;

	vertex.x = x;
	vertex.y = y;

	return vertex;
}

ColorVertex color_vertex(float x, float y, Color color)
{
	ColorVertex vertex;

	vertex.x = x;
	vertex.y = y;
	vertex.r = color.r;
	vertex.g = color.g;
	vertex.b = color.b;
	vertex.a = color.a;

	return vertex;
}

SpriteVertex sprite_vertex(float x, float y, float u, float v, Color color)
{
	SpriteVertex vertex;

	vertex.x = x;
	vertex.y = y;
	vertex.u = u;
	vertex.v = v;
	vertex.r = color.r;
	vertex.g = color.g;
	vertex.b = color.b;
	vertex.a = color.a;

	return vertex;
}

TextVertex text_vertex(float x, float y, uint16_t u, uint16_t v)
{
	TextVertex vertex;

	vertex.x = x;
	vertex.y = y;
	vertex.u = u;
	vertex.v = v;

	return vertex;
}

// -----------------------------------------------------------------------------
// attributes<T>()
// -----------------------------------------------------------------------------

template<> const Attributes *attributes<SimpleVertex>()
{
	static Attributes attribs = simple_attr();
	return &attribs;
}

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

template<> const Attributes *attributes<TextVertex>()
{
	static Attributes attribs = text_attr();
	return &attribs;
}

// -----------------------------------------------------------------------------
// default_shader<T>()
// -----------------------------------------------------------------------------

template<> Shader *default_shader<SimpleVertex>()
{
	return context->shdrsimple;
}

template<> Shader *default_shader<ColorVertex>()
{
	return context->shdrcolor;
}

template<> Shader *default_shader<SpriteVertex>()
{
	return context->shdrsprite;
}

template<> Shader *default_shader<TextVertex>()
{
	return context->shdrtext;
}

VERTEX_INSTANCES();

// -----------------------------------------------------------------------------
// Attributes definitions
// -----------------------------------------------------------------------------

#define add push_back

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

Attributes simple_attr()
{
	Attributes a;

	const GLsizei sz = sizeof(SimpleVertex);

	a.add(attr("in_position", 2, GL_FLOAT, GL_FALSE, sz, (GLvoid*)offsetof(SimpleVertex, x)));

	return a;
}

Attributes color_attr()
{
	Attributes a;

	const GLsizei sz = sizeof(ColorVertex);

	a.add(attr("in_position", 2, GL_FLOAT, GL_FALSE, sz, (GLvoid*)offsetof(ColorVertex, x)));
	a.add(attr("in_color", 4, GL_UNSIGNED_BYTE, GL_TRUE, sz, (GLvoid*)offsetof(ColorVertex, r)));

	return a;
}

Attributes sprite_attr()
{
	Attributes a;

	const GLsizei sz = sizeof(SpriteVertex);

	a.add(attr("in_position", 2, GL_FLOAT, GL_FALSE, sz, (GLvoid*)offsetof(SpriteVertex, x)));
	a.add(attr("in_texcoords", 2, GL_FLOAT, GL_FALSE, sz, (GLvoid*)offsetof(SpriteVertex, u)));
	a.add(attr("in_color", 4, GL_UNSIGNED_BYTE, GL_TRUE, sz, (GLvoid*)offsetof(SpriteVertex, r)));

	return a;
}

Attributes text_attr()
{
	Attributes a;

	const GLsizei sz = sizeof(TextVertex);

	a.add(attr("in_position", 2, GL_FLOAT, GL_FALSE, sz, (GLvoid*)offsetof(TextVertex, x)));
	a.add(attr("in_texcoords", 2, GL_UNSIGNED_SHORT, GL_FALSE, sz, (GLvoid*)offsetof(TextVertex, u)));

	return a;
}

} // gfx
