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

namespace sys {
	class Event;
}

namespace g13 {

class Debugger
{
public:
	Debugger();
	~Debugger();

	void loadCollisionHulls();
	void drawCollisionHulls();
	void showCollisionData();
	void drawFontAtlas();
	void drawConsole();
	void drawStateBuffers(int tick, int interp, int local, const net::Player *players);

	bool event(sys::Event *evt);
	bool onKeyPressed(int key);

	Map *map;
	ent::Soldier *soldier;

	bool showCollisionHulls;
	bool showFPS;
	bool stepMode;
	bool wireframe;
	bool interpolation;
	bool extrapolation;
	int  ticksBehind;
	bool showFontAtlas;
	bool consoleEnabled;
	bool showStateBuffers;

private:
	gfx::VBO *collisionHulls[2];
	gfx::Text *consoleText_;
	gfx::VBO *playersState_;
};

} // g13

#endif
