#include "res.h"
#include <gfx/gfx.h>

namespace g13 {
namespace res {

struct TextureInfo
{
	const char   *filename;
	gfx::Texture *texture;
};

static TextureInfo textures[TextureCount] = {};

void initialize()
{
	textures[Soldier].filename = "data/guy.png";
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
}

gfx::Texture *texture(TextureID id)
{
	if (textures[id].texture == 0)
		textures[id].texture = new gfx::Texture(textures[id].filename);

	return textures[id].texture;
}

}} // g13::res
