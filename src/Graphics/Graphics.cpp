#include <pch.h>
#include <Graphics/OpenGL.h>
#include <Graphics/Graphics.h>

Graphics::Graphics()
	:	program_(0),
		buffer_(0),
		timeLoc_(0)
{
}

Graphics::~Graphics()
{
	if (buffer_ != 0)
		glDeleteBuffers(1, &buffer_);

	if (program_ != 0)
		glDeleteProgram(program_);
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

	GLint status;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// vertex shader

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);

	const char *vertSource =
		#if defined(IOS)
			"#version 100\n"
			"attribute vec2 position;"
			"varying vec2 pos;"
			"void main() {"
			"	pos = mat2(0, -1, 1, 0) * position;"
			"	gl_Position = vec4(pos, 0.0, 1.0);"
			"}";
		#else
			"#version 110\n"
			"attribute vec2 position;"
			"varying vec2 pos;"
			"void main() {"
			"	pos = position;"
			"	gl_Position = vec4(position, 0.0, 1.0);"
			"}";
		#endif

	glShaderSource(vertShader, 1, &vertSource, 0);
	glCompileShader(vertShader);
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		error_log("Error compiling vertex shader.");
		return false;
	}

	// fragment shader

	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char *fragSource =
		#if defined(IOS)
			"#version 100\n"
			"precision mediump float;"
			"uniform float time;"
			"varying lowp vec2 pos;"
			"void main() {"
			"	float r = (pos.x + 1.0) / 4.0 + ((cos(time) + 1.0) / 4.0);"
			"	float b = (pos.y + 1.0) / 4.0 + ((sin(time) + 1.0) / 4.0);"
			"	float g = mix(r, b, r * b);"
			"	gl_FragColor = vec4(vec3(r, g, b), 1.0);"
			"}";
		#else
			"#version 110\n"
			"uniform float time;"
			"varying vec2 pos;"
			"void main() {"
			"	float r = (pos.x + 1.0) / 4.0 + ((cos(time) + 1.0) / 4.0);"
			"	float b = (pos.y + 1.0) / 4.0 + ((sin(time) + 1.0) / 4.0);"
			"	float g = mix(r, b, r * b);"
			"	gl_FragColor = vec4(vec3(r, g, b), 1.0);"
			"}";
		#endif

	glShaderSource(fragShader, 1, &fragSource, 0);
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		error_log("Error compiling fragment shader.");
		return false;
	}

	// program

	program_ = glCreateProgram();

	glBindAttribLocation(program_, 0, "position");
	glAttachShader(program_, vertShader);
	glAttachShader(program_, fragShader);
	glLinkProgram(program_);
	glGetProgramiv(program_, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
		error_log("Error linking program.");

	timeLoc_ = glGetUniformLocation(program_, "time");

	// shaders cleanup

	glDetachShader(program_, vertShader);
	glDetachShader(program_, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	// vertex buffer

	vertices_[0] = -1.0f;	vertices_[1] = 1.0f;
	vertices_[2] = -1.0f;	vertices_[3] = -1.0f;
	vertices_[4] = 1.0f;	vertices_[5] = -1.0f;
	vertices_[6] = 1.0f;	vertices_[7] = 1.0f;

	glGenBuffers(1, &buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

void Graphics::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::viewport(int width, int height)
{
	glViewport(0, 0, width, height);
}

void Graphics::test(float t)
{
	glUseProgram(program_);
	glUniform1f(timeLoc_, t);

	glBindBuffer(GL_ARRAY_BUFFER, buffer_);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
