#pragma once

#include <gfx/forward.h>

namespace g13 {
namespace res {

enum TextureID
{
	Soldier,
	TextureCount
};

enum FontID
{
	DefaultFont,
	Monospace,
	FontCount
};

void initialize();
void terminate();

gfx::Texture *texture(TextureID id);
gfx::Font *font(FontID id);

}} // g13::res
