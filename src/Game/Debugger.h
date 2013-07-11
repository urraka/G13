#pragma once

#ifdef DEBUG
	#define DBG(x) { x; }
	#define debug_log(x) std::cout << "[Debug] " << x << std::endl;
#else
	#define DBG(x)
	#define debug_log(x)
#endif

#ifdef DEBUG

#include "../System/Event.h"
#include <iostream>

template<class T> class VBO;

class Graphics;
struct ColorVertex;
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
	bool extrapolation;
	int ticksBehind;

private:
	VBO<ColorVertex> *collisionHulls[2];
};

extern Debugger *dbg;

#endif
