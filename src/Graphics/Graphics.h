#pragma once
#include <Graphics/OpenGL.h>
#include <Graphics/Shader.h>
#include <Graphics/Vertex.h>
#include <Graphics/Geometry.h>
#include <Graphics/Texture.h>
#include <Graphics/Sprite.h>

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool init();
	void bgcolor(float r, float g, float b, float a = 1.0f);
	void viewport(int width, int height);
	void texture(const Texture *tex);
	void clear();
	void add(Sprite sprite);
	void draw();
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

	struct VBO
	{
		enum Enum
		{
			ArrayBuffer = 0,
			ElementArrayBuffer = 1,
			kVertices = 5000 * Sprite::kVertices,
			kIndices = 5000 * Sprite::kIndices
		};
	};

	struct State
	{
		mat4 matrix;
	};

	bool matrixChanged_;
	mat4 matrix_;
	mat4 projection_;
	std::stack<State> stack_;
	GLuint vbo_[2];
	size_t vboIndex_[2];
	GLuint texture_;
	Shader shader_;
	GLint uniforms_[Uniform::Count];
};
