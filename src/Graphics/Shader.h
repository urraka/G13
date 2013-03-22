#pragma once

class Shader
{
public:
	Shader();
	~Shader();

	bool load(const std::string &srcVertex, const std::string &srcFragment, const std::vector<std::string> &attributes);
	void bind();
	void unbind();
	GLuint id() const { return program_; }

protected:
	GLuint program_;

	GLuint createShader(GLenum type, const char *source);
	GLuint createProgram(GLuint vertexShader, GLuint fragmentShader, const std::vector<std::string> &attributes);
};
