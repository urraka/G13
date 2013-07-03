#pragma once

#ifdef DEBUG
	#define DBG(x) { x; }
#else
	#define DBG(x)
#endif

#ifdef DEBUG

#include "../System/Event.h"
#include <iostream>

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
	~Debugger();

	void loadCollisionHulls();
	void drawCollisionHulls();
	void showCollisionData();

	void onKeyPressed(Keyboard::Key key);

	Map *map;
	ent::Soldier *soldier;
	Graphics *graphics;

	bool showCollisionHulls;
	bool showFPS;
	bool stepMode;
	bool wireframe;
	bool interpolation;

private:
	VBO<ColorVertex> *collisionHulls[2];
};

extern Debugger *dbg;

#endif
