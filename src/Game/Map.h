#pragma once

#include "Collision.h"

class Map
{
public:
	Map();
	~Map();
	void load(Graphics *graphics);
	void draw(Graphics *graphics);
	const Collision::Map *collisionMap() const;

protected:
	std::vector< VBO<ColorVertex>* > buffers_;
	Collision::Map collisionMap_;
};
