#pragma once

#include "Collision.h"
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
	void draw();
	const Collision::Map *collisionMap() const;

protected:
	Collision::Map collisionMap_;

	gfx::VBO *vbo_;
	gfx::IBO *ground_;
	gfx::IBO *outlines_;
	gfx::IBO *diagram_;
	gfx::IBO *background_;
	gfx::SpriteBatch *rocks_;

	GLint colorLocation_;
};

/*
class Map
{
public:
	Map();
	~Map();
	void load();
	void draw();
	const Collision::Map *collisionMap() const;

protected:
	std::vector<gfx::VBO*> vbos_;
	gfx::SpriteBatch *rocks_;
	gfx::SpriteBatch *trees_;
	Collision::Map collisionMap_;
};
*/

} // g13
