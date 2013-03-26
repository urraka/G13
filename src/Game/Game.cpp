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
		texture_(),
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

		if (!window_->init(true))
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
	texture_[0] = graphics->texture("data/tree.png");
	texture_[1] = graphics->texture("data/white-tree.png");

	const size_t nSprites = 1000;

	batch_ = graphics->batch(nSprites);
	sprites_.resize(nSprites);
	spriteAngles_.resize(nSprites);

	for (size_t i = 0; i < sprites_.size(); i++)
	{
		Sprite &sprite = sprites_[i];

		sprite.texcoords = vec4(0.0f, 0.0f, 1.0f, 1.0f);
		sprite.position = glm::diskRand(glm::min((float)resolution.x, (float)resolution.y) / 2.0f);
		sprite.center = vec2(168.0f, 252.0f);
		sprite.size = vec2((float)texture_[0]->width(), (float)texture_[0]->height());
		sprite.scale = vec2(0.2f, 0.2f);
		spriteAngles_[i] = glm::linearRand(-180.0f, 180.0f);
	}

	ColorVertex vertices[8];

	vertices[0].position = vec2(-128.0f, -128.0f);
	vertices[1].position = vec2(128.0f, -128.0f);
	vertices[2].position = vec2(128.0f, 128.0f);
	vertices[3].position = vec2(-128.0f, 128.0f);

	vertices[0].color = vec4(0.3f, 0.3f, 1.0f, 1.0f);
	vertices[1].color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[2].color = vec4(0.0f, 0.0f, 0.3f, 1.0f);
	vertices[3].color = vec4(0.0f, 0.0f, 0.3f, 1.0f);

	vertices[0].uv = vec2(0.0f, 0.0f);
	vertices[1].uv = vec2(1.0f, 0.0f);
	vertices[2].uv = vec2(1.0f, 1.0f);
	vertices[3].uv = vec2(0.0f, 1.0f);

	vertices[4].position = vec2(0.0f, 0.0f);
	vertices[5].position = vec2((float)resolution.x, 0.0f);
	vertices[6].position = vec2((float)resolution.x, (float)resolution.y);
	vertices[7].position = vec2(0.0f, (float)resolution.y);

	vertices[4].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[5].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[6].color = vec4(1.0f);
	vertices[7].color = vec4(1.0f);

	buffer_ = graphics->buffer<ColorVertex>(VBO<ColorVertex>::TriangleFan, VBO<ColorVertex>::StaticDraw, 8);
	buffer_->set(vertices, 0, 8);

	return true;
}

void Game::terminate()
{
	delete buffer_;
	delete texture_[0];
	delete texture_[1];
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
	const float scale = glm::exp(1.0f * glm::sin(t));

	graphics->clear();

	graphics->bind((Texture*)0);
	graphics->bind(Graphics::ColorShader);
	graphics->draw(buffer_, 4, 4);

	graphics->save();
	graphics->translate(resolution.x / 2.0f, resolution.y / 2.0f);
	graphics->scale(scale, scale);
	graphics->rotate(t * 45.0f);

	batch_->clear();

	for (size_t i = 0; i < sprites_.size(); i++)
	{
		sprites_[i].angle = spriteAngles_[i] + t * 90.0f;
		batch_->add(sprites_[i]);
	}

	graphics->bind(Graphics::TextureShader);
	graphics->bind(texture_[0]);
	graphics->draw(batch_);
	graphics->restore();

	graphics->save();
	graphics->translate(resolution.x - 128.0f, resolution.y - 128.0f);
	graphics->bind(Graphics::ColorShader);
	graphics->bind(texture_[1]);
	graphics->draw(buffer_, 0, 4);
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
			{
				resolution = ivec2(event.size.width, event.size.height);
				graphics->viewport(resolution.x, resolution.y);

				ColorVertex vertices[4];

				vertices[0].position = vec2(0.0f, 0.0f);
				vertices[1].position = vec2((float)resolution.x, 0.0f);
				vertices[2].position = vec2((float)resolution.x, (float)resolution.y);
				vertices[3].position = vec2(0.0f, (float)resolution.y);

				vertices[0].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
				vertices[1].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
				vertices[2].color = vec4(1.0f);
				vertices[3].color = vec4(1.0f);

				buffer_->set(vertices, 4, 4);

				break;
			}

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
