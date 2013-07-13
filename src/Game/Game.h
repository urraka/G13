#pragma once

#include "States/State.h"

#include <sys/sys.h>
#include <stdint.h>

class Game
{
public:
	Game();
	~Game();

	void initialize();
	void draw();
	void input();
	void update();
	uint64_t tick() const;

	void quit();
	void state(stt::State *state);

private:
	stt::State *state_;

	sys::Time currentTime_;
	sys::Time timeAccumulator_;
	sys::Time dt_;
	sys::Time fpsTimer_;
	int fps_;
	uint64_t tick_;
};
