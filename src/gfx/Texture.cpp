#include "Texture.h"
#include "Image.h"
#include "Context.h"
#include "gfx.h"

namespace gfx {

Texture::Texture(const char *filename)
	:	id_(0),
		width_(0),
		height_(0)
{
	Image image(filename);
	init(&image);
}

Texture::Texture(Image *image)
	:	id_(0),
		width_(0),
		height_(0)
{
	init(image);
}

Texture::~Texture()
{
	if (id_ != 0)
		glDeleteTextures(1, &id_);
}

void Texture::filter(uint8_t filter, TexFilterType type)
{
	gfx::bind(this);

	GLint value = 0;

	if (type & MinFilter)
	{
		switch (filter)
		{
			case Nearest                : value = GL_NEAREST;                break;
			case Linear                 : value = GL_LINEAR;                 break;
			case Nearest | NearestMipmap: value = GL_NEAREST_MIPMAP_NEAREST; break;
			case Linear  | NearestMipmap: value = GL_LINEAR_MIPMAP_NEAREST;  break;
			case Nearest | LinearMipmap : value = GL_NEAREST_MIPMAP_LINEAR;  break;
			case Linear  | LinearMipmap : value = GL_LINEAR_MIPMAP_LINEAR;   break;
			default                     : assert(false);                     break;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value);
	}

	if (type & MagFilter)
	{
		value = (filter & Nearest ? GL_NEAREST : Linear);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, value);
	}
}

void Texture::wrap(TexWrap wrap, TexWrapAxis axis)
{
	gfx::bind(this);

	GLint value = (wrap == Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);

	if (axis & WrapX)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, value);

	if (axis & WrapY)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, value);
}

GLuint Texture::id() const
{
	return id_;
}

int Texture::width() const
{
	return width_;
}

int Texture::height() const
{
	return height_;
}

void Texture::init(Image *image)
{
	assert(image->data() != 0);

	width_ = image->width();
	height_ = image->height();

	uint8_t *data = image->data();
	GLenum format = image->format() == RGB ? GL_RGB : GL_RGBA;

	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	#ifndef GLES2
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	#endif

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format, GL_UNSIGNED_BYTE, data);

	#ifdef GLES2
		glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		glGenerateMipmap(GL_TEXTURE_2D);
	#endif

	context->texture[context->unit] = this;
}

} // gfx
