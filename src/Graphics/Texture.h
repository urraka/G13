#pragma once

class Texture
{
public:
	Texture();

	bool load(const char *path);
	void release();

	GLint id() const { return (GLint)textureId_; }
	operator GLint() const { return (GLint)textureId_; }

	int width() const { return width_; }
	int height() const { return height_; }

private:
	GLuint textureId_;
	int width_;
	int height_;
};
