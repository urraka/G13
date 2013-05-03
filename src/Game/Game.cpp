#include "Game.h"
#include "Scene.h"
#include "Scenes/MainScene.h"
#include "../Graphics/Graphics.h"
#include "../System/Event.h"
#include "../System/Keyboard.h"
#include "../System/Application.h"
#include "../System/Window.h"
#include "Debugger.h"

#include <iostream>

Game *game = 0;

void Game::launch(Application *app)
{
	DBG( dbg = new Debugger(); );

	game = new Game();
	game->init(app);
}

void Game::display()
{
	game->input();
	game->draw();
	game->update();
}

void Game::terminate()
{
	delete game;
}

Game::Game()
	:	window(0),
		graphics(0),
		scene_(0),
		currentTime_(0),
		timeAccumulator_(0),
		dt_(0),
		fpsTimer_(0),
		fps_(0),
		tick_(0),
		stepMode_(false)
{
}

Game::~Game()
{
	delete scene_;
	delete graphics;
}

void Game::init(Application *app)
{
	currentTime_ = Clock::time();
	dt_ = Clock::milliseconds(30);

	window = app->window(false, 0);
	window->display(Game::display);
	window->title("G13");
	window->vsync(false);

	graphics = new Graphics();
	graphics->init();

	ivec2 size;
	window->size(size.x, size.y);
	graphics->viewport(size.x, size.y, window->rotation());

	DBG( dbg->graphics = graphics; );

	scene_ = new MainScene();
	scene_->init();
}

void Game::draw()
{
	float percent = (float)(timeAccumulator_ / (double)dt_);

	DBG(
		if (stepMode_)
			percent = 1.0f;
	);

	scene_->draw(percent);
	fps_++;
}

void Game::input()
{
	Event event;

	while (window->poll(&event))
	{
		scene_->event(event);

		if (event.type == Event::Resize)
			graphics->viewport(event.resize.width, event.resize.height, event.resize.rotation);

		DBG(
			if (event.type == Event::Keyboard)
			{
				if (event.keyboard.pressed)
				{
					if (event.keyboard.key == Keyboard::F5)
					{
						stepMode_ = !stepMode_;
						currentTime_ = Clock::time();
					}

					if (stepMode_ && event.keyboard.key == Keyboard::F10)
					{
						std::cout << "tick: " << tick_ << std::endl;
						scene_->update(dt_);
						tick_++;
					}
				}
			}
		);
	}
}

void Game::update()
{
	DBG( if (stepMode_) return; );

	const Time maxFrameTime = Clock::milliseconds(250);

	Time newTime = Clock::time();
	Time frameTime = newTime - currentTime_;

	DBG(
		if (DEBUG_FPS)
		{
			fpsTimer_ += frameTime;

			if (fpsTimer_ >= Clock::seconds(1))
			{
				std::cout << "FPS: " << fps_ << " - Frame time: " << frameTime << std::endl;

				fpsTimer_ -= Clock::seconds(1);
				fps_ = 0;
			}
		}
	);

	if (frameTime > maxFrameTime)
		frameTime = maxFrameTime;

	currentTime_ = newTime;
	timeAccumulator_ += frameTime;

	while (timeAccumulator_ >= dt_)
	{
		scene_->update(dt_);
		timeAccumulator_ -= dt_;
		tick_++;
	}
}

uint64_t Game::tick() const
{
	return tick_;
}
