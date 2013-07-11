#include "Testing.h"

#include "../Game.h"
#include "../Network/Client.h"
#include "../../System/Keyboard.h"
#include "../../System/Window.h"
#include "../Debugger.h"

#include <gfx/gfx.h>

namespace stt {

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

	sprites_ = new gfx::SpriteBatch(1);
	textures_[TextureGuy] = new gfx::Texture("data/guy.png");

	int width, height;
	game->window->size(width, height);

	background_ = new gfx::VBO();
	background_->allocate<gfx::ColorVertex>(4, gfx::Static);
	background_->mode(gfx::TriangleFan);

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
		client_->update(dt);

	replayLog_.update(&replay_, &soldier_);

	if (Keyboard::pressed(Keyboard::NumpadAdd))
		camera_.zoom(ent::Camera::ZoomIn);

	if (Keyboard::pressed(Keyboard::NumpadSubtract))
		camera_.zoom(ent::Camera::ZoomOut);

	if (replay_.state() == Replay::Playing)
	{
		cmp::SoldierInput input = replay_.input();
		soldier_.update(dt, &input);
	}
	else
	{
		soldier_.update(dt);
	}

	camera_.update(dt);

	if (replay_.state() == Replay::Recording)
		replay_.input(&soldier_.input);
}

void Testing::draw(float framePercent)
{
	soldier_.graphics.frame(framePercent);

	gfx::clear();
	gfx::matrix(mat4(1.0f));
	gfx::draw(background_);

	gfx::matrix(camera_.matrix(framePercent));

	map_.draw();
	DBG( dbg->drawCollisionHulls(); );

	sprites_->clear();
	sprites_->texture(textures_[TextureGuy]);
	sprites_->add(soldier_.graphics.sprite);

	gfx::draw(sprites_);
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
			}

			break;
		}

		default:
			break;
	}
}

void Testing::updateBackground(int width, int height)
{
	gfx::ColorVertex vertex[4];

	float w = (float)width;
	float h = (float)height;

	vertex[0] = gfx::color_vertex(0.0f, 0.0f, 0, 0, 255, 255);
	vertex[1] = gfx::color_vertex(w, 0.0f, 0, 0, 255, 255);
	vertex[2] = gfx::color_vertex(w, h, 255, 255, 255, 255);
	vertex[3] = gfx::color_vertex(0.0f, h, 255, 255, 255, 255);

	background_->set(vertex, 0, 4);
}

} // stt
