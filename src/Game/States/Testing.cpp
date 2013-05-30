#include "Testing.h"

#include "../Game.h"
#include "../Network/Client.h"
#include "../../System/Keyboard.h"
#include "../../System/Window.h"
#include "../Debugger.h"

namespace stt
{
	stt_name(Testing);

	Testing::Testing(net::Client *client)
		:	background_(0),
			textures_(),
			sprites_(0),
			client_(client)
	{
		DBG(
			dbg->map = &map_;
			dbg->soldier = &soldier_;
		);

		Graphics *graphics = game->graphics;

		sprites_ = graphics->batch(1);
		textures_[TextureGuy] = graphics->texture("data/guy.png");
		textures_[TextureTree] = graphics->texture("data/tree.png");

		int width, height;
		game->window->size(width, height);
		background_ = graphics->buffer<ColorVertex>(vbo_t::TriangleFan, vbo_t::StaticDraw, 4);
		updateBackground(width, height);

		map_.load();

		DBG( dbg->loadCollisionHulls(); );

		soldier_.map(map_.collisionMap());
		soldier_.reset(fixvec2(150, -500));

		camera_.target(&soldier_.graphics.position.current);
		camera_.viewport(width, height);
	}

	Testing::~Testing()
	{
		if (client_)
			delete client_;

		delete background_;
		delete sprites_;

		for (int i = 0; i < TextureCount; i++)
			delete textures_[i];
	}

	void Testing::update(Time dt)
	{
		if (client_)
			client_->update();

		replayLog_.update(&replay_, &soldier_);

		if (Keyboard::pressed(Keyboard::NumpadAdd))
			camera_.zoom(ent::Camera::ZoomIn);

		if (Keyboard::pressed(Keyboard::NumpadSubtract))
			camera_.zoom(ent::Camera::ZoomOut);

		soldier_.update(dt, &replay_);
		camera_.update(dt);

		if (replay_.state() == Replay::Recording)
			replay_.input(&soldier_.input);
	}

	void Testing::draw(float framePercent)
	{
		soldier_.graphics.frame(framePercent);

		Graphics *graphics = game->graphics;

		graphics->clear();

		graphics->bind(Graphics::ColorShader);
		graphics->draw(background_);

		graphics->save();
		graphics->matrix(camera_.matrix(framePercent));

		map_.draw(graphics);

		DBG( dbg->drawCollisionHulls(); );

		sprites_->clear();
		sprites_->texture(textures_[TextureGuy]);
		sprites_->add(soldier_.graphics.sprite);

		graphics->draw(sprites_);

		graphics->restore();
	}

	void Testing::event(const Event &evt)
	{
		switch (evt.type)
		{
			case Event::Resize:
			{
				updateBackground(evt.resize.width, evt.resize.height);
				camera_.viewport(evt.resize.width, evt.resize.height);
				break;
			}

			case Event::Keyboard:
			{
				if (evt.keyboard.pressed)
				{
					switch (evt.keyboard.key)
					{
						case Keyboard::Escape:
						{
							game->quit();
						}
						break;

						case Keyboard::F11:
						{
							if (replay_.state() == Replay::Idle)
								replay_.startRecording(&soldier_);
							else if (replay_.state() == Replay::Recording)
								replay_.stopRecording("G13.replay");
						}
						break;

						case Keyboard::F12:
						{
							if (replay_.state() == Replay::Idle)
							{
								replay_.play("G13.replay", &soldier_);
								camera_.target(&soldier_.graphics.position.current);
							}
							else if (replay_.state() == Replay::Playing)
							{
								replay_.stop();
								replayLog_.save("replay.log");
							}
						}
						break;

						default:
							break;
					}

					DBG(
						switch (evt.keyboard.key)
						{
							case Keyboard::H:
								dbg->showCollisionHulls = !dbg->showCollisionHulls;
								break;

							case Keyboard::C:
								dbg->showCollisionData();
								break;

							case Keyboard::W:
								dbg->wireframe = !dbg->wireframe;
								break;

							default:
								break;
						}
					);
				}

				break;
			}

			default:
				break;
		}
	}

	void Testing::updateBackground(int width, int height)
	{
		ColorVertex vertices[4];

		vertices[0].position = vec2(0.0f, 0.0f);
		vertices[1].position = vec2((float)width, 0.0f);
		vertices[2].position = vec2((float)width, (float)height);
		vertices[3].position = vec2(0.0f, (float)height);

		vertices[0].color = u8vec4(0, 0, 255, 255);
		vertices[1].color = u8vec4(0, 0, 255, 255);
		vertices[2].color = u8vec4(255, 255, 255, 255);
		vertices[3].color = u8vec4(255, 255, 255, 255);

		background_->set(vertices, 0, 4);
	}
}
