#pragma once

#include "Collision.h"
#include <gfx/forward.h>

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
	std::vector<gfx::VBO*> vbos_;
	Collision::Map collisionMap_;
};

} // g13
