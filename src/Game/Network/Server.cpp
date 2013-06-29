#include "Server.h"
#include "msg.h"

#include <hlp/assign.hpp>
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

	bool Server::start(int port)
	{
		assert(state_ == Stopped);

		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = port;

		connection_ = enet_host_create(&address, MaxPlayers, 2, 0, 0);

		if (connection_ == 0)
		{
			std::cerr << "[Server] Failed to start connection." << std::endl;
			return false;
		}

		tick_ = 0;
		state_ = Running;

		loadMap();

		for (size_t i = 0; i < MaxPlayers; i++)
		{
			players_[i].initialize();
			players_[i].mode(Player::Server);
		}

		return true;
	}

	void Server::update(Time dt)
	{
		assert(state_ != Stopped);

		Multiplayer::update(dt);

		for (size_t i = 0; i < MaxPlayers; i++)
		{
			Player *player = &players_[i];

			if (player->state() != Player::Disconnected)
				player->update(dt, tick_);
		}

		if (tick_ & 1)
		{
			msg::GameState gameState;
			gameState.tick = tick_;
			gameState.nSoldiers = 0;

			for (size_t i = 0; i < MaxPlayers; i++)
			{
				if (players_[i].state() == Player::Playing)
				{
					size_t iSoldier = gameState.nSoldiers;

					// TODO: set soldier state
					gameState.soldiers[iSoldier].playerId = i;

					gameState.nSoldiers++;
				}
			}

			send(&gameState);
		}

		tick_++;
	}

	void Server::onConnect(ENetPeer *peer)
	{
		Player *player = 0;

		for (size_t i = 0; i < MaxPlayers; i++)
		{
			if (players_[i].state() == Player::Disconnected)
			{
				player = &players_[i];
				break;
			}
		}

		assert(player != 0);

		peer->data = player;
		player->onConnecting(peer);
	}

	void Server::onDisconnect(ENetPeer *peer)
	{
		Player *player = (Player*)peer->data;
		player->onDisconnect();

		msg::PlayerDisconnect msg;
		msg.id = player->id();
		send(&msg);
	}

	void Server::onMessage(msg::Message *msg, ENetPeer *from)
	{
		Player *player = (Player*)from->data;

		switch (msg->type())
		{
			case msg::Login::Type: onPlayerLogin(player, msg); break;
			case msg::Ready::Type: onPlayerReady(player, msg); break;
			case msg::Input::Type: onPlayerInput(player, msg); break;

			default: break;
		}
	}

	void Server::onPlayerLogin(Player *player, msg::Message *msg)
	{
		msg::Login *login = (msg::Login*)msg;

		player->onConnect(login->name);

		ENetPeer *peer = player->peer();
		uint8_t id = player->id();

		// send ServerInfo message
		msg::ServerInfo info;
		info.tick = tick_;
		info.clientId = id;
		info.nPlayers = 0;

		for (size_t i = 0; i < MaxPlayers; i++)
			if (i != id && players_[i].connected())
				info.players[info.nPlayers++] = i;

		send(&info, peer);

		// broadcast PlayerConnect event
		msg::PlayerConnect playerConnect;
		playerConnect.id = id;
		hlp::assign(playerConnect.name, login->name);
		send(&playerConnect);

		// send the connecting player a PlayerConnect event for each other connected player
		for (size_t i = 0; i < MaxPlayers; i++)
		{
			if (i != id && players_[i].connected())
			{
				playerConnect.id = i;
				hlp::assign(playerConnect.name, players_[i].name());
				send(&playerConnect, peer);
			}
		}
	}

	void Server::onPlayerReady(Player *player, msg::Message *msg)
	{
		msg::PlayerJoin join;
		join.tick = tick_;
		join.id = player->id();
		join.position = fixvec2(0, 0);

		player->onJoin(tick_, map_, join.position);

		send(&join);
	}

	void Server::onPlayerInput(Player *player, msg::Message *msg)
	{
		msg::Input *msgInput = (msg::Input*)msg;

		cmp::SoldierInput input;
		input.unserialize(msgInput->input);

		player->onInput(msgInput->tick, input);
	}
}
