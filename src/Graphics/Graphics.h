#pragma once

#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_precision.hpp>

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec2;
using glm::u8vec4;

class Graphics;
class Shader;

#include <Graphics/OpenGL.h>
#include <Graphics/Vertex.h>
#include <Graphics/VBO.h>
#include <Graphics/Texture.h>
#include <Graphics/Sprite.h>
#include <Graphics/SpriteBatch.h>

class Graphics
{
public:
	enum ShaderType
	{
		InvalidShader = -1,
		ColorShader,
		TextureShader,
		MixedShader,
		ShaderTypeCount
	};

	Graphics();
	~Graphics();

	void init();
	void viewport(int width, int height, int rotation);

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

	void bind(Texture *tx);
	void bind(ShaderType shader);
	template<class VertexT> void bind(VBO<VertexT> *vbo, typename VBO<VertexT>::Type type);

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

	Shader *shaders_[ShaderTypeCount];
	Uniform uniforms_[ShaderTypeCount][UniformCount];

	ShaderType currentShader_;
	Texture *currentTexture_;
	vbo_t currentVbo_;
	vbo_t currentIbo_;
	vbo_t pointedBuffer_;
	int nEnabledAttributes_;
};
