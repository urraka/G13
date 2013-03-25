#pragma once

class Shader
{
public:
	Shader();
	~Shader();

	void load(const std::string &srcVertex, const std::string &srcFragment, int nAttributes, AttribCallback attrib);
	void bind();
	void unbind();
	GLuint id() const { return program_; }

protected:
	GLuint program_;

	GLuint createShader(GLenum type, const char *source);
	GLuint createProgram(GLuint vertexShader, GLuint fragmentShader, int nAttributes, AttribCallback attrib);
};
