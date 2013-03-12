#pragma once

#include <Graphics/Texture.h>

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool init();
	void clear();
	void viewport(int x, int y, int width, int height);

	void test(float t);

private:
	GLuint program_;
	GLuint buffer_;
	GLfloat vertices_[8];
	Texture texture_;

	GLint timeSlot_;
	GLint textureSlot_;
};
