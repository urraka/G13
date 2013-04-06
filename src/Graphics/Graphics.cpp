#define VBO_TEMPLATE_INSTANCE(VertexT) \
	template void Graphics::draw<VertexT>(VBO<VertexT> *vbo); \
	template void Graphics::draw<VertexT>(VBO<VertexT> *vbo, size_t count); \
	template void Graphics::draw<VertexT>(VBO<VertexT> *vbo, size_t offset, size_t count); \
	template VBO<VertexT> *Graphics::buffer<VertexT>(VBO<VertexT>::Mode mode, VBO<VertexT>::Usage vboUsage, VBO<VertexT>::Usage iboUsage, size_t vboSize, size_t iboSize); \
	template VBO<VertexT> *Graphics::buffer<VertexT>(VBO<VertexT>::Mode mode, VBO<VertexT>::Usage usage, size_t size);

#include <System/System.h>
#include <Graphics/Graphics.h>
#include <Graphics/Shader.h>
#include <Graphics/shaders.h>

Graphics::Graphics()
	:	matrix_(1.0f),
		projection_(1.0f),
		shaders_(),
		uniforms_(),
		currentShader_(InvalidShader),
		currentTexture_(0),
		currentVbo_(0),
		currentIbo_(0),
		pointedBuffer_(0),
		nEnabledAttributes_(0)
{
}

Graphics::~Graphics()
{
	for (int i = 0; i < ShaderTypeCount; i++)
		delete shaders_[i];
}

void Graphics::init()
{
	#if !defined(IOS)
		if (glewInit() != GLEW_OK)
			std::cerr << "Error initializing glew." << std::endl;
	#endif

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shaders_[ColorShader] = new Shader();
	shaders_[ColorShader]->load(shaders::color.vert, shaders::color.frag, ColorVertex::AttributesCount, ColorVertex::attrib);

	shaders_[TextureShader] = new Shader();
	shaders_[TextureShader]->load(shaders::texture.vert, shaders::texture.frag, TextureVertex::AttributesCount, TextureVertex::attrib);

	shaders_[MixedShader] = new Shader();
	shaders_[MixedShader]->load(shaders::mixed.vert, shaders::mixed.frag, MixedVertex::AttributesCount, MixedVertex::attrib);

	const char *uniformNames[UniformCount] = {0};

	uniformNames[UniformMatrix] = "matrix";
	uniformNames[UniformSampler] = "sampler";
	uniformNames[UniformTextureEnabled] = "textureEnabled";

	for (int iShader = 0; iShader < ShaderTypeCount; iShader++)
	{
		assert(shaders_[iShader] != 0);

		shaders_[iShader]->bind();

		for (int iUniform = 0; iUniform < UniformCount; iUniform++)
		{
			assert(uniformNames[iUniform] != 0);

			uniforms_[iShader][iUniform].location = glGetUniformLocation(shaders_[iShader]->id(), uniformNames[iUniform]);
			uniforms_[iShader][iUniform].modified = true;
		}

		// for now there is only one sampler
		glUniform1i(uniforms_[iShader][UniformSampler].location, 0);
	}

	bind(TextureShader);
}

void Graphics::uniformModified(int iUniform)
{
	for (int i = 0; i < ShaderTypeCount; i++)
		uniforms_[i][iUniform].modified = true;
}

void Graphics::updateUniforms()
{
	Uniform *u = 0;
	Uniform (&uniforms)[UniformCount] = uniforms_[currentShader_];

	u = &uniforms[UniformMatrix];

	if (u->modified && u->location != -1)
	{
		u->modified = false;
		mat4 mvp = projection_ * matrix_;
		glUniformMatrix4fv(u->location, 1, GL_FALSE, glm::value_ptr(mvp));
	}

	u = &uniforms[UniformTextureEnabled];

	if (u->modified && u->location != -1)
	{
		u->modified = false;
		glUniform1f(u->location, currentTexture_ != 0 ? 1.0f : 0.0f);
	}
}

void Graphics::viewport(int width, int height, int rotation)
{
	assert(rotation == 0 || rotation == 90 || rotation == -90 || rotation == 180);

	projection_ = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

	if (rotation == 90 || rotation == -90)
		glViewport(0, 0, height, width);
	else
		glViewport(0, 0, width, height);

	if (rotation != 0)
		projection_ = glm::rotate((float)rotation, 0.0f, 0.0f, 1.0f) * projection_;

	uniformModified(UniformMatrix);
}

// -----------------------------------------------------------------------------
// Draw
// -----------------------------------------------------------------------------

void Graphics::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::bgcolor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

template<class VertexT> void Graphics::draw(VBO<VertexT> *vbo)
{
	assert(vbo != 0);
	draw<VertexT>(vbo, 0, vbo->size());
}

template<class VertexT> void Graphics::draw(VBO<VertexT> *vbo, size_t count)
{
	assert(vbo != 0);
	draw<VertexT>(vbo, 0, count);
}

template<class VertexT> void Graphics::draw(VBO<VertexT> *vbo, size_t offset, size_t count)
{
	assert(vbo != 0);
	assert(offset + count <= vbo->size());
	assert(shaders_[currentShader_]->attribCount() == VertexT::AttributesCount);

	updateUniforms();

	if (pointedBuffer_ != vbo->handle())
	{
		bind<VertexT>(vbo, VBO<VertexT>::Vertices);

		for (int i = nEnabledAttributes_; i < VertexT::AttributesCount; i++)
			glEnableVertexAttribArray(i);

		for (int i = VertexT::AttributesCount; i < nEnabledAttributes_; i++)
			glDisableVertexAttribArray(i);

		for (int i = 0; i < VertexT::AttributesCount; i++)
		{
			const VertexAttribute &attr = VertexT::attrib(i);
			glVertexAttribPointer(i, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
		}

		pointedBuffer_ = vbo->handle();
		nEnabledAttributes_ = VertexT::AttributesCount;
	}

	if (vbo->id(VBO<VertexT>::Elements) != 0)
	{
		bind<VertexT>(vbo, VBO<VertexT>::Elements);
		glDrawElements(vbo->mode(), count, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(uint16_t) * offset));
	}
	else
	{
		glDrawArrays(vbo->mode(), offset, count);
	}
}

void Graphics::draw(SpriteBatch *spriteBatch)
{
	assert(spriteBatch != 0);
	spriteBatch->draw(0, spriteBatch->size());
}

void Graphics::draw(SpriteBatch *spriteBatch, size_t offset, size_t count)
{
	assert(spriteBatch != 0);
	spriteBatch->draw(offset, count);
}

// -----------------------------------------------------------------------------
// Create
// -----------------------------------------------------------------------------

Texture *Graphics::texture(const char *path, Texture::Mode mode)
{
	if (currentTexture_ == 0)
		uniformModified(UniformTextureEnabled);

	Texture *tx = new Texture(this);
	currentTexture_ = tx;
	tx->load(path, mode);

	return tx;
}

template<class VertexT> VBO<VertexT> *Graphics::buffer(typename VBO<VertexT>::Mode mode, typename VBO<VertexT>::Usage usage, size_t size)
{
	return buffer<VertexT>(mode, usage, usage, size, 0);
}

template<class VertexT> VBO<VertexT> *Graphics::buffer(typename VBO<VertexT>::Mode mode, typename VBO<VertexT>::Usage vboUsage, typename VBO<VertexT>::Usage iboUsage, size_t vboSize, size_t iboSize)
{
	VBO<VertexT> *vbo = new VBO<VertexT>(this);

	currentVbo_ = vbo;

	if (iboSize > 0)
		currentIbo_ = vbo;

	vbo->create(mode, vboUsage, iboUsage, vboSize, iboSize);

	return vbo;
}

SpriteBatch *Graphics::batch(size_t maxSize)
{
	SpriteBatch *spriteBatch = new SpriteBatch(this);
	spriteBatch->create(maxSize);
	return spriteBatch;
}

// -----------------------------------------------------------------------------
// Bind
// -----------------------------------------------------------------------------

void Graphics::bind(ShaderType shader)
{
	shaders_[shader]->bind();
	currentShader_ = shader;
}

void Graphics::bind(Texture *tx)
{
	if (tx != currentTexture_)
	{
		if (!currentTexture_ != !tx)
			uniformModified(UniformTextureEnabled);

		glBindTexture(GL_TEXTURE_2D, tx != 0 ? tx->id() : 0);
		currentTexture_ = tx;
	}
}

template<class VertexT> void Graphics::bind(VBO<VertexT> *vbo, typename VBO<VertexT>::Type type)
{
	vbo_t &current = (type == VBO<VertexT>::Vertices ? currentVbo_ : currentIbo_);

	if (vbo->handle() != current)
	{
		GLenum target = (type == VBO<VertexT>::Vertices ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(target, vbo != 0 ? vbo->id(type) : 0);
		current = vbo ? vbo->handle() : 0;
	}
}

// -----------------------------------------------------------------------------
// Matrix
// -----------------------------------------------------------------------------

void Graphics::save()
{
	Graphics::State state;
	state.matrix = matrix_;
	stack_.push(state);
}

void Graphics::restore()
{
	Graphics::State &state = stack_.top();
	matrix(state.matrix);
	stack_.pop();
}

const mat4 &Graphics::matrix()
{
	return matrix_;
}

void Graphics::matrix(const mat4 &m)
{
	matrix_ = m;
	uniformModified(UniformMatrix);
}

void Graphics::translate(float x, float y)
{
	matrix(glm::translate(matrix_, vec3(x, y, 0.0f)));
}

void Graphics::rotate(float angle)
{
	matrix(glm::rotate(matrix_, angle, vec3(0.0f, 0.0f, 1.0f)));
}

void Graphics::scale(float width, float height)
{
	matrix(glm::scale(matrix_, vec3(width, height, 1.0f)));
}

void Graphics::transform(const mat4 &m)
{
	matrix(matrix_ * m);
}

VBO_TEMPLATE_INSTANCES();
