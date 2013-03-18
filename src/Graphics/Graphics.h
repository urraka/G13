#pragma once

#include <Graphics/Vertex.h>
#include <Graphics/Texture.h>
#include <Graphics/Shader.h>

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool init();
	void draw();
	void clear();
	void background(float r, float g, float b, float a = 1.0f);
	void viewport(int width, int height);
	void texture(const Texture *tex);
	void save();
	void restore();
	const mat4 &matrix();
	void matrix(const mat4 &m);
	void translate(float x, float y);
	void rotate(float angle);
	void scale(float width, float height);
	void transform(const mat4 &m);

private:
	struct Uniform
	{
		enum Enum
		{
			Matrix = 0,
			Projection,
			Sampler,
			Count
		};
	};

	struct State
	{
		mat4 matrix;
	};

	bool matrixChanged_;
	mat4 matrix_;
	mat4 projection_;
	GLuint texture_;
	Shader shader_;
	GLint uniforms_[Uniform::Count];
	std::stack<State> stack_;

	// testing
	GLuint buffer_;
	Vertex vertices_[4];
};
