#include <pch.h>
#include <Graphics/OpenGL.h>
#include <Graphics/Vertex.h>
#include <Graphics/Shader.h>

Shader::Shader()
	:	program_(0),
		nAttributes_(0)
{
}

Shader::~Shader()
{
	if (program_)
		glDeleteProgram(program_);
}

void Shader::load(const std::string &vertexSource, const std::string &fragmentSource, int nAttributes, AttribCallback attrib)
{
	nAttributes_ = nAttributes;

	GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexSource.c_str());
	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());

	program_ = createProgram(vertexShader, fragmentShader, nAttributes, attrib);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::bind()
{
	glUseProgram(program_);
}

void Shader::unbind()
{
	glUseProgram(0);
}

GLuint Shader::createShader(GLenum type, const char *source)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

		GLchar *info = new GLchar[length + 1];
		glGetShaderInfoLog(shader, length, 0, info);

		error_log((type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader failed to compile:" << std::endl << info);

		delete[] info;
	}

	return shader;
}

GLuint Shader::createProgram(GLuint vertexShader, GLuint fragmentShader, int nAttributes, AttribCallback attrib)
{
	GLuint program = glCreateProgram();

	for (int i = 0; i < nAttributes; i++)
		glBindAttribLocation(program, i, attrib(i).name);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

		GLchar *info = new GLchar[length + 1];
		glGetProgramInfoLog(program, length, 0, info);

		error_log("Failed to link program:" << std::endl << info);

		delete[] info;
	}

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	return program;
}

int Shader::attribCount() const
{
	return nAttributes_;
}

GLuint Shader::id() const
{
	return program_;
}
