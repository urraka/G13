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
	"uniform   mat4  mvp;\n"
	"uniform   vec2  texsize;\n"
	"attribute vec2  in_position;\n"
	"attribute vec2  in_texcoords;\n"
	"varying   vec2  texcoords;\n"
	"varying   float shift;\n"
	"\n"
	"void main()\n"
	"{\n"
	"\ttexcoords = in_texcoords / texsize;\n"
	"\n"
	"\tvec2 pos = vec2(floor(in_position.x), in_position.y);\n"
	"\tshift = fract(in_position.x);\n"
	"\n"
	"\tgl_Position = mvp * vec4(pos, 0.0, 1.0);\n"
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
	"varying lowp vec2  texcoords;\n"
	"varying lowp float shift;\n"
	"uniform vec2       texsize;\n"
	"uniform sampler2D  sampler;\n"
	"uniform vec4       color;\n"
	"\n"
	"void main()\n"
	"{\n"
	"\tvec4 curr = texture2D(sampler, texcoords);\n"
	"\tvec4 prev = texture2D(sampler, vec2(texcoords.x - 1.0 / texsize.x, texcoords.y));\n"
	"\n"
	"\tfloat r = curr.r;\n"
	"\tfloat g = curr.g;\n"
	"\tfloat b = curr.b;\n"
	"\n"
	"\tif (shift <= 1.0 / 3.0)\n"
	"\t{\n"
	"\t\tfloat z = 3.0 * shift;\n"
	"\t\tr = mix(curr.r, prev.b, z);\n"
	"\t\tg = mix(curr.g, curr.r, z);\n"
	"\t\tb = mix(curr.b, curr.g, z);\n"
	"\t}\n"
	"\telse if (shift <= 2.0 / 3.0)\n"
	"\t{\n"
	"\t\tfloat z = 3.0 * shift - 1.0;\n"
	"\t\tr = mix(prev.b, prev.g, z);\n"
	"\t\tg = mix(curr.r, prev.b, z);\n"
	"\t\tb = mix(curr.g, curr.r, z);\n"
	"\t}\n"
	"\telse if (shift < 1.0)\n"
	"\t{\n"
	"\t\tfloat z = 3.0 * shift - 2.0;\n"
	"\t\tr = mix(prev.g, prev.r, z);\n"
	"\t\tg = mix(prev.b, prev.g, z);\n"
	"\t\tb = mix(curr.r, prev.b, z);\n"
	"\t}\n"
	"\n"
	"\tvec4 result = vec4(color.rgb, (r + g + b) / 3.0);\n"
	"\n"
	"\tfloat mn = min(min(r, g), b);\n"
	"\tfloat mx = max(max(r, g), b);\n"
	"\n"
	"\tresult = mx * result + (1.0 - mx) * vec4(r, g, b, mn);\n"
	"\tresult.a *= color.a;\n"
	"\n"
	"\tgl_FragColor = result;\n"
	"}\n";

}} // gfx::glsl
