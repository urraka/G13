#pragma once

class Texture
{
public:
	Texture();
	~Texture();

	bool load(const char *path);
	int width() const { return width_; }
	int height() const { return height_; }
	GLuint id() const { return textureId_; }

private:
	GLuint textureId_;
	int width_;
	int height_;
};
