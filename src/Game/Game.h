#pragma once

#include "../System/Clock.h"

#include <stdint.h>

class Application;
class Window;
class Graphics;
class Scene;

class Game {
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

	Window *window;
	Graphics *graphics;

private:
	Scene *scene_;

	Time currentTime_;
	Time timeAccumulator_;
	Time dt_;
	Time fpsTimer_;
	int fps_;
	uint64_t tick_;
	bool stepMode_;
};

extern Game *game;
