#include "gfx.h"
#include "Context.h"
#include "glsl/glsl.h"

#include <glm/glm.hpp>
#include <assert.h>
#include <stdint.h>

namespace gfx {

static Context ctx_;
Context *const context = &ctx_;

// -----------------------------------------------------------------------------
// General
// -----------------------------------------------------------------------------

void initialize()
{
	#ifndef GLES2
		if (glGenerateMipmap != 0)
			glGenerateMipmapEXT = glGenerateMipmap;
	#endif

	FT_Init_FreeType(&context->freetype);

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &context->maxTextureSize);

	bgcolor(Color(0, 0, 0));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	context->shdrcolor = new Shader();
	context->shdrcolor->compile<ColorVertex>(glsl::color_vert, glsl::color_frag);

	context->shdrsprite = new Shader();
	context->shdrsprite->compile<SpriteVertex>(glsl::sprite_vert, glsl::sprite_frag);

	context->shdrtext = new Shader();
	context->shdrtext->compile<TextVertex>(glsl::text_vert, glsl::text_frag);

	context->shdrtext_color = context->shdrtext->location("color");
	context->shdrtext_texsize = context->shdrtext->location("texsize");

	context->vbosprite = new VBO();
	context->vbosprite->allocate<SpriteVertex>(4, Dynamic);
	context->vbosprite->mode(TriangleFan);
}

void terminate()
{
	if (context->freetype)
		FT_Done_FreeType(context->freetype);

	delete context->vbosprite;
	delete context->shdrcolor;
	delete context->shdrsprite;
	delete context->shdrtext;
}

void viewport(int width, int height, int rotation)
{
	assert(rotation == 0 || rotation == 90 || rotation == -90 || rotation == 180);

	context->projection = mat2d::ortho(0.0f, (float)width, (float)height, 0.0f);

	if (rotation == 90 || rotation == -90)
		glViewport(0, 0, height, width);
	else
		glViewport(0, 0, width, height);

	if (rotation != 0)
		context->projection = mat2d::rotate(glm::radians((float)rotation)) * context->projection;

	context->mvpModified = true;
}

void clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void bgcolor(Color color)
{
	glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
}

void wireframe(bool enable)
{
	#if !defined(GLES2)
		glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
	#endif
}

// -----------------------------------------------------------------------------
// Bind
// -----------------------------------------------------------------------------

void bind(Shader *shader)
{
	assert(shader == 0 || shader->program() != 0);

	if (shader != context->shader)
	{
		glUseProgram(shader != 0 ? shader->program() : 0);
		context->shader = shader;
	}
}

void bind(Texture *texture, int unit)
{
	assert(texture == 0 || texture->id() != 0);
	assert(unit < (int)(sizeof context->texture / sizeof context->texture[0]));

	if (texture != context->texture[unit])
	{
		if (unit != context->unit)
			glActiveTexture(GL_TEXTURE0 + unit);

		glBindTexture(GL_TEXTURE_2D, texture != 0 ? texture->id() : 0);

		context->unit = unit;
		context->texture[unit] = texture;
	}
}

void bind(VBO *vbo)
{
	assert(vbo == 0 || vbo->id() != 0);

	if (vbo != context->vbo)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo != 0 ? vbo->id() : 0);
		context->vbo = vbo;
	}
}

void bind(IBO *ibo)
{
	assert(ibo == 0 || ibo->id() != 0);

	if (ibo != context->ibo)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo != 0 ? ibo->id() : 0);
		context->ibo = ibo;
	}
}

// -----------------------------------------------------------------------------
// Draw
// -----------------------------------------------------------------------------

void draw(VBO *vbo)
{
	assert(vbo != 0);
	IBO *ibo = vbo->ibo();
	draw(vbo, 0, ibo != 0 ? ibo->size() : vbo->size());
}

void draw(VBO *vbo, size_t count)
{
	assert(vbo != 0);
	draw(vbo, 0, count);
}

void draw(VBO *vbo, size_t offset, size_t count)
{
	IBO *ibo = vbo->ibo();
	Shader *shader = vbo->shader();

	assert(vbo != 0);
	assert(shader != 0);
	assert(offset + count <= (ibo != 0 ? ibo->size() : vbo->size()));
	assert(shader->attributes_ == vbo->attributes_);

	bind(shader);

	if (context->mvpModified)
	{
		for (size_t i = 0; i < context->shaders.size(); i++)
			context->shaders[i]->mvpModified_ = true;

		context->mvpModified = false;
	}

	if (shader->mvpModified_ && shader->mvp_ != -1)
	{
		shader->uniform(shader->mvp_, context->projection * context->matrix);
		shader->mvpModified_ = false;
	}

	if (context->pvbo != vbo)
	{
		bind(vbo);

		const Attributes &attribs = *(vbo->attributes_);

		int nAttr = (int)attribs.size();

		for (int i = context->attr; i < nAttr; i++)
			glEnableVertexAttribArray(i);

		for (int i = nAttr; i < context->attr; i++)
			glDisableVertexAttribArray(i);

		for (int i = 0; i < nAttr; i++)
		{
			const Attribute &attr = attribs[i];
			glVertexAttribPointer(i, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
		}

		context->pvbo = vbo;
		context->attr = nAttr;
	}

	if (ibo != 0)
	{
		bind(ibo);
		glDrawElements(vbo->mode(), count, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(uint16_t) * offset));
	}
	else
	{
		glDrawArrays(vbo->mode(), offset, count);
	}
}

void draw(SpriteBatch *spriteBatch)
{
	spriteBatch->draw(0, spriteBatch->size());
}

void draw(SpriteBatch *spriteBatch, size_t count)
{
	spriteBatch->draw(0, count);
}

void draw(SpriteBatch *spriteBatch, size_t offset, size_t count)
{
	spriteBatch->draw(offset, count);
}

void draw(const Sprite &sprite)
{
	assert(sprite.texture != 0);

	SpriteVertex v[4];
	sprite.vertices(v);
	context->vbosprite->set(v, 0, 4);

	bind(sprite.texture);
	draw(context->vbosprite, 0, 4);
}

void draw(Text *text)
{
	text->draw();
}

// -----------------------------------------------------------------------------
// Matrix
// -----------------------------------------------------------------------------

void matrix(const mat2d &m)
{
	context->matrix = m;
	context->mvpModified = true;
}

const mat2d &matrix()
{
	return context->matrix;
}

void translate(float x, float y)
{
	matrix(context->matrix * mat2d::translate(x, y));
}

void rotate(float angle)
{
	matrix(context->matrix * mat2d::rotate(angle));
}

void scale(float x, float y)
{
	matrix(context->matrix * mat2d::scale(x, y));
}

void transform(const mat2d &m)
{
	matrix(context->matrix * m);
}

} // gfx
