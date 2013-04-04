#pragma once

#include <System/System.h>
#include <Graphics/Graphics.h>
#include <Game/Entity.h>
#include <Game/Scene.h>

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

	Time currentTime_;
	Time timeAccumulator_;
	Time dt_;
	Time fpsTimer_;
	int fps_;
};
