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
	void load_old();
	void draw(const ent::Camera *camera);
	void draw_old(const ent::Camera *camera);

	coll::World *world() { return world_; }
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
	gfx::Texture *groundTexture_;
	gfx::Texture *grassTexture_;
	gfx::SpriteBatch *grass_;

	GLint colorLocation_;

	void destroy();
};

} // g13
