#include "Server.h"
#include "msg.h"

#include <hlp/countof.h>
#include <hlp/assign.h>
#include <assert.h>
#include <iostream>

#define LOG(x) std::cout << "[Server] " << x << std::endl

namespace g13 {
namespace net {

Server::Server() : state_(Stopped) {}

bool Server::start(int port)
{
	assert(state_ == Stopped);

	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;

	connection_ = enet_host_create(&address, MaxPlayers, ChannelsCount, 0, 0);

	if (connection_ == 0)
	{
		LOG("Failed to start connection.");
		return false;
	}

	enet_host_compress_with_range_coder(connection_);

	tick_ = 1;
	state_ = Running;

	loadMap();

	for (int i = 0; i < MaxPlayers; i++)
		players_[i].initialize();

	LOG("Listening on port " << port << "...");

	return true;
}

void Server::stop()
{
	state_ = Stopping;

	LOG("Disconnecting...");

	for (int i = 0; i < MaxPlayers; i++)
	{
		Player *player = &players_[i];

		if (player->state() != Player::Disconnected)
			enet_peer_disconnect(player->peer(), 0);
	}
}

void Server::update(Time dt)
{
	if (state_ == Stopped)
		return;

	pollEvents();

	int activePlayers = 0;

	for (int i = 0; i < MaxPlayers; i++)
	{
		Player *player = &players_[i];

		if (player->state() != Player::Disconnected)
		{
			player->updateServer(dt);
			activePlayers++;
		}
	}

	if (state_ == Stopping && activePlayers == 0)
	{
		// TODO: call some terminate() instead

		LOG("Disconnected.");
		state_ = Stopped;
		enet_host_destroy(connection_);
		connection_ = 0;
		bullets_.clear();
		createdBullets_.clear();

		return;
	}

	updateBullets(dt);

	for (int i = 0; i < MaxPlayers; i++)
	{
		if (players_[i].state() == Player::Disconnected)
			continue;

		msg::Bullet msg;
		msg.tick = tick_;

		int k = 0;

		const int N = createdBullets_.size();

		for (int j = 0; j < N; j++)
		{
			if (createdBullets_[j].playerid == players_[i].id())
				continue;

			assert(tick_ >= players_[i].tick());

			const int offset = tick_ - players_[i].tick();

			msg.bullets[k].tickOffset = std::min<int>(offset, Player::MaxTickOffset);
			msg.bullets[k].params = createdBullets_[j];

			if (++k == countof(msg.bullets) || j == N - 1)
			{
				msg.nBullets = k;
				send(&msg, players_[i].peer());
				k = 0;
			}
		}
	}

	createdBullets_.clear();

	if (tick_ & 1)
	{
		msg::GameState gameState;
		gameState.tick = tick_;
		gameState.nSoldiers = 0;

		for (int i = 0; i < MaxPlayers; i++)
		{
			if (players_[i].state() == Player::Playing)
			{
				int iSoldier = gameState.nSoldiers;
				msg::GameState::SoldierState *s = &gameState.soldiers[iSoldier];

				s->tickOffset = std::min<int>(Player::MaxTickOffset, tick_ - players_[i].tick());
				s->playerId = i;
				s->state = players_[i].soldier()->state();

				gameState.nSoldiers++;
			}
		}

		send(&gameState);
	}

	tick_++;
}

Server::State Server::state() const
{
	return state_;
}

void Server::onConnect(ENetPeer *peer)
{
	if (state_ == Stopping)
	{
		enet_peer_reset(peer);
		return;
	}

	Player *player = 0;

	for (int i = 0; i < MaxPlayers; i++)
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

	LOG("Player #" << (int)player->id() << " connecting...");
}

void Server::onDisconnect(ENetPeer *peer)
{
	Player *player = (Player*)peer->data;
	player->onDisconnect(tick_);

	msg::PlayerDisconnect msg;
	msg.id = player->id();
	send(&msg);

	LOG("Player #" << (int)player->id() << " disconnected.");
}

void Server::onMessage(msg::Message *msg, ENetPeer *from)
{
	Player *player = (Player*)from->data;

	switch (msg->type())
	{
		case msg::Login::Type: onPlayerLogin(player, (msg::Login*)msg); break;
		case msg::Ready::Type: onPlayerReady(player, (msg::Ready*)msg); break;
		case msg::Input::Type: onPlayerInput(player, (msg::Input*)msg); break;
		case msg::Chat::Type:  onPlayerChat (player, (msg::Chat*) msg); break;

		default: break;
	}
}

void Server::onPlayerLogin(Player *player, msg::Login *login)
{
	player->onConnect(login->name);

	ENetPeer *peer = player->peer();
	uint8_t id = player->id();

	// send ServerInfo message
	msg::ServerInfo info;
	info.tick = tick_;
	info.clientId = id;
	info.nPlayers = 0;

	for (int i = 0; i < MaxPlayers; i++)
		if (i != id && players_[i].connected())
			info.players[info.nPlayers++] = i;

	send(&info, peer);

	// broadcast PlayerConnect event
	msg::PlayerConnect playerConnect;
	playerConnect.id = id;
	hlp::assign(playerConnect.name, login->name);
	send(&playerConnect);

	// send the connecting player a PlayerConnect event for each other connected player
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (i != id && players_[i].connected())
		{
			playerConnect.id = i;
			hlp::assign(playerConnect.name, players_[i].name());
			send(&playerConnect, peer);
		}
	}

	LOG("Player #" << (int)player->id() << " connected. Name: " << player->name());
}

void Server::onPlayerReady(Player *player, msg::Ready *ready)
{
	msg::PlayerJoin join;
	join.tick = tick_;
	join.id = player->id();
	join.position = fixvec2(0, 0);

	player->onJoin(tick_, map_, join.position);

	send(&join);
}

void Server::onPlayerInput(Player *player, msg::Input *input)
{
	if (input->tick > tick_)
	{
		#ifdef DEBUG
			int t = input->tick;
			int id = (int)player->id();

			debug_log("input->tick > tick (" << t << " > " << tick_ << ") for player #" << id);
		#endif

		return;
	}

	cmp::SoldierInput soldierInput;

	soldierInput.rightwards = input->rightwards;
	soldierInput.angle      = input->angle;
	soldierInput.left       = input->left;
	soldierInput.right      = input->right;
	soldierInput.jump       = input->jump;
	soldierInput.run        = input->run;
	soldierInput.duck       = input->duck;
	soldierInput.shoot      = input->shoot;

	player->onInput(input->tick, soldierInput);
}

void Server::onPlayerChat(Player *player, msg::Chat *chat)
{
	LOG(player->name() << ": " << chat->text);

	chat->id = player->id();

	for (int i = 0; i < chat->id; i++)
	{
		if (players_[i].connected())
			send(chat, players_[i].peer());
	}

	for (int i = chat->id + 1; i < MaxPlayers; i++)
	{
		if (players_[i].connected())
			send(chat, players_[i].peer());
	}
}

void Server::onBulletCreated(const cmp::BulletParams &params)
{
	createdBullets_.push_back(cmp::BulletParams(params));
}

}} // g13::net
