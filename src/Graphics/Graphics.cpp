#include <pch.h>
#include <Graphics/OpenGL.h>
#include <Graphics/Graphics.h>

Graphics::Graphics()
	:	program_(0),
		buffer_(0),
		timeSlot_(0),
		textureSlot_(0)
{
}

Graphics::~Graphics()
{
	texture_.release();

	glUseProgram(0);

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

	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!texture_.load("data/tree.png"))
		error_log("Graphics->init() - Failed to load texture.");

	// vertex shader

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);

	const char *vertSource =
		#if defined(IOS)
			"#version 100\n"
		#else
			"#version 110\n"
		#endif
			"attribute vec2 position;"
			"varying vec2 pos;"
			"void main() {"
		#if defined(IOS)
			"	pos = mat2(0, -1, 1, 0) * position;" // rotate 90° for landscape mode
		#else
			"	pos = position;"
		#endif
			"	gl_Position = vec4(pos, 0.0, 1.0);"
			"}";

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
			"uniform sampler2D tex;"
			"varying lowp vec2 pos;"
		#else
			"#version 110\n"
			"uniform float time;"
			"uniform sampler2D tex;"
			"varying vec2 pos;"
		#endif
			"void main() {"
			"	float r = (pos.x + 1.0) / 4.0 + ((cos(time) + 1.0) / 4.0);"
			"	float b = (pos.y + 1.0) / 4.0 + ((sin(time) + 1.0) / 4.0);"
			"	float g = mix(r, b, r * b);"
			"	vec2 coords = vec2((pos.x + 1.0) / 2.0, (pos.y + 1.0) / 2.0);"
			"	vec4 color = texture2D(tex, coords);"
			"	gl_FragColor = color;"
			"}";

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

	timeSlot_ = glGetUniformLocation(program_, "time");
	textureSlot_ = glGetUniformLocation(program_, "tex");

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

void Graphics::viewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void Graphics::test(float t)
{
	glUseProgram(program_);
	glUniform1f(timeSlot_, t);
	glUniform1i(textureSlot_, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffer_);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
