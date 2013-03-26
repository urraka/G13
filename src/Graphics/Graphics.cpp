#include <pch.h>

#define VBO_TEMPLATE_INSTANCE(VertexT) \
	template void Graphics::draw<VertexT>(VBO<VertexT> *vbo); \
	template void Graphics::draw<VertexT>(VBO<VertexT> *vbo, size_t count); \
	template void Graphics::draw<VertexT>(VBO<VertexT> *vbo, size_t offset, size_t count); \
	template VBO<VertexT> *Graphics::buffer<VertexT>(VBO<VertexT>::Mode mode, VBO<VertexT>::Usage vboUsage, VBO<VertexT>::Usage iboUsage, size_t vboSize, size_t iboSize); \
	template VBO<VertexT> *Graphics::buffer<VertexT>(VBO<VertexT>::Mode mode, VBO<VertexT>::Usage usage, size_t size);

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
		currentBuffer_(0),
		nEnabledAttributes_(0),
		bufferFlagged_(false)
{
}

Graphics::~Graphics()
{
	for (int i = 0; i < ShaderCount; i++)
		delete shaders_[i];
}

bool Graphics::init()
{
	#if !defined(IOS)
		if (glewInit() != GLEW_OK)
		{
			error_log("Error initializing glew.");
			return false;
		}
	#endif

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shaders_[TextureShader] = new ::Shader();
	shaders_[TextureShader]->load(shaders::texture.vert, shaders::texture.frag, Vertex::AttributesCount, Vertex::attrib);

	shaders_[ColorShader] = new ::Shader();
	shaders_[ColorShader]->load(shaders::color.vert, shaders::color.frag, ColorVertex::AttributesCount, ColorVertex::attrib);

	const char *uniformNames[UniformCount] = {0};

	uniformNames[UniformMatrix] = "matrix";
	uniformNames[UniformProjection] = "projection";
	uniformNames[UniformSampler] = "sampler";
	uniformNames[UniformTextureEnabled] = "textureEnabled";

	for (int iShader = 0; iShader < ShaderCount; iShader++)
	{
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

	return true;
}

void Graphics::uniformModified(int iUniform)
{
	for (int i = 0; i < ShaderCount; i++)
		uniforms_[i][iUniform].modified = true;
}

void Graphics::updateUniforms()
{
	Uniform (&uniforms)[UniformCount] = uniforms_[currentShader_];

	if (uniforms[UniformProjection].modified)
	{
		uniforms[UniformProjection].modified = false;
		glUniformMatrix4fv(uniforms[UniformProjection].location, 1, GL_FALSE, glm::value_ptr(projection_));
	}

	if (uniforms[UniformMatrix].modified)
	{
		uniforms[UniformMatrix].modified = false;
		glUniformMatrix4fv(uniforms[UniformMatrix].location, 1, GL_FALSE, glm::value_ptr(matrix_));
	}

	if (uniforms[UniformTextureEnabled].modified && uniforms[UniformTextureEnabled].location != -1)
	{
		uniforms[UniformTextureEnabled].modified = false;
		glUniform1f(uniforms[UniformTextureEnabled].location, currentTexture_ ? 1.0f : 0.0f);
	}
}

void Graphics::viewport(int width, int height)
{
	projection_ = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

	#if defined(IOS)
		glViewport(0, 0, height, width);
		projection_ = glm::rotate(-90.0f, 0.0f, 0.0f, 1.0f) * projection_;
	#else
		glViewport(0, 0, width, height);
	#endif

	uniformModified(UniformProjection);
}

// -----------------------------------------------------------------------------
// Draw

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

	bind<VertexT>(vbo);

	updateUniforms();

	if (bufferFlagged_)
	{
		bufferFlagged_ = false;

		for (int i = nEnabledAttributes_; i < VertexT::AttributesCount; i++)
			glEnableVertexAttribArray(i);

		for (int i = VertexT::AttributesCount; i < nEnabledAttributes_; i++)
			glDisableVertexAttribArray(i);

		for (int i = 0; i < VertexT::AttributesCount; i++)
		{
			const VertexAttribute &attr = VertexT::attrib(i);
			glVertexAttribPointer(i, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
		}

		nEnabledAttributes_ = VertexT::AttributesCount;
	}

	if (vbo->id(VBO<VertexT>::Elements) != 0)
		glDrawElements(vbo->mode(), count, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(uint16_t) * offset));
	else
		glDrawArrays(vbo->mode(), offset, count);
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
	currentBuffer_ = vbo;
	bufferFlagged_ = true;
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

void Graphics::bind(Graphics::Shader shader)
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

		glBindTexture(GL_TEXTURE_2D, tx ? tx->id() : 0);
		currentTexture_ = tx;
	}
}

template<class VertexT> void Graphics::bind(VBO<VertexT> *vbo)
{
	if (vbo->handle() != currentBuffer_)
	{
		if (vbo)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo->id(VBO<VertexT>::Vertices));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo->id(VBO<VertexT>::Elements));
		}
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		currentBuffer_ = vbo ? vbo->handle() : 0;
		bufferFlagged_ = true;
	}
}

void Graphics::bind(SpriteBatch *spriteBatch)
{
	bind(spriteBatch->buffer_);
}

// -----------------------------------------------------------------------------
// Matrix

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
