#pragma once

class Map
{
public:
	Map();
	~Map();
	void load(Graphics *graphics);
	void draw(Graphics *graphics);
protected:
	VBO<ColorVertex> *buffer_;
};
