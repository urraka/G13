#pragma once

#ifdef DEBUG
	#define DBG(x) { x; }
#else
	#define DBG(x)
#endif

#ifdef DEBUG

template<class T> class VBO;

class Graphics;
class ColorVertex;
class Map;

namespace ent {
	class Soldier;
}

class Debugger
{
public:
	Debugger();

	void loadCollisionHulls();
	void drawCollisionHulls();
	void showCollisionData();

	Map *map;
	ent::Soldier *soldier;
	Graphics *graphics;

	bool showCollisionHulls;
	bool showFPS;
	bool stepMode;
	bool wireframe;

private:
	VBO<ColorVertex> *collisionHulls[2];
};

extern Debugger *dbg;

#endif
