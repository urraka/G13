#pragma once

#include <gfx/forward.h>

namespace g13 {
namespace res {

enum TextureID
{
	Soldier,
	TextureCount
};

void initialize();
void terminate();

gfx::Texture *texture(TextureID id);

}} // g13::res
