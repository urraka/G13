#pragma once

#include "enums.h"
#include <stdint.h>

namespace gfx {

class Image
{
public:
	Image(const char *filename);
	~Image();

	uint8_t *data();
	const uint8_t *data() const;

	int width() const;
	int height() const;
	ImageFormat format() const;

private:
	int width_;
	int height_;
	ImageFormat format_;
	uint8_t *data_;
};

} // gfx
