#include "Texture.h"
#include "Image.h"
#include "Context.h"
#include "gfx.h"

#if defined(GL_ALPHA)
	#define FORMAT1 GL_ALPHA
#elif defined(GL_RED)
	#define FORMAT1 GL_RED
#endif

#define FORMAT3 GL_RGB
#define FORMAT4 GL_RGBA

#if defined(GL_ALPHA8)
	#define INTERNAL1 GL_ALPHA8
#elif defined(GL_R8)
	#define INTERNAL1 GL_R8
#else
	#define INTERNAL1 FORMAT1
#endif

#if defined(GL_RGB8)
	#define INTERNAL3 GL_RGB8
#else
	#define INTERNAL3 GL_RGB
#endif

#if defined(GL_RGBA8)
	#define INTERNAL4 GL_RGBA8
#else
	#define INTERNAL4 GL_RGBA
#endif

#define FORMAT(channels) \
	(\
		channels == 1 ? FORMAT1 :   \
		channels == 3 ? FORMAT3 :   \
		channels == 4 ? FORMAT4 : 0 \
	)

#define INTERNAL_FORMAT(channels) \
	(\
		channels == 1 ? INTERNAL1 :   \
		channels == 3 ? INTERNAL3 :   \
		channels == 4 ? INTERNAL4 : 0 \
	)

namespace gfx {

Texture::Texture() : id_(0), width_(0), height_(0), channels_(0) {}

Texture::Texture(const char *filename)
{
	load(filename);
}

Texture::Texture(const Image &image)
{
	load(image);
}

Texture::Texture(int width, int height, int channels)
{
	create(width, height, channels);
}

Texture::~Texture()
{
	if (id_ != 0)
		glDeleteTextures(1, &id_);
}

void Texture::load(const char *filename)
{
	load(Image(filename));
}

void Texture::load(const Image &image)
{
	initialize(image.width(), image.height(), image.format() == RGBA ? 4 : 3, image.data());
}

void Texture::create(int width, int height, int channels)
{
	initialize(width, height, channels, 0);
}

void Texture::update(int x, int y, int width, int height, const uint8_t *data)
{
	assert(id_ != 0);
	gfx::bind(this);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, FORMAT(channels_), GL_UNSIGNED_BYTE, data);
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

void Texture::filter(Filter filter)
{
	assert(id_ != 0);
	gfx::bind(this);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture::filterMin(Filter filter)
{
	assert(id_ != 0);
	gfx::bind(this);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
}

void Texture::filterMag(Filter filter)
{
	assert(id_ != 0);
	gfx::bind(this);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture::wrap(WrapMode mode)
{
	assert(id_ != 0);
	gfx::bind(this);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
}

void Texture::wrapX(WrapMode mode)
{
	assert(id_ != 0);
	gfx::bind(this);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
}

void Texture::wrapY(WrapMode mode)
{
	assert(id_ != 0);
	gfx::bind(this);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
}

void Texture::initialize(int width, int height, int channels, const uint8_t *data)
{
	width_    = width;
	height_   = height;
	channels_ = channels;

	if (id_ == 0)
		glGenTextures(1, &id_);

	glBindTexture(GL_TEXTURE_2D, id_);
	context->texture[context->unit] = this;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLint internalFormat = INTERNAL_FORMAT(channels);
	GLenum format = FORMAT(channels);
	GLenum type = GL_UNSIGNED_BYTE;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
}

} // gfx
