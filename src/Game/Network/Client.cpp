#include "Client.h"
#include "msg.h"
#include "../Game.h"
#include "../Map.h"
#include "../../System/Window.h"

#include "../Debugger.h"

#include <hlp/assign.hpp>
#include <assert.h>
#include <iostream>

#define LOG(x) DBG( std::cout << "[Client] " << x << std::endl; )

namespace net
{
	Client::Client()
		:	state_(Disconnected),
			peer_(0),
			id_(Player::InvalidId),
			background_(0),
			texture_(0),
			spriteBatch_(0)
	{
		hlp::assign(name_, "player");

		// load resources
		Graphics *gfx = game->graphics;
		background_ = gfx->buffer<ColorVertex>(vbo_t::TriangleFan, vbo_t::StaticDraw, 4);
		texture_ = gfx->texture("data/guy.png");
		spriteBatch_ = gfx->batch(MaxPlayers);
		spriteBatch_->texture(texture_);

		int w, h;
		game->window->size(w, h);
		onResize(w, h);
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
			LOG("enet_host_create failed.")
			return false;
		}

		ENetAddress address;
		enet_address_set_host(&address, host);
		address.port = port;

		peer_ = enet_host_connect(connection_, &address, ChannelsCount, 0);

		if (peer_ == 0)
		{
			LOG("enet_host_connect failed.")
			return false;
		}

		enet_peer_timeout(peer_, 0, 4000, 5000);

		state_ = Connecting;

		LOG("Connecting to " << host << ":" << port << "...")

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

		LOG("Connected to server. Waiting response...")
	}

	void Client::onDisconnect(ENetPeer *peer)
	{
		DBG(
			if (state_ == Connecting)
				LOG("Connection failed.")
			else
				LOG("Disconnected from host.")
		);

		enet_host_destroy(connection_);

		peer_ = 0;
		connection_ = 0;
		id_ = Player::InvalidId;
		state_ = Disconnected;
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

		LOG("Received server info. Players: " << info->nPlayers << "/" << MaxPlayers << ".")

		if (connectingCount_ > 0)
		{
			players_[id_].onConnecting();

			LOG("Waiting for players info...")
		}
		else
		{
			players_[id_].onConnect(name_);

			msg::Ready ready;
			send(&ready, peer_);

			LOG("Ready to play.")
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

				LOG("Ready to play.")
			}
		}
		else
		{
			player->onConnect(playerConnect->name);

			LOG("Player #" << (int)playerConnect->id << " connected.")
		}
	}

	void Client::onPlayerDisconnect(msg::Message *msg)
	{
		msg::PlayerDisconnect *playerDisconnect = (msg::PlayerDisconnect*)msg;
		players_[playerDisconnect->id].onDisconnect();

		LOG("Player #" << (int)playerDisconnect->id << " disconnected.")
	}

	void Client::onPlayerJoin(msg::Message *msg)
	{
		msg::PlayerJoin *playerJoin = (msg::PlayerJoin*)msg;
		players_[playerJoin->id].onJoin(playerJoin->tick, map_, playerJoin->position);

		if (playerJoin->id == id_)
			camera_.target(&players_[id_].soldier()->graphics.position.current);

		DBG(
			if (playerJoin->id == id_)
				LOG("Joined game.")
			else
				LOG("Player #" << (int)playerJoin->id << " joined.")
		)
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

			player->onSoldierState(gameState->tick, gameState->soldiers[i].state);
		}
	}

	void Client::draw(float framePercent)
	{
		Graphics *gfx = game->graphics;

		gfx->clear();
		gfx->matrix(mat4(1.0f));
		gfx->bind(Graphics::ColorShader);
		gfx->draw(background_);

		if (active())
		{
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

			gfx->matrix(camera_.matrix(framePercent));
			map_->draw(gfx);
			gfx->draw(spriteBatch_);
		}
	}

	void Client::event(const Event &evt)
	{
		switch (evt.type)
		{
			case Event::Resize:
				onResize(evt.resize.width, evt.resize.height);
				break;

			case Event::Keyboard:
				if (evt.keyboard.pressed)
					onKeyPressed(evt.keyboard.key);
				break;

			default: break;
		}
	}

	void Client::onResize(int width, int height)
	{
		camera_.viewport(width, height);

		ColorVertex vertices[4];

		vertices[0].position = vec2(0.0f, 0.0f);
		vertices[1].position = vec2((float)width, 0.0f);
		vertices[2].position = vec2((float)width, (float)height);
		vertices[3].position = vec2(0.0f, (float)height);

		vertices[0].color = u8vec4(0, 0, 255, 255);
		vertices[1].color = u8vec4(0, 0, 255, 255);
		vertices[2].color = u8vec4(255, 255, 255, 255);
		vertices[3].color = u8vec4(255, 255, 255, 255);

		background_->set(vertices, 0, 4);
	}

	void Client::onKeyPressed(Keyboard::Key key)
	{
	}
}
