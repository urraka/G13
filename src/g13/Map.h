#pragma once

#include <g13/g13.h>
#include <gfx/forward.h>
#include <gfx/gl.h>
#include <vector>

namespace g13 {

class Map
{
public:
	Map();
	~Map();
	void load();
	void draw(const ent::Camera *camera);

	const coll::World *world() const { return world_; }

protected:
	coll::World *world_;

	gfx::VBO *vbo_;
	gfx::IBO *ground_;
	gfx::IBO *outlines_;
	gfx::IBO *diagram_;
	gfx::IBO *background_;
	gfx::SpriteBatch *rocks_;
	gfx::Texture *textures_[2];
	gfx::SpriteBatch *parallax_;
	gfx::VBO *sky_;

	GLint colorLocation_;
};

} // g13
