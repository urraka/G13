#pragma once

#include "OpenGL.h"
#include "Vertex.h"

#include <string>

class Shader
{
public:
	Shader();
	~Shader();

	void load(const std::string &srcVertex, const std::string &srcFragment, int nAttributes, AttribCallback attrib);
	void bind();
	void unbind();
	GLuint id() const;
	int attribCount() const;

protected:
	GLuint program_;
	int nAttributes_;

	GLuint createShader(GLenum type, const char *source);
	GLuint createProgram(GLuint vertexShader, GLuint fragmentShader, int nAttributes, AttribCallback attrib);
};
