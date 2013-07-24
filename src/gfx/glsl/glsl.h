#pragma once

namespace gfx {
namespace glsl {

// -----------------------------------------------------------------------------
// color.vert
// -----------------------------------------------------------------------------

const char *color_vert =
	"uniform   mat4 mvp;\n"
	"attribute vec2 in_position;\n"
	"attribute vec4 in_color;\n"
	"varying   vec4 color;\n"
	"\n"
	"void main()\n"
	"{\n"
	"\tcolor = in_color;\n"
	"\tgl_Position = mvp * vec4(in_position, 0.0, 1.0);\n"
	"}\n";

// -----------------------------------------------------------------------------
// sprite.vert
// -----------------------------------------------------------------------------

const char *sprite_vert =
	"uniform   mat4  mvp;\n"
	"attribute vec2  in_position;\n"
	"attribute vec2  in_texcoords;\n"
	"attribute float in_opacity;\n"
	"varying   vec2  texcoords;\n"
	"varying   float opacity;\n"
	"\n"
	"void main()\n"
	"{\n"
	"\topacity = in_opacity;\n"
	"\ttexcoords = in_texcoords;\n"
	"\tgl_Position = mvp * vec4(in_position, 0.0, 1.0);\n"
	"}\n";

// -----------------------------------------------------------------------------
// text.vert
// -----------------------------------------------------------------------------

const char *text_vert =
	"uniform   mat4 mvp;\n"
	"uniform   vec2 texsize;\n"
	"attribute vec2 in_position;\n"
	"attribute vec2 in_texcoords;\n"
	"varying   vec2 texcoords;\n"
	"\n"
	"void main()\n"
	"{\n"
	"\ttexcoords = in_texcoords / texsize;\n"
	"\tgl_Position = mvp * vec4(in_position, 0.0, 1.0);\n"
	"}\n";

// -----------------------------------------------------------------------------
// color.frag
// -----------------------------------------------------------------------------

const char *color_frag =
	"varying lowp vec4 color;\n"
	"uniform sampler2D sampler;\n"
	"\n"
	"void main()\n"
	"{\n"
	"\tgl_FragColor = color;\n"
	"}\n";

// -----------------------------------------------------------------------------
// sprite.frag
// -----------------------------------------------------------------------------

const char *sprite_frag =
	"varying lowp vec2  texcoords;\n"
	"varying lowp float opacity;\n"
	"uniform sampler2D sampler;\n"
	"\n"
	"void main()\n"
	"{\n"
	"\tgl_FragColor = texture2D(sampler, texcoords);\n"
	"\tgl_FragColor.a *= opacity;\n"
	"}\n";

// -----------------------------------------------------------------------------
// text.frag
// -----------------------------------------------------------------------------

const char *text_frag =
	"varying lowp vec2 texcoords;\n"
	"uniform sampler2D sampler;\n"
	"uniform vec4      color;\n"
	"\n"
	"void main()\n"
	"{\n"
	"\tgl_FragColor = color;\n"
	"\tgl_FragColor.a *= texture2D(sampler, texcoords).a;\n"
	"}\n";

}} // gfx::glsl
