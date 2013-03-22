#pragma once

class Graphics;

#include <Graphics/OpenGL.h>
#include <Graphics/Shader.h>
#include <Graphics/Vertex.h>
#include <Graphics/VertexBuffer.h>
#include <Graphics/Texture.h>
#include <Graphics/Sprite.h>
#include <Graphics/SpriteBatch.h>

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool init();
	void viewport(int width, int height);

	// drawing
	void draw(VertexBuffer *buffer);
	void draw(VertexBuffer *buffer, size_t count);
	void draw(VertexBuffer *buffer, size_t offset, size_t count);
	void draw(SpriteBatch *spriteBatch);
	void clear();
	void bgcolor(float r, float g, float b, float a = 1.0f);

	// create
	Texture *texture(const char *path, Texture::Mode mode = Texture::kDefault);
	VertexBuffer *buffer(VertexBuffer::Mode mode, VertexBuffer::Usage usage, size_t size);
	VertexBuffer *buffer(VertexBuffer::Mode mode, VertexBuffer::Usage vboUsage, VertexBuffer::Usage iboUsage, size_t vboSize, size_t iboSize);
	SpriteBatch *batch(size_t maxSize);

	// bind
	void texture(Texture *tx);
	void buffer(VertexBuffer *buffer);
	Texture *texture();
	VertexBuffer *buffer();

	// matrix
	void save();
	void restore();
	const mat4 &matrix();
	void matrix(const mat4 &m);
	void translate(float x, float y);
	void rotate(float angle);
	void scale(float width, float height);
	void transform(const mat4 &m);

private:
	enum Uniform
	{
		kMatrix = 0,
		kProjection,
		kSampler,
		kUniformCount
	};

	struct State
	{
		mat4 matrix;
	};

	mat4 matrix_;
	mat4 projection_;
	std::stack<State> stack_;

	Shader shader_;
	GLint uniforms_[kUniformCount];

	Texture *currentTexture_;
	VertexBuffer *currentBuffer_;

	bool matrixFlagged_;
	bool bufferFlagged_;
};
