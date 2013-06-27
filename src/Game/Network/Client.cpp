#include "Client.h"
#include "msg.h"

#include "../Debugger.h"

#include <hlp/assign.hpp>
#include <assert.h>
#include <iostream>

namespace net
{
	Client::Client()
		:	state_(Disconnected),
			peer_(0),
			id_(-1)
	{
		hlp::assign(name_, "player");
	}

	Client::~Client()
	{
	}

	bool Client::connect(const char *host, int port)
	{
		assert(connection_ == 0 && peer_ == 0);

		connection_ = enet_host_create(0, 1, ChannelsCount, 0, 0);

		if (connection_ == 0)
		{
			std::cerr << "[Client] enet_host_create failed." << std::endl;
			return false;
		}

		ENetAddress address;
		enet_address_set_host(&address, host);
		address.port = port;

		peer_ = enet_host_connect(connection_, &address, ChannelsCount, 0);

		if (peer_ == 0)
		{
			std::cerr << "[Client] enet_host_connect failed." << std::endl;
			return false;
		}

		enet_peer_timeout(peer_, 0, 4000, 5000);

		state_ = Connecting;

		DBG( std::cout << "[Client] Connecting to " << host << ":" << port << "..." << std::endl );

		return true;
	}

	void Client::update(Time dt)
	{
		Multiplayer::update(dt);

		if (active())
		{
			input_.update();
			players_[id_].onInput(tick_, input_);
			players_[id_].update(dt, tick_);

			msg::Input inputMsg;
			inputMsg.tick = tick_;
			inputMsg.input = input_.serialize();

			for (size_t i = 0; i < MaxPlayers; i++)
			{
				Player *player = &players_[i];

				if (i == id_ || player->state() != Player::Playing)
					continue;

				player->update(dt, tick_);
			}
		}

		tick_++;
	}

	bool Client::active() const
	{
		return state_ == Connected && id_ != -1 && players_[id_].connected();
	}

	void Client::onConnect(ENetPeer *peer)
	{
		state_ = Connected;

		msg::Login login;
		hlp::assign(login.name, name_);

		send(&login, peer);
	}

	void Client::onDisconnect(ENetPeer *peer)
	{
		DBG(
			if (state_ == Connecting)
				std::cout << "[Client] Connection failed." << std::endl;
			else
				std::cout << "[Client] Disconnected from host." << std::endl;
		);

		state_ = Disconnected;
		id_ = -1;
	}

	void Client::onMessage(msg::Message *msg, ENetPeer *from)
	{
		switch (msg->type())
		{
			case msg::ServerInfo::Type:       onServerInfo(msg);       break;
			case msg::PlayerConnect::Type:    onPlayerConnect(msg);    break;
			case msg::PlayerDisconnect::Type: onPlayerDisconnect(msg); break;
			case msg::PlayerJoin::Type:       onPlayerJoin(msg);       break;
			case msg::GameState::Type:        onGameState(msg);        break;
			default: break;
		}
	}

	void Client::onServerInfo(msg::Message *msg)
	{
		msg::ServerInfo *info = (msg::ServerInfo*)msg;

		id_ = info->clientId;
		tick_ = info->tick;

		for (size_t i = 0; i < MaxPlayers; i++)
			players_[i].initialize();

		connectingCount_ = 0;

		for (size_t i = 0; i < info->nPlayers; i++)
		{
			uint8_t id = info->players[i];
			players_[id].onConnecting();

			connectingCount_++;
		}

		players_[id_].onConnecting();
	}

	void Client::onPlayerConnect(msg::Message *msg)
	{
		msg::PlayerConnect *playerConnect = (msg::PlayerConnect*)msg;
		Player *player = &players_[playerConnect->id];

		if (player->connected())
			player->onDisconnect();

		if (player->state() == Player::Connecting)
		{
			player->onConnect(playerConnect->name);

			if (--connectingCount_ == 0)
			{
				players_[id_].onConnect(name_);

				msg::Ready ready;
				send(&ready, peer_);
			}
		}
		else
		{
			player->onConnect(playerConnect->name);
		}
	}

	void Client::onPlayerDisconnect(msg::Message *msg)
	{
		msg::PlayerDisconnect *playerDisconnect = (msg::PlayerDisconnect*)msg;
		players_[playerDisconnect->id].onDisconnect();
	}

	void Client::onPlayerJoin(msg::Message *msg)
	{
		msg::PlayerJoin *playerJoin = (msg::PlayerJoin*)msg;
		players_[playerJoin->id].onJoin(playerJoin->tick, map_, playerJoin->position);
	}

	void Client::onGameState(msg::Message *msg)
	{
		msg::GameState *gameState = (msg::GameState*)msg;

		for (size_t i = 0; i < gameState->nSoldiers; i++)
		{
			Player *player = &players_[gameState->soldiers[i].playerId];

			if (active() && player->state() == Player::Connected)
				player->onJoin(tick_, map_, gameState->soldiers[i].position);

			player->onSoldierState(gameState->tick, gameState->soldiers[i]);
		}
	}
}
