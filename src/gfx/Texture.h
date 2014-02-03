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
	Texture();
	Texture(const char *filename);
	Texture(const Image &image);
	Texture(int width, int height, int channels);
	~Texture();

	void load(const char *filename);
	void load(const Image &image);
	void create(int width, int height, int channels);
	void update(int x, int y, int width, int height, const uint8_t *data);
	void generateMipmap();

	void filter(Filter filter);
	void filterMin(Filter filter);
	void filterMag(Filter filter);

	void wrap(WrapMode mode);
	void wrapX(WrapMode mode);
	void wrapY(WrapMode mode);

	GLuint id()       const { return id_;       }
	int    width()    const { return width_;    }
	int    height()   const { return height_;   }
	int    channels() const { return channels_; }

private:
	GLuint id_;
	int width_;
	int height_;
	int channels_;

	void initialize(int width, int height, int channels, const uint8_t *data);
};

} // gfx
