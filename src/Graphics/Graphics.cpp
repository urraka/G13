#include <pch.h>
#include <Graphics/OpenGL.h>
#include <Graphics/Graphics.h>
#include <Graphics/shaders.h>

Graphics::Graphics()
	:	texture_(0),
		matrixChanged_(false),
		matrix_(1.0f),
		projection_(1.0f),
		uniforms_(),

		// testing
		buffer_(0)
{
}

Graphics::~Graphics()
{
	shader_.bind(false);

	if (buffer_ != 0)
		glDeleteBuffers(1, &buffer_);
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

	for (size_t i = 0; i < attributes.size(); i++)
		glEnableVertexAttribArray(i);

	shader_.load(shaders::vertex, shaders::fragment, attributes);
	shader_.bind();

	uniforms_[Uniform::Matrix] = glGetUniformLocation(shader_.id(), "matrix");
	uniforms_[Uniform::Projection] = glGetUniformLocation(shader_.id(), "projection");
	uniforms_[Uniform::Sampler] = glGetUniformLocation(shader_.id(), "sampler");

	glUniform1i(uniforms_[Uniform::Sampler], 0);
	glUniformMatrix4fv(uniforms_[Uniform::Matrix], 1, GL_FALSE, glm::value_ptr(matrix_));
	glUniformMatrix4fv(uniforms_[Uniform::Projection], 1, GL_FALSE, glm::value_ptr(projection_));

	// vertex buffer

	vertices_[0].position = vec2(0.0f, 0.0f);
	vertices_[0].texcoords = vec2(0.0f, 0.0f);

	vertices_[1].position = vec2(256.0f, 0.0f);
	vertices_[1].texcoords = vec2(1.0f, 0.0f);

	vertices_[2].position = vec2(256.0f, 256.0f);
	vertices_[2].texcoords = vec2(1.0f, 1.0f);

	vertices_[3].position = vec2(0.0f, 256.0f);
	vertices_[3].texcoords = vec2(0.0f, 1.0f);

	glGenBuffers(1, &buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

void Graphics::draw()
{
	if (matrixChanged_)
		glUniformMatrix4fv(uniforms_[Uniform::Matrix], 1, GL_FALSE, glm::value_ptr(matrix_));

	matrixChanged_ = false;

	glBindBuffer(GL_ARRAY_BUFFER, buffer_);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(vec2));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Graphics::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::background(float r, float g, float b, float a)
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

	glUniformMatrix4fv(uniforms_[Uniform::Projection], 1, GL_FALSE, glm::value_ptr(projection_));
}

void Graphics::texture(const Texture *tex)
{
	GLuint id = (tex != 0 ? tex->id() : 0);

	if (id != texture_)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		texture_ = id;
	}
}

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
	matrixChanged_ = true;
}

void Graphics::translate(float x, float y)
{
	matrix(glm::translate(x, y, 0.0f) * matrix_);
}

void Graphics::rotate(float angle)
{
	matrix(glm::rotate(angle, 0.0f, 0.0f, 1.0f) * matrix_);
}

void Graphics::scale(float width, float height)
{
	matrix(glm::scale(width, height, 1.0f) * matrix_);
}

void Graphics::transform(const mat4 &m)
{
	matrix(m * matrix_);
}
