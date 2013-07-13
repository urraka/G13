#include "Game.h"
#include "States/Testing.h"
#include "States/Multiplayer.h"
#include "Debugger.h"

#include <sys/sys.h>
#include <gfx/gfx.h>
#include <enet/enet.h>
#include <iostream>

Game::Game()
	:	state_(0),
		currentTime_(0),
		timeAccumulator_(0),
		dt_(0),
		fpsTimer_(0),
		fps_(0),
		tick_(0)
{
}

Game::~Game()
{
	delete state_;

	gfx::terminate();
	enet_deinitialize();
}

void Game::initialize()
{
	sys::samples(4);
	sys::fullscreen(false);
	sys::window_title("G13");
	sys::window_size(0.45f, 0.45f);
	sys::window_position(0.5f, 0.5f);
	sys::vsync(1);

	sys::initialize();
	gfx::initialize();

	currentTime_ = sys::time();
	dt_ = sys::time<sys::Milliseconds>(30);

	int width;
	int height;

	sys::window_size(&width, &height);
	gfx::viewport(width, height, sys::window_rotation());

	if (enet_initialize() != 0)
		std::cerr << "Failed to initialize enet." << std::endl;

	state_ = new stt::Multiplayer();
}

void Game::draw()
{
	if (sys::exiting())
		return;

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
	while (sys::Event *event = sys::poll_events())
	{
		state_->event(event);

		switch (event->type)
		{
			case sys::Resize:
			{
				sys::ResizeEvent *resize = (sys::ResizeEvent*)event;
				gfx::viewport(resize->width, resize->height, resize->rotation);
			}
			break;

			case sys::Keyboard:
			{
				sys::KeyboardEvent *keyboard = (sys::KeyboardEvent*)event;

				if (keyboard->pressed)
					dbg->onKeyPressed(keyboard->key);
			}
			break;

			default: break;
		}

		if (sys::exiting())
			break;

		// step mode stuff, should redesign for multiplayer
		//
		// DBG(
		// 	if (event.type == Event::Keyboard)
		// 	{
		// 		if (event.keyboard.pressed)
		// 		{
		// 			if (event.keyboard.key == Keyboard::F5)
		// 			{
		// 				dbg->stepMode = !dbg->stepMode;

		// 				if (!dbg->stepMode)
		// 					timeAccumulator_ = 0;
		// 			}

		// 			if (dbg->stepMode && event.keyboard.key == Keyboard::F10)
		// 			{
		// 				std::cout << "tick: " << tick_ << std::endl;
		// 				state_->update(dt_);
		// 				tick_++;
		// 			}
		// 		}
		// 	}
		// );
	}
}

void Game::update()
{
	if (sys::exiting())
		return;

	const sys::Time maxFrameTime = sys::time<sys::Milliseconds>(250);

	sys::Time newTime = sys::time();
	sys::Time frameTime = newTime - currentTime_;

	#ifdef DEBUG
		fpsTimer_ += frameTime;

		const sys::Time sec = sys::time<sys::Seconds>(1);

		if (fpsTimer_ >= sec)
		{
			if (dbg->showFPS)
				std::cout << "FPS: " << fps_ << " - Frame time: " << frameTime << std::endl;

			fpsTimer_ -= sec;
			fps_ = 0;
		}
	#endif

	sys::Time frozenTime = 0;

	if (frameTime > maxFrameTime)
	{
		frozenTime = frameTime - maxFrameTime;
		frameTime = maxFrameTime;
	}

	currentTime_ = newTime - frozenTime;
	timeAccumulator_ += frameTime;

	#ifdef DEBUG
		if (dbg->stepMode) return;
	#endif

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
	sys::exit();
}

void Game::state(stt::State *state)
{
	state_ = state;
}
