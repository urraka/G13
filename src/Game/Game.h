#pragma once

class Window;
class Graphics;
class Events;

class Game {
public:
	Game();

	bool init();
	void terminate();
	void draw();
	void input();
	void update();
	void quit();
	void loop();

	Events *events;
	Window *window;
	Graphics *graphics;

	ivec2 viewSize;

private:
	// all timing stuff in microseconds
	uint64_t currentTime_;
	uint64_t timeAccumulator_;
	uint64_t dt_;
	uint64_t time_;
	uint64_t fpsTimer_;
	int fps_;

	bool quit_;
};

extern Game game;
