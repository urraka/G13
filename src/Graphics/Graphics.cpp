#include <pch.h>
#include <Graphics/Graphics.h>
#include <Graphics/shaders.h>

Graphics::Graphics()
	:	matrix_(1.0f),
		projection_(1.0f),
		uniforms_(),
		currentTexture_(0),
		currentBuffer_(0),
		matrixFlagged_(false),
		bufferFlagged_(false)
{
}

Graphics::~Graphics()
{
	shader_.unbind();
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

	// shader

	std::vector<std::string> attributes;
	attributes.push_back("position");
	attributes.push_back("texCoords");

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	shader_.load(shaders::vertex, shaders::fragment, attributes);
	shader_.bind();

	uniforms_[kMatrix] = glGetUniformLocation(shader_.id(), "matrix");
	uniforms_[kProjection] = glGetUniformLocation(shader_.id(), "projection");
	uniforms_[kSampler] = glGetUniformLocation(shader_.id(), "sampler");

	glUniform1i(uniforms_[kSampler], 0);
	glUniformMatrix4fv(uniforms_[kMatrix], 1, GL_FALSE, glm::value_ptr(matrix_));
	glUniformMatrix4fv(uniforms_[kProjection], 1, GL_FALSE, glm::value_ptr(projection_));

	return true;
}

void Graphics::draw(VertexBuffer *vertexBuffer)
{
	assert(vertexBuffer != 0);
	draw(vertexBuffer, 0, vertexBuffer->size());
}

void Graphics::draw(VertexBuffer *vertexBuffer, size_t count)
{
	assert(vertexBuffer != 0);
	draw(vertexBuffer, 0, count);
}

void Graphics::draw(VertexBuffer *vertexBuffer, size_t offset, size_t count)
{
	assert(vertexBuffer != 0);

	bind(vertexBuffer);

	if (matrixFlagged_)
	{
		matrixFlagged_ = false;
		glUniformMatrix4fv(uniforms_[kMatrix], 1, GL_FALSE, glm::value_ptr(matrix_));
	}

	if (bufferFlagged_)
	{
		bufferFlagged_ = false;
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(vec2));
	}

	if (currentBuffer_->id(VertexBuffer::IBO) != 0)
		glDrawElements(currentBuffer_->mode(), count, GL_UNSIGNED_SHORT, (GLvoid*)offset);
	else
		glDrawArrays(currentBuffer_->mode(), offset, count);
}

void Graphics::draw(SpriteBatch *spriteBatch)
{
	assert(spriteBatch != 0);
	spriteBatch->draw();
}

void Graphics::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::bgcolor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
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

	glUniformMatrix4fv(uniforms_[kProjection], 1, GL_FALSE, glm::value_ptr(projection_));
}

// ---- Create ----

Texture *Graphics::texture(const char *path, Texture::Mode mode)
{
	Texture *tx = new Texture(this);
	currentTexture_ = tx;
	tx->load(path, mode);

	return tx;
}

VertexBuffer *Graphics::buffer(VertexBuffer::Mode mode, VertexBuffer::Usage usage, size_t size)
{
	return buffer(mode, usage, usage, size, 0);
}

VertexBuffer *Graphics::buffer(VertexBuffer::Mode mode, VertexBuffer::Usage vboUsage, VertexBuffer::Usage iboUsage, size_t vboSize, size_t iboSize)
{
	VertexBuffer *vertexBuffer = new VertexBuffer(this);
	currentBuffer_ = vertexBuffer;
	bufferFlagged_ = true;
	vertexBuffer->create(mode, vboUsage, iboUsage, vboSize, iboSize);

	return vertexBuffer;
}

SpriteBatch *Graphics::batch(size_t maxSize)
{
	SpriteBatch *spriteBatch = new SpriteBatch(this);
	spriteBatch->create(maxSize);
	return spriteBatch;
}

// ---- Bind ----

void Graphics::bind(Texture *tx)
{
	if (tx != currentTexture_)
	{
		glBindTexture(GL_TEXTURE_2D, tx ? tx->id() : 0);
		currentTexture_ = tx;
	}
}

void Graphics::bind(VertexBuffer *vertexBuffer)
{
	if (vertexBuffer != currentBuffer_)
	{
		if (vertexBuffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->id(VertexBuffer::VBO));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer->id(VertexBuffer::IBO));
		}
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		currentBuffer_ = vertexBuffer;
		bufferFlagged_ = true;
	}
}

void Graphics::bind(SpriteBatch *spriteBatch)
{
	bind(spriteBatch->buffer_);
}

// ---- Matrix ----

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
	matrixFlagged_ = true;
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
