#include "Client.h"
#include "msg/msg.h"
#include <g13/vars.h>
#include <assert.h>
#include <json/json.h>
#include <hlp/read.h>
#include <hlp/assign.h>
#include <hlp/utf8.h>

namespace g13 {
namespace net {

Client::Client() : renderer_(this)
{
	initialize();
}

void Client::connect(const char *host, int port)
{
	assert(state_ == Disconnected);

	initialize();

	Connection::connect(host, port, ChannelCount);
	Connection::timeout(5000);

	state_ = Connecting;
}

void Client::disconnect()
{
	if (state_ == Connected || state_ == Connecting)
	{
		state_ = Disconnecting;
		Connection::disconnect();
	}
}

void Client::update(Time dt)
{
	if (state_ == Disconnected)
		return;

	Connection::poll();

	if (state_ == Connected)
	{
		world_.clear();

		updateLocalPlayer(dt);
		updateRemotePlayers(dt);
		updateBullets(dt);

		Connection::flush();
	}

	tick_++;
}

bool Client::onEvent(const sys::Event &event)
{
	if (state_ != Connected)
		return true;

	if (!renderer_.onEvent(event))
		return false;

	if (localPlayer_->state == Player::Playing)
	{
		localPlayer_->input.onEvent(event);
	}
	else if (event.type == sys::MouseButtonPress && event.mouseButton.code == sys::MouseLeft)
	{
		sendJoinRequestMessage();
	}

	if (event.type == sys::KeyPress)
	{
		if (event.key.code == sys::NumpadAdd)
		{
			renderer_.camera.zoom(Camera::ZoomIn);
		}
		else if (event.key.code == sys::NumpadSubtract)
		{
			renderer_.camera.zoom(Camera::ZoomOut);
		}
		else if (event.key.code == sys::Enter)
		{
			if (renderer_.chatbox.active)
			{
				renderer_.chatbox.hide();

				if (renderer_.chatbox.value.size() > 0)
				{
					renderer_.onPlayerChat(localPlayer_, renderer_.chatbox.value);
					sendPlayerChatMessage(renderer_.chatbox.value);
				}
			}
			else
			{
				renderer_.chatbox.show();
				renderer_.camera.zoom(Camera::ZoomNone);
			}

			return false;
		}
		else if (renderer_.chatbox.active && event.key.code == sys::Escape)
		{
			renderer_.chatbox.hide();
			return false;
		}
	}
	else if (event.type == sys::KeyRelease)
	{
		if (event.key.code == sys::NumpadAdd || event.key.code == sys::NumpadSubtract)
			renderer_.camera.zoom(Camera::ZoomNone);
	}

	return true;
}

void Client::draw(const Frame &frame)
{
	renderer_.draw(frame);
}

void Client::initialize()
{
	Connection::reset();

	tick_ = 0;
	state_ = Disconnected;

	world_.unload();

	localPlayer_ = 0;
	remotePlayers_.clear();

	localPlayerStorage_.initialize();
	localPlayerStorage_.soldier.createBulletCallback = make_callback(this, Client, onSpawnBullet);
	localPlayerStorage_.nickname = nickname_;
	localPlayerStorage_.color = color_;

	for (size_t i = 0; i < countof(remotePlayersStorage_); i++)
	{
		remotePlayersStorage_[i].id = i;
		remotePlayersStorage_[i].soldier.id = i;
		remotePlayersStorage_[i].soldier.createBulletCallback = make_callback(this, Client, onSpawnBullet);
	}

	renderer_.initialize();

	matchInfo_.playing = false;
	matchInfo_.start = 0;

	joinRequestSent_ = false;
}

void Client::loadMap(const char *name)
{
	Json::Value data;
	Json::Reader json(Json::Features::strictMode());
	json.parse(hlp::read("data/map_test2.json"), data);

	world_.load(data);
	renderer_.map.load(data);
	renderer_.camera.bounds(from_fixed(world_.bounds().tl), from_fixed(world_.bounds().br));
}

void Client::updateLocalPlayer(Time dt)
{
	if (localPlayer_->state == Player::Playing)
	{
		// send input before local player tick gets incremented
		localPlayer_->updateInput();
		sendInputMessage();

		localPlayer_->update(dt, world_);
	}
}

void Client::updateRemotePlayers(Time dt)
{
	for (size_t i = 0; i < remotePlayers_.size(); i++)
	{
		if (remotePlayers_[i]->state == Player::Playing)
			remotePlayers_[i]->update(dt, tick_ - InterpolationTicks, world_);
	}
}

void Client::updateBullets(Time dt)
{
	if (localPlayer_ != 0)
		localPlayer_->updateBullets(dt, world_);

	for (size_t i = 0; i < remotePlayers_.size(); i++)
		remotePlayers_[i]->updateBullets(dt, world_);
}

void Client::sendInputMessage()
{
	msg::Input input;

	input.tick       = localPlayer_->tick;
	input.rightwards = localPlayer_->input.rightwards;
	input.angle      = localPlayer_->input.angle;
	input.left       = localPlayer_->input.left;
	input.right      = localPlayer_->input.right;
	input.jump       = localPlayer_->input.jump;
	input.run        = localPlayer_->input.run;
	input.duck       = localPlayer_->input.duck;
	input.shoot      = localPlayer_->input.shoot;

	Connection::send(input);
}

void Client::sendLoginMessage()
{
	msg::Login login;

	hlp::assign(login.name, nickname_.c_str());

	login.color[0] = color_.r;
	login.color[1] = color_.g;
	login.color[2] = color_.b;

	Connection::send(login);
}

void Client::sendPongMessage()
{
	msg::Pong pong;

	Connection::send(pong);
	Connection::flush();
}

void Client::sendJoinRequestMessage()
{
	if (!joinRequestSent_ && matchInfo_.playing)
	{
		Connection::send(msg::JoinRequest());
		joinRequestSent_ = true;
	}
}

void Client::sendPlayerChatMessage(const string32_t &text)
{
	msg::PlayerChat msg;
	msg.id = localPlayer_->id;

	hlp::utf8_encode(text, msg.text);

	Connection::send(msg);
}

Player *Client::getPlayerById(int id)
{
	return isLocalPlayer(id) ? (Player*)localPlayer_ : (Player*)&remotePlayersStorage_[id];
}

bool Client::isLocalPlayer(int id) const
{
	return localPlayer_ != 0 && localPlayer_->id == id;
}

bool Client::isLocalPlayer(const Player *player) const
{
	return localPlayer_ != 0 && player == localPlayer_;
}

void Client::onSpawnBullet(void *data)
{
	const cmp::BulletParams &params = *(cmp::BulletParams*)data;
	Player *player = getPlayerById(params.playerid);

	ent::Bullet bullet(params, &player->soldier.entity);
	bullet.collisionCallback = make_callback(this, Client, onPlayerBulletCollision);

	player->bullets.push_back(bullet);
}

void Client::onPlayerBulletCollision(void *data)
{
	// TODO: show blood, etc

	struct params_t
	{
		uint8_t bulletOwner;
		const coll::Entity *entity;
	};

	params_t *params = (params_t*)data;
	renderer_.onPlayerHit((Player*)params->entity->data);
}

void Client::onConnect(Peer)
{
	sendLoginMessage();
}

void Client::onDisconnect(Peer)
{
	state_ = Disconnected;
	Connection::reset();
}

void Client::onMessage(const msg::Message *msg, Peer)
{
	#define CASE(T) case msg::T::Type: on ## T(*(const msg::T*)msg); break

	switch (msg->type)
	{
		CASE(ServerInfo);
		CASE(PlayerInfo);
		CASE(PlayerConnect);
		CASE(PlayerDisconnect);
		CASE(PlayerJoin);
		CASE(PlayerLeave);
		CASE(PlayerChat);
		CASE(GameState);
		CASE(Bullet);
		CASE(Damage);
		CASE(MatchStart);
		CASE(MatchEnd);

		default:
			assert(false);
			break;
	}

	#undef CASE
}

void Client::onConnectionReady()
{
	state_ = Connected;

	localPlayer_->state = Player::Spectator;
	localPlayer_->health = MaxHealth;

	renderer_.onPlayerConnect(localPlayer_);
}

void Client::onServerInfo(const msg::ServerInfo &msg)
{
	sendPongMessage();

	tick_ = msg.tick;

	localPlayerStorage_.id = msg.clientId;
	localPlayerStorage_.soldier.id = msg.clientId;
	localPlayer_ = &localPlayerStorage_;

	matchInfo_.playing = msg.matchPlaying;
	matchInfo_.start = msg.matchStartTick;

	vars::set(msg.vars);

	loadMap(0);

	connectingProgress_.remainingPlayers = msg.nPlayers;

	if (connectingProgress_.ready())
		onConnectionReady();
}

void Client::onPlayerInfo(const msg::PlayerInfo &msg)
{
	RemotePlayer *player = (RemotePlayer*)getPlayerById(msg.id);

	assert(player != 0);

	player->initialize();
	player->state       = msg.playing ? Player::Playing : Player::Spectator;
	player->nickname    = msg.name;
	player->color       = gfx::Color(msg.color[0], msg.color[1], msg.color[2]);
	player->health      = msg.health;
	player->kills       = msg.kills;
	player->deaths      = msg.deaths;
	player->connectTick = msg.connectTick;
	player->spawnTick   = msg.currentTick;

	player->stateBuffer.push(make_ticked(msg.currentTick, msg.soldierState));

	remotePlayers_.push_back(player);

	renderer_.onPlayerConnect(player);

	connectingProgress_.remainingPlayers--;

	if (connectingProgress_.ready())
		onConnectionReady();
}

void Client::onPlayerConnect(const msg::PlayerConnect &msg)
{
	RemotePlayer *player = (RemotePlayer*)getPlayerById(msg.id);

	player->initialize();
	player->state       = Player::Spectator;
	player->nickname    = msg.name;
	player->color       = gfx::Color(msg.color[0], msg.color[1], msg.color[2]);
	player->connectTick = msg.tick;
	player->kills       = 0;
	player->deaths      = 0;

	remotePlayers_.push_back(player);

	renderer_.onPlayerConnect(player);
}

void Client::onPlayerDisconnect(const msg::PlayerDisconnect &msg)
{
	for (size_t i = 0; i < remotePlayers_.size(); i++)
	{
		if (remotePlayers_[i]->id == msg.id)
		{
			remotePlayers_[i]->state = Player::Disconnected;
			remotePlayers_.erase(remotePlayers_.begin() + i);

			break;
		}
	}
}

void Client::onPlayerJoin(const msg::PlayerJoin &msg)
{
	Player *player = getPlayerById(msg.id);

	player->state = Player::Playing;
	player->health = MaxHealth;
	player->soldier.reset(msg.position);

	if (isLocalPlayer(player))
	{
		localPlayer_->input = cmp::SoldierInput();
		localPlayer_->tick = msg.tick;
		joinRequestSent_ = false;
	}
	else
	{
		RemotePlayer *p = (RemotePlayer*)player;

		p->spawnTick = msg.tick;
		p->stateBuffer.clear();
		p->stateBuffer.push(make_ticked(msg.tick, p->soldier.state()));
	}

	renderer_.onPlayerJoin(player);
}

void Client::onPlayerLeave(const msg::PlayerLeave &msg)
{
	getPlayerById(msg.id)->state = Player::Spectator;
}

void Client::onPlayerChat(const msg::PlayerChat &msg)
{
	renderer_.onPlayerChat(getPlayerById(msg.id), msg.text);
}

void Client::onGameState(const msg::GameState &msg)
{
	for (int i = 0; i < msg.nSoldiers; i++)
	{
		RemotePlayer *player = (RemotePlayer*)getPlayerById(msg.soldiers[i].playerId);

		assert(!isLocalPlayer(player));

		int tick = msg.tick - msg.soldiers[i].tickOffset;

		if (player->stateBuffer.size() == 0 || tick > player->stateBuffer.back().tick)
			player->stateBuffer.push(make_ticked(tick, msg.soldiers[i].state));
	}
}

void Client::onBullet(const msg::Bullet &msg)
{
	for (int i = 0; i < msg.nBullets; i++)
	{
		RemotePlayer *player = (RemotePlayer*)getPlayerById(msg.bullets[i].params.playerid);

		assert(!isLocalPlayer(player));

		int tick = msg.tick - msg.bullets[i].tickOffset;

		if (player->connected() && tick >= player->connectTick && tick >= matchInfo_.start)
			player->onBullet(tick, msg.bullets[i].params);
	}
}

void Client::onDamage(const msg::Damage &msg)
{
	Player *attacker = msg.hasAttacker ? getPlayerById(msg.attacker) : 0;
	Player *victim = getPlayerById(msg.victim);

	victim->health -= msg.amount;

	if (victim->health <= 0)
	{
		victim->state = Player::Spectator;
		victim->health = 0;
		victim->deaths++;

		if (attacker != 0)
			attacker->kills++;

		renderer_.onPlayerKill(attacker, victim);
	}

	renderer_.onPlayerDamage(attacker, victim);
}

void Client::onMatchStart(const msg::MatchStart &msg)
{
	matchInfo_.playing = true;
	matchInfo_.start = msg.tick;

	joinRequestSent_ = false;

	localPlayer_->state = Player::Spectator;
	localPlayer_->bullets.clear();
	localPlayer_->kills = 0;
	localPlayer_->deaths = 0;

	for (size_t i = 0; i < remotePlayers_.size(); i++)
	{
		RemotePlayer *player = remotePlayers_[i];

		player->state = Player::Spectator;
		player->bullets.clear();
		player->kills = 0;
		player->deaths = 0;
	}

	renderer_.onMatchStart();
}

void Client::onMatchEnd(const msg::MatchEnd &msg)
{
	matchInfo_.playing = false;
}

}} // g13::net
