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

static TextureInfo textures[TextureCount] = {};
static FontInfo    fonts   [FontCount   ] = {};

void initialize()
{
	textures[Soldier].filename = "data/guy.png";

	fonts[DefaultFont].filename = "data/NotoSans-Bold.ttf";
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
		textures[id].texture = new gfx::Texture(textures[id].filename, false);

	return textures[id].texture;
}

gfx::Font *font(FontID id)
{
	if (fonts[id].font == 0)
		fonts[id].font = new gfx::Font(fonts[id].filename);

	return fonts[id].font;
}

}} // g13::res
