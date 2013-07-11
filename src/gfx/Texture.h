#pragma once

#include "gl.h"
#include "enums.h"
#include <stdint.h>

namespace gfx {

class Image;

class Texture
{
public:
	Texture(const char *filename);
	Texture(Image *image);
	~Texture();

	void filter(uint8_t filter, TexFilterType type = MinMagFilter);
	void wrap(TexWrap wrap, TexWrapAxis axis = WrapBoth);

	GLuint id    () const;
	int    width () const;
	int    height() const;

private:
	GLuint id_;
	int width_;
	int height_;

	void init(Image *image);
};

} // gfx
