#include "Server.h"
#include "msg.h"

#include <g13/Map.h>
#include <g13/callback.h>
#include <hlp/countof.h>
#include <hlp/assign.h>
#include <vector>
#include <assert.h>
#include <iostream>

#define LOG(x) std::cout << "[Server] " << x << std::endl

namespace g13 {
namespace net {

Server::Server() : state_(Stopped)
{
	for (int i = 0; i < MaxPlayers; i++)
		players_[i].soldier()->createBulletCallback = make_callback(this, Server, createBullet);
}

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
	if (state_ != Running)
		return;

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
			player->updateServer(dt, tick_);
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
		createdBullets_.clear();

		return;
	}

	for (int i = 0; i < MaxPlayers; i++)
	{
		if (!players_[i].connected())
			continue;

		int initialRTT = players_[i].pongTick - players_[i].pingTick;
		int currentRTT = players_[i].peer()->roundTripTime / (int)sys::to_milliseconds(dt);

		int lag = std::max(InterpolationTicks + initialRTT / 2, currentRTT / 2);

		map_->world()->clear();

		for (int j = 0; j < MaxPlayers; j++)
		{
			if (players_[j].state() != Player::Playing)
				continue;

			players_[j].setCollisionTick(tick_ - (i != j ? lag : 0));
			map_->world()->add(&(players_[j].soldier()->collisionEntity));
		}

		players_[i].updateBullets(dt);
	}

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
			if (createdBullets_[j].data.playerid == players_[i].id())
				continue;

			assert(tick_ >= players_[i].tick());

			const int offset = tick_ - createdBullets_[j].tick;

			msg.bullets[k].tickOffset = std::min<int>(offset, Player::MaxTickOffset);
			msg.bullets[k].params = createdBullets_[j].data;

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

	#if defined(DEBUG) && 0
		if (tick_ % 3 == 0)
		{
			for (int i = 0; i < MaxPlayers; i++)
			{
				if (players_[i].peer() == 0)
					continue;

				LOG("Player #" << i << " roundTripTime: " << players_[i].peer()->roundTripTime);
				LOG("Player #" << i << " lastRoundTripTime: " << players_[i].peer()->lastRoundTripTime);
			}
		}
	#endif

	if (connection_ != 0)
		enet_host_flush(connection_);

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
		case msg::Pong::Type:  onPlayerPong (player, (msg::Pong*) msg); break;
		case msg::Ready::Type: onPlayerReady(player, (msg::Ready*)msg); break;
		case msg::Input::Type: onPlayerInput(player, (msg::Input*)msg); break;
		case msg::Chat::Type:  onPlayerChat (player, (msg::Chat*) msg); break;

		default: break;
	}
}

void Server::onPlayerLogin(Player *player, msg::Login *login)
{
	player->onConnect(login->name, gfx::Color(login->color[0], login->color[1], login->color[2]));

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

	player->pingTick = tick_;

	send(&info, peer);
	enet_host_flush(connection_);

	// broadcast PlayerConnect event
	msg::PlayerConnect playerConnect;
	playerConnect.id = id;
	playerConnect.color[0] = login->color[0];
	playerConnect.color[1] = login->color[1];
	playerConnect.color[2] = login->color[2];
	hlp::assign(playerConnect.name, login->name);
	send(&playerConnect);

	// send the connecting player a PlayerConnect event for each other connected player
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (i != id && players_[i].connected())
		{
			playerConnect.id = i;
			playerConnect.color[0] = players_[i].soldier()->graphics.bodyColor.r;
			playerConnect.color[1] = players_[i].soldier()->graphics.bodyColor.g;
			playerConnect.color[2] = players_[i].soldier()->graphics.bodyColor.b;
			hlp::assign(playerConnect.name, players_[i].name());
			send(&playerConnect, peer);
		}
	}

	LOG("Player #" << (int)player->id() << " connected. Name: " << player->name());
}

void Server::onPlayerPong(Player *player, msg::Pong *pong)
{
	if (player->pongTick == -1)
		player->pongTick = tick_;
}

void Server::onPlayerReady(Player *player, msg::Ready *ready)
{
	msg::PlayerJoin join;
	const std::vector<fixvec2> &spawnpoints = map_->world()->spawnpoints();

	join.tick = tick_;
	join.id = player->id();
	join.position = spawnpoints[rand() % spawnpoints.size()];

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

		// return;
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

void Server::createBullet(void *data)
{
	const cmp::BulletParams &params = *(cmp::BulletParams*)data;
	Player *player = &players_[params.playerid];

	player->createBullet(params, make_callback(this, Server, playerBulletCollision));
	createdBullets_.push_back(BulletParams(player->tick(), params));
}

void Server::playerBulletCollision(void *data)
{
	struct params_t
	{
		uint8_t bulletOwner;
		const coll::Entity *entity;
	};

	params_t *params = (params_t*)data;
	Player   *victim = (Player*)params->entity->data;

	uint16_t amount = Player::MaxHealth / 20;

	victim->onDamage(tick_, amount);

	msg::Damage damage;

	damage.tick = tick_;
	damage.playerId = victim->id();
	damage.amount = amount;

	send(&damage);
}

}} // g13::net
