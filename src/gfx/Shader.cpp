#define VERTEX_INSTANCE(T) \
	template void Shader::compile<T>(const char *vert, const char *frag);

#include "Shader.h"
#include "Context.h"
#include "gfx.h"
#include <assert.h>
#include <glm/gtc/type_ptr.hpp>

#ifdef DEBUG
	#include <iostream>
#endif

namespace gfx {

static GLuint create_shader(GLenum type, const char *source);
static GLuint create_program(GLuint vert, GLuint frag, const Attributes *attributes);

Shader::Shader()
	:	program_(0),
		mvp_(-1),
		mvpModified_(true),
		attributes_(0)
{
	context->shaders.push_back(this);
}

Shader::~Shader()
{
	if (context->shader == this)
		context->shader = 0;

	if (program_ != 0)
		glDeleteProgram(program_);

	for (size_t i = 0; i < context->shaders.size(); i++)
	{
		if (context->shaders[i] == this)
		{
			context->shaders.erase(context->shaders.begin() + i);
			break;
		}
	}
}

template<typename T> void Shader::compile(const char *vert, const char *frag)
{
	assert(program_ == 0);

	attributes_ = gfx::attributes<T>();

	GLuint vertShader = create_shader(GL_VERTEX_SHADER, vert);
	GLuint fragShader = create_shader(GL_FRAGMENT_SHADER, frag);

	program_ = create_program(vertShader, fragShader, attributes_);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	if (program_ != 0)
	{
		mvp_ = location("mvp");

		GLint sampler = location("sampler");

		if (sampler != -1)
			uniform(sampler, 0);
	}
}

GLuint Shader::program() const
{
	return program_;
}

GLint Shader::location(const char *name)
{
	return glGetUniformLocation(program_, name);
}

// -----------------------------------------------------------------------------
// Uniforms
// -----------------------------------------------------------------------------

void Shader::uniform(GLint location, int x)
{
	gfx::bind(this);
	glUniform1i(location, x);
}

void Shader::uniform(GLint location, float x)
{
	gfx::bind(this);
	glUniform1f(location, x);
}

void Shader::uniform(GLint location, const glm::mat4 &x)
{
	gfx::bind(this);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(x));
}

void Shader::uniform(const char *name, int x)
{
	gfx::bind(this);
	glUniform1i(location(name), x);
}

void Shader::uniform(const char *name, float x)
{
	gfx::bind(this);
	glUniform1f(location(name), x);
}

void Shader::uniform(const char *name, const glm::mat4 &x)
{
	gfx::bind(this);
	glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(x));
}

VERTEX_INSTANCES();

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

GLuint create_shader(GLenum type, const char *source)
{
	#ifdef GLES2
		const char *preamble =
			"#version 100\n"
			"precision mediump float;\n";
	#else
		const char *preamble =
			"#version 110\n"
			"#define lowp\n"
			"#define mediump\n"
			"#define highp\n";
	#endif

	const char *src[] = { preamble, source };

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 2, src, 0);
	glCompileShader(shader);

	#ifdef DEBUG
		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

		if (status == GL_FALSE)
		{
			GLint length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			GLchar *info = new GLchar[length + 1];
			glGetShaderInfoLog(shader, length, 0, info);

			std::cout << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");
			std::cout << " shader failed to compile:";
			std::cout << std::endl << info << std::endl;

			delete[] info;
		}
	#endif

	return shader;
}

GLuint create_program(GLuint vert, GLuint frag, const Attributes *attributes)
{
	GLuint program = glCreateProgram();

	for (size_t i = 0; i < attributes->size(); i++)
		glBindAttribLocation(program, i, (*attributes)[i].name);

	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);

	#ifdef DEBUG
		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);

		if (status == GL_FALSE)
		{
			GLint length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

			GLchar *info = new GLchar[length + 1];
			glGetProgramInfoLog(program, length, 0, info);

			std::cerr << "Failed to link program:" << std::endl << info << std::endl;

			delete[] info;
		}
	#endif

	glDetachShader(program, vert);
	glDetachShader(program, frag);

	return program;
}

} // gfx
