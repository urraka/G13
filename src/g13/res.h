#pragma once

#include <sys/sys.h>
#include <gfx/forward.h>

namespace g13 {
namespace res {

enum TextureID
{
	Soldier,
	Rocks,
	Tree,
	Bullet,
	Rope,
	TextureCount
};

enum FontID
{
	DefaultFont,
	Monospace,
	FontCount
};

enum CursorID
{
	Crosshair,
	CursorCount
};

void initialize();
void terminate();

gfx::Texture *texture(TextureID id);
gfx::Font    *font(FontID id);
sys::Cursor   cursor(CursorID id);

}} // g13::res
