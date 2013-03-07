#pragma once

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool init();
	void clear();
	void viewport(int width, int height);

	void test(float t);

private:
	GLuint program_;
	GLuint buffer_;
	GLfloat vertices_[8];
	GLint timeLoc_;
};
