#include "Texture.h"
#include "Image.h"
#include "Context.h"
#include "gfx.h"

namespace gfx {

Texture::Texture(const char *filename, bool mipmap)
	:	id_(0),
		width_(0),
		height_(0),
		channels_(0),
		mipmap_(mipmap),
		format_(0)
{
	Image image(filename);

	width_ = image.width();
	height_ = image.height();
	channels_ = image.format() == RGB ? 3 : 4;
	format_ = format();

	create(image.data());
}

Texture::Texture(Image *image, bool mipmap)
	:	id_(0),
		width_(image->width()),
		height_(image->height()),
		channels_(image->format() == RGB ? 3 : 4),
		mipmap_(mipmap),
		format_(format())
{
	create(image->data());
}

Texture::Texture(int width, int height, int channels, bool mipmap)
	:	id_(0),
		width_(width),
		height_(height),
		channels_(channels),
		mipmap_(mipmap),
		format_(format())
{
	create(0);
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

int Texture::channels() const
{
	return channels_;
}

void Texture::update(int x, int y, int width, int height, uint8_t *data)
{
	assert(id_ != 0);

	gfx::bind(this);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, format_, GL_UNSIGNED_BYTE, data);

	if (mipmap_)
		generateMipmap();
}

void Texture::copy(uint8_t *buffer, size_t size)
{
	assert(id_ != 0);
	assert(size >= (size_t)(width_ * height_ * channels_));

	gfx::bind(this);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glGetTexImage(GL_TEXTURE_2D, 0, format_, GL_UNSIGNED_BYTE, buffer);
}

void Texture::create(uint8_t *data)
{
	assert(id_ == 0);

	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap_ ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, format_, GL_UNSIGNED_BYTE, data);

	// TODO: detect failure?

	if (mipmap_)
		generateMipmap();

	context->texture[context->unit] = this;
}

void Texture::generateMipmap()
{
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	#ifdef GLES2
		glGenerateMipmap(GL_TEXTURE_2D);
	#else
		glGenerateMipmapEXT(GL_TEXTURE_2D);
	#endif
}

GLenum Texture::format() const
{
	switch (channels_)
	{
		case 1: return GL_ALPHA;
		case 3: return GL_RGB;
		case 4: return GL_RGBA;

		default: assert(false);
	}
}

} // gfx
