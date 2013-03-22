#include <pch.h>
#include <Graphics/Graphics.h>
#include <System/Window.h>
#include <System/Events.h>
#include <System/Clock.h>
#include <Game/Game.h>

#include <string>
#include <iostream>
#include <fstream>

Game::Game()
	:	graphics(0),
		events(0),
		window_(0),
		texture_(0),
		buffer_(0),
		batch_(0)
{
}

bool Game::init()
{
	quit_ = false;
	timeAccumulator_ = 0;
	currentTime_ = Clock::time();
	dt_ = Clock::milliseconds(30);
	time_ = 0;
	fpsTimer_ = 0;
	fps_ = 0;

	events = new Events();
	graphics = new Graphics();

	// iOS already has the view and OpenGL context created by now
	#if !defined(IOS)
		window_ = new Window();

		if (!window_->init(false))
			return false;

		window_->title("G13");
		window_->vsync(false);

		resolution = window_->size();
	#endif

	events->init();

	if (!graphics->init())
		return false;

	graphics->viewport(resolution.x, resolution.y);
	graphics->bgcolor(0.5f, 0.5f, 0.5f);

	texture_ = graphics->texture("data/tree.png");
	// buffer_ = graphics->buffer(VertexBuffer::kTriangleFan, VertexBuffer::kStaticDraw, VertexBuffer::kStaticDraw, 4, 0);

	// Vertex vert[4];

	// vert[0].position = vec2(100.0f, 100.0f);
	// vert[1].position = vec2(356.0f, 100.0f);
	// vert[2].position = vec2(356.0f, 356.0f);
	// vert[3].position = vec2(100.0f, 356.0f);
	// vert[0].uv = vec2(0.0f, 0.0f);
	// vert[1].uv = vec2(1.0f, 0.0f);
	// vert[2].uv = vec2(1.0f, 1.0f);
	// vert[3].uv = vec2(0.0f, 1.0f);

	// uint16_t indices[6] = { 0, 1, 2, 2, 3, 0 };

	// buffer_->set(vert, 0, 4);
	//buffer_->set(indices, 0, 6);

	const size_t nSprites = 100;
	batch_ = graphics->batch(nSprites);
	sprites_.resize(nSprites);
	spriteAngles_.resize(nSprites);

	for (size_t i = 0; i < sprites_.size(); i++)
	{
		Sprite &sprite = sprites_[i];

		sprite.texture = texture_;
		sprite.texcoords = vec4(0.0f, 0.0f, 1.0f, 1.0f);
		sprite.position = glm::diskRand(glm::min((float)resolution.x, (float)resolution.y) / 2.0f);
		sprite.center = vec2(168.0f, 252.0f);
		sprite.size = vec2(256.0f);
		sprite.scale = vec2(0.5f, 0.5f);
		spriteAngles_[i] = glm::linearRand(-180.0f, 180.0f);
	}

	return true;
}

void Game::terminate()
{
	delete buffer_;
	delete texture_;
	delete graphics;
	delete events;

	#if !defined(IOS)
		delete window_;
	#endif

	graphics = 0;
	events = 0;
	window_ = 0;
}

void Game::draw()
{
	const float t = Clock::toSeconds<float>(time_ + timeAccumulator_);
	const float scale = 0.5f * glm::exp(1.5f * glm::sin(t));

	graphics->save();
	graphics->clear();
	graphics->translate(resolution.x / 2.0f, resolution.y / 2.0f);
	graphics->scale(scale, scale);

	batch_->clear();

	for (size_t i = 0; i < sprites_.size(); i++)
	{
		sprites_[i].angle = spriteAngles_[i] + t * 90.0f;
		batch_->add(sprites_[i]);
	}

	graphics->draw(batch_);
	graphics->restore();

	fps_++;
}

void Game::input()
{
	Event event;

	while (events->poll(&event))
	{
		switch (event.type)
		{
			case Event::Close:
				this->quit();
				break;

			case Event::Resize:
				resolution = ivec2(event.size.width, event.size.height);
				graphics->viewport(resolution.x, resolution.y);
				break;

			case Event::KeyPress:
				if (event.key == Keyboard::Escape)
					this->quit();

				break;

			default:
				break;
		}
	}
}

void Game::update()
{
	const uint64_t maxFrameTime = Clock::milliseconds(250);

	uint64_t newTime = Clock::time();
	uint64_t frameTime = newTime - currentTime_;

	fpsTimer_ += frameTime;

	if (fpsTimer_ >= Clock::seconds(1))
	{
		std::cout << "FPS: " << fps_ << " - Frame time: " << frameTime << std::endl;

		fpsTimer_ -= Clock::seconds(1);
		fps_ = 0;
	}

	if (frameTime > maxFrameTime)
		frameTime = maxFrameTime;

	currentTime_ = newTime;
	timeAccumulator_ += frameTime;

	while (timeAccumulator_ >= dt_)
	{
		// TODO: Update logic

		time_ += dt_;
		timeAccumulator_ -= dt_;
	}

	// TODO: interpolate
}

void Game::quit()
{
	quit_ = true;
}

#if !defined(IOS)
	// no loop in iOS, instead draw, input, update will be called by a callback hooked up in ios.mm
	void Game::loop()
	{
		while (!quit_)
		{
			this->draw();
			this->input();
			this->update();
			window_->display();
		}
	}
#endif
