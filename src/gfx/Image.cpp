#include "Image.h"
#include <iostream>
#include <stdio.h>
#include <setjmp.h>
#include <png.h>

namespace gfx {

static uint8_t *load_image(const char *path, int *width, int *height, ImageFormat *format);

Image::Image(const char *filename)
	:	width_(0),
		height_(0),
		format_(RGBA),
		data_(0)
{
	data_ = load_image(filename, &width_, &height_, &format_);
}

Image::~Image()
{
	if (data_ != 0)
		delete[] data_;
}

uint8_t *Image::data()
{
	return data_;
}

const uint8_t *Image::data() const
{
	return data_;
}

int Image::width() const
{
	return width_;
}

int Image::height() const
{
	return height_;
}

ImageFormat Image::format() const
{
	return format_;
}

uint8_t *load_image(const char *path, int *width, int *height, ImageFormat *format)
{
	FILE *file = fopen(path, "rb");

	if (!file)
	{
		std::cerr << "load_image(" << path << ") - Failed to open file." << std::endl;
		return 0;
	}

	size_t headerSize = 8;
	uint8_t header[8];
	headerSize = fread(header, 1, headerSize, file);

	if (png_sig_cmp(header, 0, headerSize))
	{
		std::cerr << "load_image(" << path << ") - File is not PNG." << std::endl;
		fclose(file);
		return 0;
	}

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png)
	{
		std::cerr << "load_image(" << path << ") - png_create_read_struct() failed." << std::endl;
		fclose(file);
		return 0;
	}

	png_infop info = png_create_info_struct(png);

	if (!info)
	{
		std::cerr << "load_image(" << path << ") - First png_create_info_struct() failed." << std::endl;
		png_destroy_read_struct(&png, NULL, NULL);
		fclose(file);
		return 0;
	}

	png_infop info_end = png_create_info_struct(png);

	if (!info_end)
	{
		std::cerr << "load_image(" << path << ") - Second png_create_info_struct() failed." << std::endl;
		png_destroy_read_struct(&png, &info, NULL);
		fclose(file);
		return 0;
	}

	if (setjmp(png_jmpbuf(png)))
	{
		std::cerr << "load_image(" << path << ") - setjmp() failed." << std::endl;
		png_destroy_read_struct(&png, &info, &info_end);
		fclose(file);
		return 0;
	}

	png_init_io(png, file);
	png_set_sig_bytes(png, headerSize);
	png_read_png(png, info, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	int color = png_get_color_type(png, info);
	*width = png_get_image_width(png, info);
	*height = png_get_image_height(png, info);

	if (color != PNG_COLOR_TYPE_RGB && color != PNG_COLOR_TYPE_RGB_ALPHA)
	{
		std::cerr << "load_image(" << path << ") - Image must be RGB or RGBA." << std::endl;
		png_destroy_read_struct(&png, &info, &info_end);
		fclose(file);
		return 0;
	}

	png_bytep *rows = png_get_rows(png, info);

	*format = (color == PNG_COLOR_TYPE_RGB_ALPHA ? RGBA : RGB);
	int channels = (*format == GL_RGB ? 3 : 4);
	uint8_t *image = new uint8_t[*width * *height * channels];

	for (int y = 0; y < *height; y++)
		memcpy(image + y * *width * channels, rows[y], *width * channels);

	png_destroy_read_struct(&png, &info, &info_end);
	fclose(file);

	return image;
}

} // gfx
