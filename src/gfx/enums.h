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

enum ImageFormat   { RGB, RGBA };
enum TexFilterType { MinFilter = 1, MagFilter = 2, MinMagFilter = (MinFilter | MagFilter) };
enum TexFilter     { Linear = 1, Nearest = 2, NearestMipmap = 4, LinearMipmap = 8 };
enum TexWrapAxis   { WrapX = 1, WrapY = 2, WrapBoth = (WrapX | WrapY) };
enum TexWrap       { Repeat, Clamp };

} //gfx
