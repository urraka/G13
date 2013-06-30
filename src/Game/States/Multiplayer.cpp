#include "Multiplayer.h"
#include "../Game.h"
#include "../Network/Client.h"
#include "../Network/Server.h"

namespace stt
{
	Multiplayer::Multiplayer()
		:	client_(0),
			server_(0)
	{
		client_ = new net::Client();
		server_ = new net::Server();
	}

	Multiplayer::~Multiplayer()
	{
		delete client_;
		delete server_;
	}

	void Multiplayer::update(Time dt)
	{
		server_->update(dt);
		client_->update(dt);
	}

	void Multiplayer::draw(float percent)
	{
		client_->draw(percent);
	}

	void Multiplayer::event(const Event &evt)
	{
		if (evt.type == Event::Keyboard && evt.keyboard.pressed)
			onKeyPressed(evt.keyboard.key);

		client_->event(evt);
	}

	void Multiplayer::onKeyPressed(Keyboard::Key key)
	{
		switch (key)
		{
			case Keyboard::S:
			{
				if (server_->state() == net::Server::Stopped)
					server_->start(1234);
				else
					server_->stop();
			}
			break;

			case Keyboard::C:
			{
				if (client_->state() == net::Client::Disconnected)
					client_->connect("localhost", 1234);
			}
			break;

			case Keyboard::Escape:
			{
				if (client_->state() == net::Client::Disconnected && server_->state() == net::Server::Stopped)
				{
					game->quit();
				}
				else
				{
					if (server_->state() == net::Server::Running)
						server_->stop();

					client_->disconnect();
				}
			}
			break;

			default: break;
		}
	}
}
