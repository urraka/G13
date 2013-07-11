#pragma once

#include "Collision.h"
#include <gfx/forward.h>

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
	Collision::Map collisionMap_;
};
