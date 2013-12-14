#include "res.h"
#include <gfx/gfx.h>

namespace g13 {
namespace res {

struct TextureInfo
{
	const char   *filename;
	gfx::Texture *texture;
};

struct FontInfo
{
	const char *filename;
	gfx::Font  *font;
};

struct CursorInfo
{
	const char *filename;
	sys::Cursor cursor;
	int cx, cy;
};

static TextureInfo textures[TextureCount] = {};
static FontInfo    fonts[FontCount]       = {};
static CursorInfo  cursors[CursorCount]   = {};

void initialize()
{
	textures[Soldier].filename = "data/soldier.png";
	textures[Rocks  ].filename = "data/rocks.png";
	textures[Tree   ].filename = "data/white-tree.png";
	textures[Bullet ].filename = "data/bullet.png";

	fonts[DefaultFont].filename = "data/NotoSans-Bold.ttf";
	fonts[Monospace  ].filename = "data/consolab.ttf";

	cursors[Crosshair].filename = "data/tcross.png";
	cursors[Crosshair].cx = 12;
	cursors[Crosshair].cy = 12;
}

void terminate()
{
	for (int i = 0; i < TextureCount; i++)
	{
		if (textures[i].texture != 0)
		{
			delete textures[i].texture;
			textures[i].texture = 0;
		}
	}

	for (int i = 0; i < FontCount; i++)
	{
		if (fonts[i].font != 0)
		{
			delete fonts[i].font;
			fonts[i].font = 0;
		}
	}
}

gfx::Texture *texture(TextureID id)
{
	if (textures[id].texture == 0)
		textures[id].texture = new gfx::Texture(textures[id].filename, true);

	return textures[id].texture;
}

gfx::Font *font(FontID id)
{
	if (fonts[id].font == 0)
		fonts[id].font = new gfx::Font(fonts[id].filename, 512);

	return fonts[id].font;
}

sys::Cursor cursor(CursorID id)
{
	CursorInfo &info = cursors[id];

	if (info.cursor == 0)
	{
		gfx::Image image = gfx::Image(info.filename);
		info.cursor = sys::create_cursor(image.width(), image.height(), info.cx, info.cy, image.data());
	}

	return info.cursor;
}

}} // g13::res
