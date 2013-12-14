#include "Multiplayer.h"
#include "MainMenu.h"

#include <g13/g13.h>
#include <g13/net/Client.h>
#include <g13/net/Server.h>

#include <gfx/gfx.h>
#include <hlp/utf8.h>

namespace g13 {
namespace stt {

Multiplayer::Multiplayer(const string32_t &name, int port)
	:	state_(Connecting),
		client_(0),
		server_(0)
{
	server_ = new net::Server();
	server_->start(port);

	client_ = new net::Client();
	client_->nick(name);
	client_->connect("localhost", port);
}

Multiplayer::Multiplayer(const string32_t &name, const char *host, int port)
	:	state_(Connecting),
		client_(0),
		server_(0)
{
	client_ = new net::Client();
	client_->nick(name);
	client_->connect(host, port);
}

Multiplayer::~Multiplayer()
{
	delete client_;

	if (server_)
		delete server_;
}

void Multiplayer::update(Time dt)
{
	if (server_ != 0)
		server_->update(dt);

	client_->update(dt);

	switch (state_)
	{
		case Connecting:
		{
			if (client_->state() == net::Client::Connected)
				state_ = Connected;

			if (client_->state() == net::Client::Disconnected ||
				(server_ != 0 && server_->state() == net::Server::Stopped))
			{
				client_->disconnect();

				if (server_ != 0)
					server_->stop();

				state_ = Disconnecting;
			}
		}
		break;

		case Connected:
		{
			if (client_->state() == net::Client::Disconnected ||
				(server_ != 0 && server_->state() == net::Server::Stopped))
			{
				client_->disconnect();

				if (server_ != 0)
					server_->stop();

				state_ = Disconnecting;
			}
		}
		break;

		case Disconnecting:
		{
			bool disconnected = client_->state() == net::Client::Disconnected;

			if (server_ != 0)
				disconnected = (disconnected && server_->state() == net::Server::Stopped);

			if (disconnected)
			{
				g13::set_state(new MainMenu());
				delete this;
				return;
			}
		}
		break;

		default: break;
	}
}

void Multiplayer::draw(const Frame &frame)
{
	gfx::clear();
	client_->draw(frame);
}

bool Multiplayer::event(Event *evt)
{
	if (!client_->event(evt))
		return false;

	if (evt->type == Event::KeyPressed)
		return onKeyPressed(evt->key);

	return true;
}

bool Multiplayer::onKeyPressed(const Event::KeyEvent &key)
{
	switch (key.code)
	{
		case sys::Escape:
		{
			if (server_ != 0)
				server_->stop();

			client_->disconnect();

			state_ = Disconnecting;
		}
		break;

		default: break;
	}

	return true;
}

}} // g13::stt
