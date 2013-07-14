#pragma once

#include <iostream>

#ifdef DEBUG
	#define debug_log(x) std::cout << "[Debug] " << x << std::endl
#else
	#define debug_log(x)
#endif

#define error_log(x) std::cerr << "[Error] " << x << std::endl

#ifdef DEBUG
#include <gfx/forward.h>

namespace g13 {

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
	int  ticksBehind;

private:
	gfx::VBO *collisionHulls[2];
};

} // g13

#endif
