#pragma once

class Map
{
public:
	Map();
	~Map();
	void load(Graphics *graphics);
	void draw(Graphics *graphics);
protected:
	std::vector< VBO<ColorVertex>* > buffers_;
};
