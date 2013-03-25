#include <pch.h>
#include <Graphics/Graphics.h>

#include <stdio.h>
#include <setjmp.h>
#include <png.h>

Texture::Texture(Graphics *graphics)
	:	graphics_(graphics),
		textureId_(0),
		width_(0),
		height_(0)
{
}

Texture::~Texture()
{
	if (textureId_)
		glDeleteTextures(1, &textureId_);
}

bool Texture::load(const char *path, Mode mode)
{
	assert(graphics_ != 0);

	FILE *file = fopen(path, "rb");

	if (!file)
	{
		error_log("Texture->load(" << path << ") - Failed to open file.");
		return false;
	}

	const size_t headerSize = 8;
	uint8_t header[headerSize];
	fread(header, 1, headerSize, file);

	if (png_sig_cmp(header, 0, headerSize))
	{
		error_log("Texture->load(" << path << ") - File is not PNG.");
		fclose(file);
		return false;
	}

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png)
	{
		error_log("Texture->load(" << path << ") - png_create_read_struct() failed.");
		fclose(file);
		return false;
	}

	png_infop info = png_create_info_struct(png);

	if (!info)
	{
		error_log("Texture->load(" << path << ") - First png_create_info_struct() failed.");
		png_destroy_read_struct(&png, NULL, NULL);
		fclose(file);
		return false;
	}

	png_infop info_end = png_create_info_struct(png);

	if (!info_end)
	{
		error_log("Texture->load(" << path << ") - Second png_create_info_struct() failed.");
		png_destroy_read_struct(&png, &info, NULL);
		fclose(file);
		return false;
	}

	if (setjmp(png_jmpbuf(png)))
	{
		error_log("Texture->load(" << path << ") - setjmp() failed.");
		png_destroy_read_struct(&png, &info, &info_end);
		fclose(file);
		return false;
	}

	png_init_io(png, file);
	png_set_sig_bytes(png, headerSize);
	png_read_png(png, info, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	int color = png_get_color_type(png, info);
	width_ = png_get_image_width(png, info);
	height_ = png_get_image_height(png, info);

	if (color != PNG_COLOR_TYPE_RGB && color != PNG_COLOR_TYPE_RGB_ALPHA)
	{
		error_log("Texture->load(" << path << ") - Image must be RGB or RGBA.");
		png_destroy_read_struct(&png, &info, &info_end);
		fclose(file);
		return false;
	}

	bool alpha = (color == PNG_COLOR_TYPE_RGB_ALPHA);

	glGenTextures(1, &textureId_);
	glBindTexture(GL_TEXTURE_2D, textureId_);
	glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width_, height_, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, 0);

	png_bytep *rows = png_get_rows(png, info);

	for (int y = 0; y < height_; y++)
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, y, width_, 1, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, rows[y]);

	this->mode(mode);

	png_destroy_read_struct(&png, &info, &info_end);
	fclose(file);

	std::cout << "Texture loaded (" << path << ") - ID: " << textureId_ << " - Size: " << width_ << "x" << height_ << " - Alpha: " << (alpha ? "true" : "false") << std::endl;

	return true;
}

void Texture::mode(Texture::Mode mode)
{
	assert(textureId_ != 0);
	assert(graphics_ != 0);

	graphics_->bind(this);

	GLint xWrap = (mode & RepeatX ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	GLint yWrap = (mode & RepeatY ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	GLint minFilter = (mode & LinearFilterMin ? GL_LINEAR : GL_NEAREST);
	GLint magFilter = (mode & LinearFilterMag ? GL_LINEAR : GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, xWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, yWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

int Texture::width() const
{
	return width_;
}

int Texture::height() const
{
	return height_;
}

GLuint Texture::id() const
{
	return textureId_;
}
