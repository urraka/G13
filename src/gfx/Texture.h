#pragma once

#include "gl.h"
#include "enums.h"
#include <stdint.h>
#include <stddef.h>

namespace gfx {

class Image;

class Texture
{
public:
	Texture(const char *filename, bool mipmap = true);
	Texture(Image *image, bool mipmap = true);
	Texture(int width, int height, int channels, bool mipmap = true);
	~Texture();

	void filter(uint8_t filter, TexFilterType type = MinMagFilter);
	void wrap(TexWrap wrap, TexWrapAxis axis = WrapBoth);

	void update(int x, int y, int width, int height, uint8_t *data);

	GLuint id() const;
	int width() const;
	int height() const;
	int channels() const;

private:
	GLuint id_;
	int width_;
	int height_;
	int channels_;
	bool mipmap_;
	GLenum format_;

	void create(uint8_t *data);
	void generateMipmap();
	GLenum format() const;
};

} // gfx
