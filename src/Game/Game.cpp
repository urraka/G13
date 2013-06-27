#include "Game.h"
#include "States/Connecting.h"
#include "States/Testing.h"
#include "Debugger.h"

#include "../Graphics/Graphics.h"
#include "../System/Event.h"
#include "../System/Keyboard.h"
#include "../System/Application.h"
#include "../System/Window.h"

#include <enet/enet.h>

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
		state_(0),
		currentTime_(0),
		timeAccumulator_(0),
		dt_(0),
		fpsTimer_(0),
		fps_(0),
		tick_(0),
		quit_(false)
{
}

Game::~Game()
{
	delete state_;
	delete graphics;

	enet_deinitialize();
}

void Game::init(Application *app)
{
	currentTime_ = Clock::time();
	dt_ = Clock::milliseconds(30);

	window = app->window(false, 0);
	window->display(Game::display);
	window->title("G13");
	window->vsync(true);

	graphics = new Graphics();
	graphics->init();

	ivec2 size;
	window->size(size.x, size.y);
	graphics->viewport(size.x, size.y, window->rotation());

	DBG( dbg->graphics = graphics; );

	if (enet_initialize() != 0)
		std::cerr << "Failed to initialize enet." << std::endl;

	state_ = new stt::Testing();
	//state_ = new stt::Connecting();
}

void Game::draw()
{
	if (quit_) return;

	float percent = (float)(timeAccumulator_ / (double)dt_);

	DBG(
		if (dbg->stepMode)
			percent = 1.0f;
	);

	state_->draw(percent);
	fps_++;
}

void Game::input()
{
	Event event;

	while (window->poll(&event))
	{
		state_->event(event);

		if (event.type == Event::Resize)
			graphics->viewport(event.resize.width, event.resize.height, event.resize.rotation);

		DBG(
			if (event.type == Event::Keyboard)
			{
				if (event.keyboard.pressed)
				{
					if (event.keyboard.key == Keyboard::F6)
						dbg->showFPS = !dbg->showFPS;

					if (event.keyboard.key == Keyboard::F5)
					{
						dbg->stepMode = !dbg->stepMode;

						if (!dbg->stepMode)
							timeAccumulator_ = 0;
					}

					if (dbg->stepMode && event.keyboard.key == Keyboard::F10)
					{
						std::cout << "tick: " << tick_ << std::endl;
						state_->update(dt_);
						tick_++;
					}
				}
			}
		);

		if (quit_) break;
	}
}

void Game::update()
{
	if (quit_) return;

	const Time maxFrameTime = Clock::milliseconds(250);

	Time newTime = Clock::time();
	Time frameTime = newTime - currentTime_;

	DBG(
		fpsTimer_ += frameTime;

		if (fpsTimer_ >= Clock::seconds(1))
		{
			if (dbg->showFPS)
				std::cout << "FPS: " << fps_ << " - Frame time: " << frameTime << std::endl;

			fpsTimer_ -= Clock::seconds(1);
			fps_ = 0;
		}
	);

	if (frameTime > maxFrameTime)
		frameTime = maxFrameTime;

	currentTime_ = newTime;
	timeAccumulator_ += frameTime;

	DBG( if (dbg->stepMode) return; );

	while (timeAccumulator_ >= dt_)
	{
		state_->update(dt_);
		timeAccumulator_ -= dt_;
		tick_++;
	}
}

uint64_t Game::tick() const
{
	return tick_;
}

void Game::quit()
{
	quit_ = true;
	window->close();
}

void Game::state(stt::State *state)
{
	state_ = state;
}
