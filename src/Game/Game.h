#pragma once

#include "../System/Clock.h"
#include "States/State.h"

#include <stdint.h>

class Application;
class Window;

class Game
{
public:
	Game();
	~Game();

	static void launch(Application *app);
	static void terminate();
	static void display();

	void init(Application *app);
	void draw();
	void input();
	void update();
	uint64_t tick() const;

	void quit();
	void state(stt::State *state);

	Window *window;

private:
	stt::State *state_;

	Time currentTime_;
	Time timeAccumulator_;
	Time dt_;
	Time fpsTimer_;
	int fps_;
	uint64_t tick_;
	bool quit_;
};

extern Game *game;
