#include "Server.h"
#include "msg/msg.h"
#include <hlp/read.h>
#include <hlp/assign.h>
#include <json/json.h>
#include <assert.h>
#include <algorithm>

namespace g13 {
namespace net {

//******************************************************************************
// General
//******************************************************************************

bool Server::start(int port)
{
	initialize();

	if (!Connection::listen(port, ChannelCount, MaxPlayers))
		return false;

	state_ = Running;

	return false;
}

void Server::stop()
{
	Connection::stop();

	state_ = Stopping;
}

void Server::initialize()
{
	Connection::reset();

	tick_ = 0;
	state_ = Stopped;

	loadMap(0);

	players_.clear();
	freePlayers_.clear();
	connectingPlayers_.clear();
	disconnectingPlayers_.clear();

	for (size_t i = 0; i < countof(playersStorage_); i++)
	{
		playersStorage_[i].initialize();
		playersStorage_[i].id = i;
		playersStorage_[i].soldier.id = i;
		playersStorage_[i].soldier.createBulletCallback = make_callback(this, Server, onSpawnBullet);

		freePlayers_.push_back(&playersStorage_[i]);
	}

	bulletQueue_.clear();

	matchPlaying_ = false;
	matchStartTick_ = 0;
	matchEndTick_ = 0;
}

void Server::loadMap(const char *name)
{
	Json::Value data;
	Json::Reader json(Json::Features::strictMode());
	json.parse(hlp::read("data/map_test2.json"), data);

	world_.load(data);
}

void Server::startMatch()
{
	matchPlaying_ = true;
	matchStartTick_ = tick_;

	bulletQueue_.clear();

	for (size_t i = 0; i < players_.size(); i++)
	{
		ServerPlayer *player = players_[i];

		player->state = Player::Spectator;
		player->bullets.clear();
	}

	for (size_t i = 0; i < disconnectingPlayers_.size(); i++)
		disconnectingPlayers_[i]->disconnectCountdown = 1;

	sendMatchStartMessage();
}

void Server::endMatch()
{
	matchPlaying_ = false;
	matchEndTick_ = tick_;
	sendMatchEndMessage();
}

ServerPlayer *Server::getPlayerById(int id)
{
	assert(id >= 0 && id < (int)countof(playersStorage_));

	return &playersStorage_[id];
}

//******************************************************************************
// Update methods
//******************************************************************************

void Server::update(Time dt)
{
	const float dts = sys::to_seconds(dt);

	switch (state_)
	{
		case Running:
		{
			Connection::poll();

			updateConnectingPlayers();
			updateDisconnectingPlayers();
			updatePlayers(dt);
			updateBullets(dt);

			sendBullets();

			if (tick_ & 0x01)
				sendGameState();

			if (!matchPlaying_ && players_.size() > 0 && (tick_ - matchEndTick_) * dts >= 3.0f)
				startMatch();

			Connection::flush();

			tick_++;
		}
		break;

		case Stopping:
			Connection::poll();
			break;

		case Stopped:
			break;
	}
}

void Server::updatePlayers(Time dt)
{
	for (size_t i = 0; i < players_.size(); i++)
	{
		if (players_[i]->state == Player::Playing)
			players_[i]->update(dt, tick_, world_);
	}
}

void Server::updateBullets(Time dt)
{
	for (size_t i = 0; i < players_.size(); i++)
		updatePlayerBullets(dt, players_[i]);
}

void Server::updateConnectingPlayers()
{
	for (size_t i = 0; i < connectingPlayers_.size(); i++)
	{
		ServerPlayer *player = connectingPlayers_[i];

		if (tick_ - player->connectTick >= TimeoutTicks)
		{
			player->peer.disconnect(false);
			player->state = Player::Disconnected;

			connectingPlayers_.erase(connectingPlayers_.begin() + i--);
			freePlayers_.push_back(player);
		}
	}
}

void Server::updateDisconnectingPlayers()
{
	for (size_t i = 0; i < disconnectingPlayers_.size(); i++)
	{
		ServerPlayer *player = disconnectingPlayers_[i];

		if (player->bullets.size() == 0)
		{
			if (player->disconnectCountdown == -1)
			{
				player->disconnectCountdown = CountdownTicks;
			}
			else if (--player->disconnectCountdown == 0)
			{
				disconnectingPlayers_.erase(disconnectingPlayers_.begin() + i--);
				players_.erase(std::find(players_.begin(), players_.end(), player));

				freePlayers_.push_back(player);

				sendPlayerDisconnectMessage(player);
			}
		}
	}
}

void Server::updatePlayerBullets(Time dt, ServerPlayer *player)
{
	int initialRTT = 0;
	int currentRTT = player->peer.rtt() / (int)sys::to_milliseconds(dt);

	if (player->pongTick != -1)
		initialRTT = player->pongTick - player->pingTick;

	int lag = std::max(InterpolationTicks + initialRTT / 2, currentRTT / 2);

	world_.clear();

	for (size_t i = 0; i < players_.size(); i++)
	{
		if (players_[i]->state == Player::Playing)
		{
			if (player->id == players_[i]->id)
				world_.add(players_[i]->entity(tick_));
			else
				world_.add(players_[i]->entity(tick_ - lag));
		}
	}

	player->updateBullets(dt, world_);
}

//******************************************************************************
// Send methods
//******************************************************************************

template<typename T> void Server::sendToConnectedPlayers(const T &msg, const ServerPlayer *exception)
{
	for (size_t i = 0; i < players_.size(); i++)
	{
		if (players_[i]->connected() && players_[i] != exception)
			Connection::send(msg, players_[i]->peer);
	}
}

void Server::sendBullets()
{
	for (size_t i = 0; i < players_.size(); i++)
	{
		if (players_[i]->connected())
			sendBulletsTo(players_[i]);
	}

	bulletQueue_.clear();
}

void Server::sendBulletsTo(const ServerPlayer *targetPlayer)
{
	msg::Bullet msg;
	msg.tick = tick_;

	int k = 0;

	const size_t N = bulletQueue_.size();

	for (size_t i = 0; i < N; i++)
	{
		if (bulletQueue_[i].data.playerid != targetPlayer->id)
		{
			assert(tick_ >= targetPlayer->tick);

			const int offset = tick_ - bulletQueue_[i].tick;

			msg.bullets[k].tickOffset = std::min<int>(offset, MaxTickOffset);
			msg.bullets[k].params = bulletQueue_[i].data;

			if (++k == countof(msg.bullets) || i == N - 1)
			{
				msg.nBullets = k;
				k = 0;

				Connection::send(msg, targetPlayer->peer);
			}
		}
	}
}

void Server::sendGameState()
{
	for (size_t i = 0; i < players_.size(); i++)
	{
		if (players_[i]->connected())
			sendGameStateTo(players_[i]);
	}
}

void Server::sendGameStateTo(const ServerPlayer *targetPlayer)
{
	msg::GameState msg;
	msg.tick = tick_;
	msg.nSoldiers = 0;

	for (size_t i = 0; i < players_.size(); i++)
	{
		const ServerPlayer *player = players_[i];

		if (player != targetPlayer && player->state == Player::Playing)
		{
			msg::GameState::SoldierState &soldier = msg.soldiers[msg.nSoldiers];

			soldier.tickOffset = std::min<int>(MaxTickOffset, tick_ - player->tick);
			soldier.playerId = player->id;
			soldier.state = player->soldier.state();

			msg.nSoldiers++;
		}
	}

	Connection::send(msg, targetPlayer->peer);
}

void Server::sendDamageMessage(const ServerPlayer *attacker, const ServerPlayer *victim, uint16_t amount)
{
	msg::Damage msg;

	msg.tick = tick_;
	msg.hasAttacker = (attacker != 0);
	msg.attacker = (attacker != 0 ? attacker->id : 0);
	msg.victim = victim->id;
	msg.amount = amount;

	sendToConnectedPlayers(msg);
}

void Server::sendServerInfoMessage(const ServerPlayer *player)
{
	msg::ServerInfo msg;

	msg.tick = tick_;
	msg.clientId = player->id;
	msg.nPlayers = players_.size();
	msg.matchPlaying = matchPlaying_;
	msg.matchStartTick = matchStartTick_;

	Connection::send(msg, player->peer);
}

void Server::sendPlayerInfoMessages(const ServerPlayer *targetPlayer)
{
	for (size_t i = 0; i < players_.size(); i++)
	{
		const ServerPlayer *player = players_[i];

		msg::PlayerInfo msg;

		msg.id           = player->id;
		msg.color[0]     = player->color.r;
		msg.color[1]     = player->color.g;
		msg.color[2]     = player->color.b;
		msg.playing      = player->state == Player::Playing;
		msg.health       = player->health;
		msg.connectTick  = player->connectTick;
		msg.currentTick  = player->tick;
		msg.soldierState = player->soldier.state();

		hlp::assign(msg.name, player->nickname.c_str());

		Connection::send(msg, targetPlayer->peer);
	}
}

void Server::sendBulletInfoMessages(const ServerPlayer *player)
{
	// TODO: implement this
}

void Server::sendPlayerConnectMessage(const ServerPlayer *player)
{
	msg::PlayerConnect msg;

	msg.tick = player->connectTick;
	msg.id = player->id;
	msg.color[0] = player->color.r;
	msg.color[1] = player->color.g;
	msg.color[2] = player->color.b;

	hlp::assign(msg.name, player->nickname.c_str());

	sendToConnectedPlayers(msg, player);
}

void Server::sendPlayerDisconnectMessage(const ServerPlayer *player)
{
	msg::PlayerDisconnect msg;
	msg.tick = tick_;
	msg.id = player->id;

	sendToConnectedPlayers(msg, player);
}

void Server::sendPlayerJoinMessage(const ServerPlayer *player, const fixvec2 &position)
{
	msg::PlayerJoin msg;

	msg.id = player->id;
	msg.tick = tick_;
	msg.position = position;

	sendToConnectedPlayers(msg);
}

void Server::sendPlayerLeaveMessage(const ServerPlayer *player)
{
	msg::PlayerLeave msg;
	msg.tick = tick_;
	msg.id = player->id;

	sendToConnectedPlayers(msg, player);
}

void Server::sendMatchStartMessage()
{
	msg::MatchStart msg;
	msg.tick = tick_;

	sendToConnectedPlayers(msg);
}

void Server::sendMatchEndMessage()
{
	msg::MatchEnd msg;
	msg.tick = tick_;

	sendToConnectedPlayers(msg);
}

//******************************************************************************
// Internal events
//******************************************************************************

void Server::onSpawnBullet(void *data)
{
	const cmp::BulletParams &params = *(cmp::BulletParams*)data;
	ServerPlayer *player = getPlayerById(params.playerid);

	ent::Bullet bullet(params, &player->soldier.entity);
	bullet.collisionCallback = make_callback(this, Server, onPlayerBulletCollision);

	player->bullets.push_back(bullet);
	bulletQueue_.push_back(make_ticked(player->tick, params));
}

void Server::onPlayerBulletCollision(void *data)
{
	if (!matchPlaying_)
		return;

	struct params_t
	{
		uint8_t bulletOwner;
		const coll::Entity *entity;
	};

	params_t *params = (params_t*)data;

	ServerPlayer *attacker = getPlayerById(params->bulletOwner);
	ServerPlayer *victim = (ServerPlayer*)params->entity->data;

	int amount = MaxHealth / (20 + rand() % 5);

	victim->health -= amount;

	if (victim->health <= 0)
	{
		victim->health = 0;
		victim->state = Player::Spectator;

		onPlayerKill(attacker, victim);
	}

	sendDamageMessage(attacker, victim, amount);
}

void Server::onPlayerKill(ServerPlayer *attacker, ServerPlayer *victim)
{
	if (matchPlaying_)
	{
		attacker->kills++;
		victim->deaths++;

		if (attacker->kills >= 5)
			endMatch();
	}
}

//******************************************************************************
// Network events
//******************************************************************************

void Server::onConnect(Peer peer)
{
	if (freePlayers_.size() == 0)
	{
		peer.disconnect(false);
		return;
	}

	ServerPlayer *player = freePlayers_.back();
	freePlayers_.pop_back();

	peer.setData(player);

	player->initialize();
	player->state = Player::Connecting;
	player->peer = peer;

	connectingPlayers_.push_back(player);
}

void Server::onDisconnect(Peer peer)
{
	ServerPlayer *player = (ServerPlayer*)peer.data();
	player->state = Player::Disconnected;

	sendPlayerLeaveMessage(player);
	disconnectingPlayers_.push_back(player);

	if (players_.size() == 1)
		endMatch();
}

void Server::onMessage(const msg::Message *msg, Peer from)
{
	#define CASE(T) case msg::T::Type: on ## T((ServerPlayer*)from.data(), *(const msg::T*)msg); break

	switch (msg->type)
	{
		CASE(Login);
		CASE(Pong);
		CASE(JoinRequest);
		CASE(Input);
		CASE(PlayerChat);

		default:
			assert(false);
			break;
	}

	#undef CASE
}

void Server::onStop()
{
	state_ = Stopped;
}

void Server::onLogin(ServerPlayer *player, const msg::Login &msg)
{
	if (player->state != Player::Connecting)
		return;

	player->state = Player::Spectator;
	player->color = gfx::Color(msg.color[0], msg.color[1], msg.color[2]);
	player->nickname = msg.name;
	player->pingTick = tick_;

	sendServerInfoMessage(player);
	sendPlayerInfoMessages(player);
	sendBulletInfoMessages(player);
	sendPlayerConnectMessage(player);

	std::vector<ServerPlayer*>::iterator i;
	i = std::find(connectingPlayers_.begin(), connectingPlayers_.end(), player);

	assert(i != connectingPlayers_.end());

	connectingPlayers_.erase(std::find(connectingPlayers_.begin(), connectingPlayers_.end(), player));
	players_.push_back(player);

	if (players_.size() == 1)
		startMatch();
}

void Server::onPong(ServerPlayer *player, const msg::Pong &msg)
{
	if (player->pongTick == -1)
		player->pongTick = tick_;
}

void Server::onJoinRequest(ServerPlayer *player, const msg::JoinRequest &msg)
{
	if (!matchPlaying_ || player->state != Player::Spectator)
		return;

	size_t N = world_.spawnpoints().size();
	const fixvec2 &spawnpoint = world_.spawnpoints()[rand() % N];

	player->state = Player::Playing;
	player->soldier.reset(spawnpoint);
	player->tick = tick_;
	player->inputTick = tick_;
	player->boundsbufTick = tick_;
	player->boundsbuf.clear();
	player->boundsbuf.push(player->soldier.physics.bounds() + player->soldier.physics.position);
	player->inputs.clear();
	player->health = MaxHealth;

	sendPlayerJoinMessage(player, spawnpoint);
}

void Server::onInput(ServerPlayer *player, const msg::Input &msg)
{
	if (player->state == Player::Playing && player->inputTick == msg.tick)
	{
		cmp::SoldierInput input;

		input.rightwards = msg.rightwards;
		input.angle      = msg.angle;
		input.left       = msg.left;
		input.right      = msg.right;
		input.jump       = msg.jump;
		input.run        = msg.run;
		input.duck       = msg.duck;
		input.shoot      = msg.shoot;

		player->inputs.push_back(make_ticked(msg.tick, input));
		player->inputTick++;
	}
}

void Server::onPlayerChat(ServerPlayer *player, const msg::PlayerChat &msg)
{
	if (player->id == msg.id)
		sendToConnectedPlayers(msg, player);
}

}} // g13::net
