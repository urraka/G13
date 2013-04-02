#pragma once

#include <vector>

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

	Window *window;
	Graphics *graphics;

private:
	Scene *scene_;

	uint64_t currentTime_;
	uint64_t timeAccumulator_;
	uint64_t dt_;
	uint64_t time_;
	uint64_t fpsTimer_;
	int fps_;
};
