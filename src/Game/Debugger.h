#pragma once

#ifdef DEBUG
	#define DBG(x) { x; }
	#define debug_log(x) std::cout << "[Debug] " << x << std::endl;
#else
	#define DBG(x)
	#define debug_log(x)
#endif

#ifdef DEBUG

#include <iostream>

#include <gfx/forward.h>

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

	void onKeyPressed(int key);

	Map *map;
	ent::Soldier *soldier;

	bool showCollisionHulls;
	bool showFPS;
	bool stepMode;
	bool wireframe;
	bool interpolation;
	bool extrapolation;
	int ticksBehind;

private:
	gfx::VBO *collisionHulls[2];
};

extern Debugger *dbg;

#endif
