#include "Server.h"
#include "msg.h"

#include <assert.h>
#include <iostream>

namespace net
{
	Server::Server()
		:	state_(Stopped)
	{
	}

	Server::~Server()
	{
	}

	void Server::start(int port)
	{
		assert(state_ == Stopped);

		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = port;

		connection_ = enet_host_create(&address, MaxPlayers, 2, 0, 0);

		if (connection_ == 0)
		{
			std::cerr << "[Server] Failed to start connection." << std::endl;
			return;
		}

		tick_ = 0;
		state_ = Running;

		loadMap();

		for (size_t i = 0; i < MaxPlayers; i++)
			players_[i].initialize();
	}

	void Server::update(Time dt)
	{
		assert(state_ != Stopped);

		Multiplayer::update(dt);

		tick_++;
	}

	void Server::onConnect(ENetPeer *peer)
	{
	}

	void Server::onDisconnect(ENetPeer *peer)
	{
	}

	void Server::onMessage(msg::Message *msg, ENetPeer *from)
	{
	}
}
