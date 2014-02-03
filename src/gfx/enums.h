#pragma once

#include "gl.h"

namespace gfx {

enum Usage
{
	Stream  = GL_STREAM_DRAW,
	Static  = GL_STATIC_DRAW,
	Dynamic = GL_DYNAMIC_DRAW
};

enum DrawMode
{
	Points        = GL_POINTS,
	LineStrip     = GL_LINE_STRIP,
	LineLoop      = GL_LINE_LOOP,
	Lines         = GL_LINES,
	TriangleStrip = GL_TRIANGLE_STRIP,
	TriangleFan   = GL_TRIANGLE_FAN,
	Triangles     = GL_TRIANGLES
};

enum Filter
{
	Nearest              = GL_NEAREST,
	Linear               = GL_LINEAR,
	NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
	NearestMipmapLinear  = GL_NEAREST_MIPMAP_LINEAR,
	LinearMipmapNearest  = GL_LINEAR_MIPMAP_NEAREST,
	LinearMipmapLinear   = GL_LINEAR_MIPMAP_LINEAR
};

enum WrapMode
{
	Clamp  = GL_CLAMP_TO_EDGE,
	Repeat = GL_REPEAT
};

enum ImageFormat
{
	RGB,
	RGBA
};

} //gfx
