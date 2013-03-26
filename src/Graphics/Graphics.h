#pragma once

class Graphics;

#include <Graphics/OpenGL.h>
#include <Graphics/Vertex.h>
#include <Graphics/VBO.h>
#include <Graphics/Texture.h>
#include <Graphics/Sprite.h>
#include <Graphics/SpriteBatch.h>

class Shader;

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool init();
	void viewport(int width, int height);

	enum Shader
	{
		InvalidShader = -1,
		TextureShader = 0,
		ColorShader,
		ShaderCount
	};

	// Draw

	void clear();
	void bgcolor(float r, float g, float b, float a = 1.0f);
	void draw(SpriteBatch *spriteBatch);
	void draw(SpriteBatch *spriteBatch, size_t offset, size_t count);
	template<class VertexT> void draw(VBO<VertexT> *vbo);
	template<class VertexT> void draw(VBO<VertexT> *vbo, size_t count);
	template<class VertexT> void draw(VBO<VertexT> *vbo, size_t offset, size_t count);

	// Create (the created object gets implicitly bound)

	Texture *texture(const char *path, Texture::Mode mode = Texture::Default);
	SpriteBatch *batch(size_t maxSize);
	template<class VertexT>
	VBO<VertexT> *buffer(typename VBO<VertexT>::Mode mode, typename VBO<VertexT>::Usage vboUsage, typename VBO<VertexT>::Usage iboUsage, size_t vboSize, size_t iboSize);
	template<class VertexT>
	VBO<VertexT> *buffer(typename VBO<VertexT>::Mode mode, typename VBO<VertexT>::Usage usage, size_t size);

	// Bind

	void bind(Graphics::Shader shader);
	void bind(Texture *tx);
	void bind(SpriteBatch *spriteBatch);
	template<class VertexT> void bind(VBO<VertexT> *vbo);

	// Matrix

	void save();
	void restore();
	const mat4 &matrix();
	void matrix(const mat4 &m);
	void translate(float x, float y);
	void rotate(float angle);
	void scale(float width, float height);
	void transform(const mat4 &m);

private:
	void uniformModified(int iUniform);
	void updateUniforms();

	enum /* uniforms */
	{
		UniformMatrix = 0,
		UniformProjection,
		UniformSampler,
		UniformTextureEnabled,
		UniformCount
	};

	struct Uniform
	{
		GLint location;
		bool modified;
	};

	struct State
	{
		mat4 matrix;
	};

	mat4 matrix_;
	mat4 projection_;
	std::stack<State> stack_;

	::Shader *shaders_[ShaderCount];
	Uniform uniforms_[ShaderCount][UniformCount];

	Graphics::Shader currentShader_;
	Texture *currentTexture_;
	vbo_t currentBuffer_;
	int nEnabledAttributes_;
	bool bufferFlagged_;
};
