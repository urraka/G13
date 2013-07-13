#include "Multiplayer.h"
#include "../Game.h"
#include "../Network/Client.h"
#include "../Network/Server.h"
#include "../Debugger.h"

#include <hlp/read.h>
#include <hlp/split.h>
#include <hlp/to_int.h>

namespace stt {

static void read_settings(std::string *host, int *port)
{
	hlp::strvector elements = hlp::split(hlp::read("data/network.txt"), ':');

	*host = "";
	*port = 0;

	if (elements.size() == 2)
	{
		*host = elements[0];
		*port = hlp::to_int(elements[1]);
	}
}

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

void Multiplayer::update(sys::Time dt)
{
	server_->update(dt);
	client_->update(dt);
}

void Multiplayer::draw(float percent)
{
	client_->draw(percent);
}

void Multiplayer::event(sys::Event *evt)
{
	if (evt->type == sys::Keyboard)
	{
		sys::KeyboardEvent *keyboard = (sys::KeyboardEvent*)evt;

		if (keyboard->pressed)
			onKeyPressed(keyboard->key);
	}

	client_->event(evt);
}

void Multiplayer::onKeyPressed(int key)
{
	switch (key)
	{
		case 'S':
		{
			if (server_->state() == net::Server::Stopped)
			{
				int port;
				std::string host;
				read_settings(&host, &port);

				server_->start(port);
			}
			else
				server_->stop();
		}
		break;

		case 'C':
		{
			if (client_->state() == net::Client::Disconnected)
			{
				int port;
				std::string host;
				read_settings(&host, &port);

				client_->connect(host.c_str(), port);
			}
		}
		break;

		case sys::Escape:
		{
			if (client_->state() == net::Client::Disconnected && server_->state() == net::Server::Stopped)
			{
				sys::exit();
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

} // stt
