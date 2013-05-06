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
class Soldier;
class Map;

class Debugger
{
public:
	Debugger();

	void loadCollisionHulls();
	void drawCollisionHulls();
	void showCollisionData();

	Map *map;
	Soldier *soldier;
	Graphics *graphics;

	bool showCollisionHulls;
	bool showFPS;
	bool stepMode;

private:
	VBO<ColorVertex> *collisionHulls[2];
};

extern Debugger *dbg;

#endif
