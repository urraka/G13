#include "Connecting.h"
#include "Testing.h"

#include "../Game.h"
#include "../Network/Client.h"
#include "../../Graphics/Graphics.h"
#include "../../System/Window.h"

namespace stt
{
	stt_name(Connecting);

	Connecting::Connecting()
	{
		client_ = new net::Client();
		client_->connect("localhost", 2345);
	}

	Connecting::~Connecting()
	{
	}

	void Connecting::update(Time dt)
	{
		client_->update();

		if (client_->connected())
		{
			// TODO: retrieve initial data and switch state

			game->state(new stt::Testing(client_));
			delete this;
		}
		else if (!client_->connecting())
		{
			// connection failed

			delete client_;
			game->quit();
		}
	}

	void Connecting::draw(float percent)
	{
		double t = game->tick() * (1.0 / 30.0) + percent * (1.0 / 30.0);
		float value = float(0.5 * std::sin(t * M_PI) + 0.5) * 0.2f;

		game->graphics->bgcolor(0.0f, 0.0f, value, 1.0f);
		game->graphics->clear();
	}

	void Connecting::event(const Event &evt)
	{
		if (evt.type == Event::Keyboard && evt.keyboard.pressed && evt.keyboard.key == Keyboard::Escape)
		{
			delete client_;
			game->quit();

			// TODO: set menu game state
		}
	}
}
