#include "Client.h"
#include "msg.h"

#include <g13/g13.h>
#include <g13/res.h>
#include <g13/Map.h>

#include <gfx/gfx.h>
#include <hlp/assign.h>
#include <hlp/utf8.h>

#include <assert.h>
#include <iostream>

namespace g13 {
namespace net {

Client::Client()
	:	state_(Disconnected),
		peer_(0),
		id_(Player::InvalidId),
		background_(0),
		texture_(0),
		spriteBatch_(0),
		chatText_(0),
		chatBackground_(0),
		textInputMode_(false)
{
	hlp::assign(name_, "player");

	caret_ += '_';

	// load resources

	chatText_ = new gfx::Text();
	chatText_->font(res::font(res::DefaultFont));
	chatText_->size(9);
	chatText_->color(gfx::Color(0, 0, 0));
	chatText_->value(caret_);

	chatBackground_ = new gfx::VBO();
	chatBackground_->allocate<gfx::ColorVertex>(4, gfx::Static);
	chatBackground_->mode(gfx::TriangleFan);

	background_ = new gfx::VBO();
	background_->allocate<gfx::ColorVertex>(4, gfx::Static);
	background_->mode(gfx::TriangleFan);

	texture_ = res::texture(res::Soldier);
	spriteBatch_ = new gfx::SpriteBatch(MaxPlayers);
	spriteBatch_->texture(texture_);
}

Client::~Client()
{
	delete chatText_;
	delete background_;
	delete spriteBatch_;
}

bool Client::connect(const char *host, int port)
{
	assert(connection_ == 0 && peer_ == 0);

	connection_ = enet_host_create(0, 1, ChannelsCount, 0, 0);

	if (connection_ == 0)
	{
		error_log("enet_host_create failed.");
		return false;
	}

	ENetAddress address;
	enet_address_set_host(&address, host);
	address.port = port;

	peer_ = enet_host_connect(connection_, &address, ChannelsCount, 0);

	if (peer_ == 0)
	{
		error_log("enet_host_connect failed.");
		return false;
	}

	enet_peer_timeout(peer_, 0, 4000, 5000);

	state_ = Connecting;

	debug_log("Connecting to " << host << ":" << port << "...");

	return true;
}

void Client::disconnect()
{
	if (state_ == Connected)
		enet_peer_disconnect(peer_, 0);
}

void Client::update(Time dt)
{
	if (state_ == Disconnected)
		return;

	Multiplayer::update(dt);

	if (active())
	{
		if (players_[id_].state() == Player::Playing)
		{
			input_.update();
			players_[id_].onInput(tick_, input_);
			players_[id_].update(dt, tick_);

			msg::Input inputMsg;
			inputMsg.tick = tick_;
			inputMsg.input = input_.serialize();
			send(&inputMsg, peer_);
		}

		for (size_t i = 0; i < MaxPlayers; i++)
		{
			Player *player = &players_[i];

			if (i == id_ || player->state() != Player::Playing)
				continue;

			player->update(dt, tick_);
		}

		camera_.update(dt);
	}

	tick_++;
}

bool Client::active() const
{
	return state_ == Connected && id_ != Player::InvalidId && players_[id_].connected();
}

Client::State Client::state() const
{
	return state_;
}

void Client::onConnect(ENetPeer *peer)
{
	state_ = Connected;

	msg::Login login;
	hlp::assign(login.name, name_);

	send(&login, peer);

	debug_log("Connected to server. Waiting response...");
}

void Client::onDisconnect(ENetPeer *peer)
{
	#ifdef DEBUG
		if (state_ == Connecting)
			debug_log("Connection failed.");
		else
			debug_log("Disconnected from host.");
	#endif

	enet_host_destroy(connection_);

	peer_ = 0;
	connection_ = 0;
	id_ = Player::InvalidId;
	state_ = Disconnected;
	textInputMode_ = false;
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

	loadMap();

	for (size_t i = 0; i < MaxPlayers; i++)
	{
		players_[i].initialize();
		players_[i].mode(Player::Remote);
	}

	connectingCount_ = 0;

	for (size_t i = 0; i < info->nPlayers; i++)
	{
		Player *player = &players_[info->players[i]];
		player->onConnecting();
		connectingCount_++;
	}

	players_[id_].mode(Player::Local);

	debug_log("Received server info. Players: " << info->nPlayers << "/" << MaxPlayers << ".");

	if (connectingCount_ > 0)
	{
		players_[id_].onConnecting();

		debug_log("Waiting for players info...");
	}
	else
	{
		players_[id_].onConnect(name_);

		msg::Ready ready;
		send(&ready, peer_);

		debug_log("Ready to play.");
	}

}

void Client::onPlayerConnect(msg::Message *msg)
{
	msg::PlayerConnect *playerConnect = (msg::PlayerConnect*)msg;

	if (playerConnect->id == id_)
		return;

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

			debug_log("Ready to play.");
		}
	}
	else
	{
		player->onConnect(playerConnect->name);

		debug_log("Player #" << (int)playerConnect->id << " connected.");
	}
}

void Client::onPlayerDisconnect(msg::Message *msg)
{
	msg::PlayerDisconnect *playerDisconnect = (msg::PlayerDisconnect*)msg;
	players_[playerDisconnect->id].onDisconnect();

	debug_log("Player #" << (int)playerDisconnect->id << " disconnected.");
}

void Client::onPlayerJoin(msg::Message *msg)
{
	msg::PlayerJoin *playerJoin = (msg::PlayerJoin*)msg;
	players_[playerJoin->id].onJoin(playerJoin->tick, map_, playerJoin->position);

	if (playerJoin->id == id_)
	{
		camera_.target(&players_[id_].soldier()->graphics.position.current);

		int w, h;
		sys::framebuffer_size(&w, &h);
		onResize(w, h);

		#ifdef DEBUG
			dbg->map = map_;
			dbg->soldier = players_[id_].soldier();
			dbg->loadCollisionHulls();
		#endif
	}

	#ifdef DEBUG
		if (playerJoin->id == id_)
			debug_log("Joined game.");
		else
			debug_log("Player #" << (int)playerJoin->id << " joined.");
	#endif
}

void Client::onGameState(msg::Message *msg)
{
	if (map_ == 0) return;

	msg::GameState *gameState = (msg::GameState*)msg;

	for (size_t i = 0; i < gameState->nSoldiers; i++)
	{
		Player *player = &players_[gameState->soldiers[i].playerId];

		if (active() && player->state() == Player::Connected)
			player->onJoin(tick_, map_, gameState->soldiers[i].state.position);

		if (player->state() == Player::Playing)
		{
			uint32_t tick = gameState->tick - gameState->soldiers[i].tickOffset;
			player->onSoldierState(tick, gameState->soldiers[i].state);
		}
	}
}

void Client::draw(float framePercent)
{
	if (active() && players_[id_].state() == Player::Playing)
	{
		gfx::matrix(mat4(1.0f));
		gfx::draw(background_);

		spriteBatch_->clear();

		for (size_t i = 0; i < MaxPlayers; i++)
		{
			if (players_[i].state() == Player::Playing)
			{
				ent::Soldier *soldier = players_[i].soldier();
				soldier->graphics.frame(framePercent);
				spriteBatch_->add(soldier->graphics.sprite);
			}
		}

		gfx::matrix(camera_.matrix(framePercent));

		map_->draw();

		#ifdef DEBUG
			dbg->drawCollisionHulls();
		#endif

		gfx::draw(spriteBatch_);
	}

	if (textInputMode_)
	{
		int width, height;
		sys::framebuffer_size(&width, &height);

		gfx::matrix(mat4(1.0f));
		gfx::draw(chatBackground_);
		gfx::translate(10.0f, height - 10.0f);
		gfx::draw(chatText_);
	}

	if (chatText_->font()->texture(0) != 0)
	{
		gfx::Sprite sprite;
		sprite.texture = chatText_->font()->texture(0);
		sprite.width = sprite.texture->width();
		sprite.height = sprite.texture->height();
		sprite.u[1] = 1.0f;
		sprite.v[1] = 1.0f;
		sprite.opacity = 0.5f;

		gfx::matrix(mat4(1.0f));
		gfx::draw(sprite);
	}
}

bool Client::event(Event *evt)
{
	bool result = true;

	switch (evt->type)
	{
		case Event::Resized:
			onResize(evt->size.fboWidth, evt->size.fboHeight);
			break;

		case Event::FocusLost:
			camera_.zoom(ent::Camera::ZoomNone);

		case Event::KeyReleased:
			if (evt->key.code == sys::NumpadAdd || evt->key.code == sys::NumpadSubtract)
				camera_.zoom(ent::Camera::ZoomNone);

			break;

		case Event::KeyPressed:
			if (evt->key.code == sys::Enter || (textInputMode_ && evt->key.code == sys::Escape))
			{
				textInputMode_ = !textInputMode_;

				if (!textInputMode_ && chatString_.size() > 0)
				{
					msg::Chat msg;
					hlp::utf8_encode(chatString_, msg.text);
					send(&msg, peer_);
				}

				chatString_.clear();
				chatText_->value(chatString_ + caret_);

				camera_.zoom(ent::Camera::ZoomNone);

				result = false;
				break;
			}

			if (!textInputMode_)
			{
				if (evt->key.code == sys::NumpadAdd)
					camera_.zoom(ent::Camera::ZoomIn);
				else if (evt->key.code == sys::NumpadSubtract)
					camera_.zoom(ent::Camera::ZoomOut);

				break;
			}

		case Event::KeyRepeat:
			if (textInputMode_)
			{
				if (evt->key.code == sys::Backspace && chatString_.size() > 0)
				{
					chatString_.erase(chatString_.size() - 1, 1);
					chatText_->value(chatString_ + caret_);
				}

				result = false;
			}
			break;

		case Event::TextEntered:
			if (textInputMode_)
			{
				if (chatString_.size() < 256)
				{
					chatString_ += evt->text.ch;
					chatText_->value(chatString_ + caret_);
				}
				result = false;
			}
			break;

		default: break;
	}

	return result;
}

void Client::onResize(int width, int height)
{
	camera_.viewport(width, height);

	gfx::ColorVertex vertex[4];

	float w = (float)width;
	float h = (float)height;

	vertex[0] = gfx::color_vertex(0.0f, 0.0f,   0,   0, 255, 255);
	vertex[1] = gfx::color_vertex(w   , 0.0f,   0,   0, 255, 255);
	vertex[2] = gfx::color_vertex(w   , h   , 255, 255, 255, 255);
	vertex[3] = gfx::color_vertex(0.0f, h   , 255, 255, 255, 255);

	background_->set(vertex, 0, 4);

	vertex[0] = gfx::color_vertex(0.0f, h - 35.0f, 255, 255, 255, 255);
	vertex[1] = gfx::color_vertex(w   , h - 35.0f, 255, 255, 255, 255);
	vertex[2] = gfx::color_vertex(w   , h        , 255, 255, 255, 255);
	vertex[3] = gfx::color_vertex(0.0f, h        , 255, 255, 255, 255);

	chatBackground_->set(vertex, 0, 4);
}

}} // g13::net
