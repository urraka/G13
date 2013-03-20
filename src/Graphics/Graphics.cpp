#include <pch.h>
#include <Graphics/Graphics.h>
#include <Graphics/shaders.h>

Graphics::Graphics()
	:	texture_(0),
		matrixChanged_(false),
		matrix_(1.0f),
		projection_(1.0f),
		uniforms_(),
		vbo_(),
		vboIndex_()
{
}

Graphics::~Graphics()
{
	shader_.bind(false);
	glDeleteBuffers(2, vbo_);
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

	uniforms_[Uniform::Matrix] = glGetUniformLocation(shader_.id(), "matrix");
	uniforms_[Uniform::Projection] = glGetUniformLocation(shader_.id(), "projection");
	uniforms_[Uniform::Sampler] = glGetUniformLocation(shader_.id(), "sampler");

	glUniform1i(uniforms_[Uniform::Sampler], 0);
	glUniformMatrix4fv(uniforms_[Uniform::Matrix], 1, GL_FALSE, glm::value_ptr(matrix_));
	glUniformMatrix4fv(uniforms_[Uniform::Projection], 1, GL_FALSE, glm::value_ptr(projection_));

	// vertex buffer

	glGenBuffers(2, vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[VBO::ArrayBuffer]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_[VBO::ElementArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, VBO::Size, 0, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, VBO::Size, 0, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(vec2));

	return true;
}

void Graphics::add(Geometry geometry)
{
	if ((vboIndex_[VBO::ArrayBuffer] + geometry.vcount) * sizeof(Vertex) > VBO::Size)
		return;

	if ((vboIndex_[VBO::ElementArrayBuffer] + geometry.icount) * sizeof(uint16_t) > VBO::Size)
		return;

	for (size_t i = 0; i < geometry.icount; i++)
		geometry.indices[i] += vboIndex_[VBO::ArrayBuffer];

	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vboIndex_[VBO::ArrayBuffer], sizeof(Vertex) * geometry.vcount, geometry.vertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * vboIndex_[VBO::ElementArrayBuffer], sizeof(uint16_t) * geometry.icount, geometry.indices);

	vboIndex_[VBO::ArrayBuffer] += geometry.vcount;
	vboIndex_[VBO::ElementArrayBuffer] += geometry.icount;
}

void Graphics::draw()
{
	if (matrixChanged_)
	{
		glUniformMatrix4fv(uniforms_[Uniform::Matrix], 1, GL_FALSE, glm::value_ptr(matrix_));
		matrixChanged_ = false;
	}

	glDrawElements(GL_TRIANGLES, vboIndex_[VBO::ElementArrayBuffer], GL_UNSIGNED_SHORT, (GLvoid*)0);

	vboIndex_[VBO::ArrayBuffer] = 0;
	vboIndex_[VBO::ElementArrayBuffer] = 0;
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
