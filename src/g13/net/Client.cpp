#include "Client.h"
#include "msg.h"

#include <g13/g13.h>
#include <g13/res.h>
#include <g13/Map.h>
#include <g13/callback.h>

#include <gfx/gfx.h>
#include <hlp/assign.h>
#include <hlp/utf8.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

#include <assert.h>
#include <iostream>

namespace g13 {
namespace net {

Client::Client()
	:	state_(Disconnected),
		peer_(0),
		id_(Player::InvalidId),
		background_(0),
		soldiersBatch_(0),
		bulletsBatch_(0),
		chatText_(0),
		chatBackground_(0),
		textInputMode_(false)
{
	hlp::assign(name_, "asshole");

	caret_ += '_';

	// load resources

	chatBackground_ = new gfx::VBO();
	chatBackground_->allocate<gfx::ColorVertex>(4, gfx::Static);
	chatBackground_->mode(gfx::TriangleFan);

	background_ = new gfx::VBO();
	background_->allocate<gfx::ColorVertex>(4, gfx::Static);
	background_->mode(gfx::TriangleFan);

	soldiersBatch_ = new gfx::SpriteBatch(MaxPlayers * cmp::SoldierGraphics::SpriteCount);
	soldiersBatch_->texture(res::texture(res::Soldier));

	bulletsBatch_ = new gfx::SpriteBatch(64);
	bulletsBatch_->texture(res::texture(res::Bullet));

	gfx::Font *font = res::font(res::DefaultFont);
	uint32_t fontSize = 11;

	chatText_ = new gfx::Text();
	chatText_->font(font);
	chatText_->size(fontSize);
	chatText_->color(gfx::Color(0xFF));
	chatText_->value(caret_);

	for (int i = 0; i < MaxPlayers; i++)
	{
		playersText_[i].text = new gfx::Text();
		playersText_[i].text->font(font);
		playersText_[i].text->size(fontSize);
		playersText_[i].text->color(gfx::Color(0xFF));
		playersText_[i].text->outlineColor(gfx::Color(0x00));
		playersText_[i].text->outlineWidth(0.5f);

		nicknamesText_[i].font(font);
		nicknamesText_[i].size(fontSize);
		nicknamesText_[i].color(gfx::Color(0xFF));
		nicknamesText_[i].outlineColor(gfx::Color(0x00));
		nicknamesText_[i].outlineWidth(0.5f);

		players_[i].soldier()->createBulletCallback = make_callback(this, Client, createBullet);
	}

	healthBar_.setPosition(15.0f, 15.0f);
	healthBar_.setSize(100.0f, 15.0f);
	healthBar_.setOutline(1.0f, gfx::Color(0));
	healthBar_.setOpacity(0.7f);

	connectingText_.font(font);
	connectingText_.size(16);
	connectingText_.color(gfx::Color(0xFF));
	connectingText_.value(hlp::utf8_decode("Connecting..."));

	// preload some glyphs

	font->size(fontSize);

	for (int i = 32; i < 127; i++)
		font->glyph(i);

	font->outlineWidth(0.5f);

	for (int i = 32; i < 127; i++)
		font->glyph(i);

	onResize(sys::framebuffer_width(), sys::framebuffer_height());
}

Client::~Client()
{
	for (int i = 0; i < MaxPlayers; i++)
		delete playersText_[i].text;

	delete chatText_;
	delete chatBackground_;
	delete background_;
	delete soldiersBatch_;
	delete bulletsBatch_;
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

	enet_host_compress_with_range_coder(connection_);

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

	pollEvents();

	if (active())
	{
		map_->world()->clear();

		if (players_[id_].state() == Player::Playing)
		{
			players_[id_].soldier()->graphics.aim(input_.angle, input_.rightwards);

			players_[id_].onInput(tick_, input_);
			players_[id_].updateLocal(dt);

			map_->world()->add(&(players_[id_].soldier()->collisionEntity));

			msg::Input input;

			input.tick       = tick_;
			input.rightwards = input_.rightwards;
			input.angle      = input_.angle;
			input.left       = input_.left;
			input.right      = input_.right;
			input.jump       = input_.jump;
			input.run        = input_.run;
			input.duck       = input_.duck;
			input.shoot      = input_.shoot;

			send(&input, peer_);

			// this shit fixes some camera issues
			const vec2 &position = from_fixed(players_[id_].soldier()->physics.position);
			Frame frame = {0, 0, 1.0f};
			camera_.frame(frame);
			target_ = camera_.matrixinv() * vec2(sys::mousex(), sys::mousey());
			cameraTarget_ = glm::mix(target_, position, 0.5f);
		}

		for (int i = 0; i < MaxPlayers; i++)
		{
			Player *player = &players_[i];

			if (i == id_ || player->state() != Player::Playing)
				continue;

			player->updateRemote(dt, tick_ - InterpolationTicks);

			map_->world()->add(&(player->soldier()->collisionEntity));
		}

		for (int i = 0; i < MaxPlayers; i++)
		{
			if (players_[i].connected())
				players_[i].updateBullets(dt);
		}

		camera_.update(dt);
	}

	for (int i = 0; i < MaxPlayers; i++)
	{
		if (playersText_[i].time > 0)
			playersText_[i].time = std::max(0, (int)(playersText_[i].time - dt));
	}

	if (connection_ != 0)
		enet_host_flush(connection_);

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

void Client::nick(const string32_t &nick)
{
	const char *names[] = {"batman", "monkey", "duck", "megatron", "robin hood"};

	if (nick.size() > 0)
		hlp::utf8_encode(nick, name_);
	else
		hlp::assign(name_, names[rand() % countof(names)]);
}

void Client::createBullet(void *data)
{
	const cmp::BulletParams &params = *(cmp::BulletParams*)data;
	Player *player = &players_[params.playerid];

	player->createBullet(params, make_callback(this, Client, playerBulletCollision));
}

void Client::playerBulletCollision(void *data)
{
	// TODO: show blood, etc
}

void Client::onConnect(ENetPeer *peer)
{
	state_ = Connected;

	msg::Login login;
	hlp::assign(login.name, name_);

	login.color[0] = soldierColor_.r;
	login.color[1] = soldierColor_.g;
	login.color[2] = soldierColor_.b;

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

	sys::set_cursor(0);

	peer_ = 0;
	connection_ = 0;
	id_ = Player::InvalidId;
	state_ = Disconnected;
	textInputMode_ = false;
	target_ = vec2(0.0f);
	input_ = cmp::SoldierInput();
	camera_ = ent::Camera();

	for (int i = 0; i < MaxPlayers; i++)
	{
		players_[i].initialize();
		playersText_[i].time = 0;
		players_[i].soldier()->graphics.target = 0;
	}
}

void Client::onMessage(msg::Message *msg, ENetPeer *from)
{
	switch (msg->type())
	{
		case msg::ServerInfo::Type:       onServerInfo((msg::ServerInfo*)msg);             break;
		case msg::PlayerConnect::Type:    onPlayerConnect((msg::PlayerConnect*)msg);       break;
		case msg::PlayerDisconnect::Type: onPlayerDisconnect((msg::PlayerDisconnect*)msg); break;
		case msg::PlayerJoin::Type:       onPlayerJoin((msg::PlayerJoin*)msg);             break;
		case msg::Chat::Type:             onPlayerChat((msg::Chat*)msg);                   break;
		case msg::GameState::Type:        onGameState((msg::GameState*)msg);               break;
		case msg::Bullet::Type:           onBullet((msg::Bullet*)msg);                     break;
		case msg::Damage::Type:           onDamage((msg::Damage*)msg);                     break;

		default: break;
	}
}

void Client::onServerInfo(msg::ServerInfo *info)
{
	id_ = info->clientId;
	tick_ = info->tick;
	nicknamesText_[id_].value(hlp::utf8_decode(name_));

	{
		msg::Pong pong;
		send(&pong, peer_);
		enet_host_flush(connection_);
	}

	loadMap();

	camera_.bounds(from_fixed(map_->world()->bounds().tl), from_fixed(map_->world()->bounds().br));
	camera_.viewport(sys::framebuffer_width(), sys::framebuffer_height());

	#ifdef DEBUG
		dbg->map = map_;
		dbg->soldier = players_[id_].soldier();
		dbg->loadCollisionHulls();
	#endif

	for (int i = 0; i < MaxPlayers; i++)
		players_[i].initialize();

	connectingCount_ = 0;

	for (int i = 0; i < info->nPlayers; i++)
	{
		Player *player = &players_[info->players[i]];
		player->onConnecting();
		connectingCount_++;
	}

	players_[id_].soldier()->graphics.target = &target_;

	debug_log("Received server info. Players: " << info->nPlayers << "/" << MaxPlayers << ".");

	if (connectingCount_ > 0)
	{
		players_[id_].onConnecting();

		debug_log("Waiting for players info...");
	}
	else
	{
		players_[id_].onConnect(name_, soldierColor_);

		sys::set_cursor(res::cursor(res::Crosshair));

		msg::Ready ready;
		send(&ready, peer_);

		debug_log("Ready to play.");
	}

}

void Client::onPlayerConnect(msg::PlayerConnect *playerConnect)
{
	if (playerConnect->id == id_)
		return;

	Player *player = &players_[playerConnect->id];
	gfx::Color color(playerConnect->color[0], playerConnect->color[1], playerConnect->color[2]);

	assert(!player->connected());

	nicknamesText_[playerConnect->id].value(hlp::utf8_decode(playerConnect->name));

	if (player->state() == Player::Connecting)
	{
		player->onConnect(playerConnect->name, color);

		if (--connectingCount_ == 0)
		{
			players_[id_].onConnect(name_, soldierColor_);

			sys::set_cursor(res::cursor(res::Crosshair));

			msg::Ready ready;
			send(&ready, peer_);

			debug_log("Ready to play.");
		}
	}
	else
	{
		player->onConnect(playerConnect->name, color);

		debug_log("Player #" << (int)playerConnect->id << " connected.");
	}
}

void Client::onPlayerDisconnect(msg::PlayerDisconnect *playerDisconnect)
{
	players_[playerDisconnect->id].onDisconnect(playerDisconnect->tick);
	playersText_[playerDisconnect->id].time = 0;

	debug_log("Player #" << (int)playerDisconnect->id << " disconnected.");
}

void Client::onPlayerJoin(msg::PlayerJoin *playerJoin)
{
	Player *player = &players_[playerJoin->id];

	player->onJoin(playerJoin->tick, map_, playerJoin->position);
	player->onSoldierState(playerJoin->tick, player->soldier()->state());

	if (playerJoin->id == id_)
	{
		cameraTarget_ = players_[id_].soldier()->graphics.position.current;
		camera_.target(&cameraTarget_);
		healthBar_.setPercent(players_[id_].health() / (float)Player::MaxHealth);
	}

	#ifdef DEBUG
		if (playerJoin->id == id_)
			debug_log("Joined game.");
		else
			debug_log("Player #" << (int)playerJoin->id << " joined.");
	#endif
}

void Client::onPlayerChat(msg::Chat *chat)
{
	if (players_[chat->id].state() == Player::Playing)
	{
		playersText_[chat->id].text->value(hlp::utf8_decode(chat->text));
		playersText_[chat->id].time = sys::time<sys::Seconds>(5);
	}
}

void Client::onGameState(msg::GameState *gameState)
{
	if (map_ == 0) return;

	for (int i = 0; i < gameState->nSoldiers; i++)
	{
		Player *player = &players_[gameState->soldiers[i].playerId];

		if (active() && player->state() == Player::Connected)
			player->onJoin(tick_, map_, gameState->soldiers[i].state.position);

		if (player->state() == Player::Playing)
		{
			int tick = gameState->tick - gameState->soldiers[i].tickOffset;
			player->onSoldierState(tick, gameState->soldiers[i].state);
		}
	}
}

void Client::onBullet(msg::Bullet *bullet)
{
	for (int i = 0; i < bullet->nBullets; i++)
	{
		const cmp::BulletParams &params = bullet->bullets[i].params;
		int tick = bullet->tick - bullet->bullets[i].tickOffset;

		players_[params.playerid].onBulletCreated(tick, params);
	}
}

void Client::onDamage(msg::Damage *damage)
{
	players_[damage->playerId].onDamage(damage->tick, damage->amount);

	if (active() && damage->playerId == id_)
		healthBar_.setPercent(players_[id_].health() / (float)Player::MaxHealth);
}

void Client::draw(const Frame &frame)
{
	if (active() && players_[id_].state() >= Player::Playing)
	{
		camera_.frame(frame);
		target_ = camera_.matrixinv() * vec2(sys::mousex(), sys::mousey());

		// draw map

		map_->draw(&camera_);

		#ifdef DEBUG
			dbg->drawCollisionHulls();
		#endif

		// draw soldiers

		soldiersBatch_->clear();

		for (int i = 0; i < MaxPlayers; i++)
		{
			if (players_[i].state() == Player::Playing)
			{
				ent::Soldier *soldier = players_[i].soldier();

				soldier->graphics.frame(frame);
				soldiersBatch_->add(soldier->graphics.sprites());
			}
		}

		gfx::draw(soldiersBatch_);

		// draw bullets

		{
			bulletsBatch_->clear();

			size_t count = 0;

			for (int i = 0; i < MaxPlayers; i++)
			{
				if (players_[i].connected())
					count += players_[i].bullets().size();
			}

			if (bulletsBatch_->capacity() < count)
				bulletsBatch_->resize(count);

			for (int i = 0; i < MaxPlayers; i++)
			{
				if (players_[i].connected())
				{
					std::vector<ent::Bullet> &bullets = players_[i].bullets();

					for (size_t j = 0; j < bullets.size(); j++)
					{
						bullets[j].graphics.frame(frame);
						bulletsBatch_->add(bullets[j].graphics.sprite());
					}
				}
			}

			gfx::draw(bulletsBatch_);
		}

		// draw chat text

		for (int i = 0; i < MaxPlayers; i++)
		{
			if (playersText_[i].time > 0)
			{
				gfx::Text *text = playersText_[i].text;
				gfx::Text::Bounds bounds = text->bounds();

				const cmp::SoldierPhysics &physics = players_[i].soldier()->physics;
				const cmp::SoldierGraphics &graph = players_[i].soldier()->graphics;

				vec2 pos = graph.position;
				pos += vec2(2.0f, -20.0f);
				pos.y -= physics.bboxNormal.height().to_float();

				// if (physics.ducking())
				// 	pos.y -= physics.bboxDucked.height().to_float();
				// else
				// 	pos.y -= physics.bboxNormal.height().to_float();

				mat2d m = gfx::matrix();
				pos = m * pos;

				gfx::matrix(mat2d());

				gfx::translate(pos.x - 0.5f * bounds.width, pos.y);
				gfx::draw(text);

				gfx::matrix(m);
			}
		}

		for (int i = 0; i < MaxPlayers; i++)
		{
			if (players_[i].state() == Player::Playing)
			{
				ent::Soldier *soldier = players_[i].soldier();
				vec2 pos = soldier->graphics.position.get() + from_fixed(soldier->bodyOffset());

				float radius = soldier->physics.bboxNormal.height().to_float();

				if (glm::distance2(pos, target_) < radius * radius)
				{
					gfx::Text *text = &nicknamesText_[i];
					const gfx::Text::Bounds &bounds = text->bounds();

					pos = gfx::matrix() * (soldier->graphics.position.get() + vec2(0.0f, 20.0f));

					gfx::matrix(mat2d());
					gfx::translate(pos.x - 0.5f * bounds.width, pos.y);
					gfx::draw(text);

					break;
				}
			}
		}

		gfx::identity();
		healthBar_.draw();
	}
	else
	{
		int w, h;
		sys::framebuffer_size(&w, &h);

		const gfx::Text::Bounds &bounds = connectingText_.bounds();

		float x = 0.5f * w - 0.5f * bounds.width - bounds.x;
		float y = 0.5f * h - 0.5f * bounds.height - bounds.y;

		float intensity = 0.5f * std::sin(M_PI * sys::to_seconds(frame.time)) + 0.5f;

		connectingText_.color(gfx::Color(0xFF, glm::mix(0xA0, 0xFF, intensity)));

		gfx::matrix(mat2d::translate(x, y));
		gfx::draw(&connectingText_);
	}

	if (textInputMode_)
	{
		int width, height;
		sys::framebuffer_size(&width, &height);

		gfx::matrix(mat2d());
		gfx::draw(chatBackground_);
		gfx::translate(10.0f, height - 10.0f);
		gfx::draw(chatText_);
	}

	#ifdef DEBUG
		dbg->drawStateBuffers(tick_, InterpolationTicks, id_, players_);
		dbg->drawFontAtlas();
	#endif
}

bool Client::event(Event *evt)
{
	bool result = true;

	switch (evt->type)
	{
		case Event::Resized:
		{
			onResize(evt->size.fboWidth, evt->size.fboHeight);
		}
		break;

		case Event::FocusLost:
		{
			camera_.zoom(ent::Camera::ZoomNone);
		}
		break;

		case Event::MouseButtonPressed:
		{
			if (active())
			{
				if (players_[id_].state() == Player::Playing)
				{
					input_.onMousePress(evt->mouseButton);
				}
				else if (players_[id_].state() == Player::Dead)
				{
					msg::Ready ready;
					send(&ready, peer_); // TODO: check if it was already sent
				}
			}
		}
		break;

		case Event::MouseButtonReleased:
		{
			input_.onMouseRelease(evt->mouseButton);
		}
		break;

		case Event::KeyReleased:
		{
			if (evt->key.code == sys::NumpadAdd || evt->key.code == sys::NumpadSubtract)
				camera_.zoom(ent::Camera::ZoomNone);

			input_.onKeyRelease(evt->key);
		}
		break;

		case Event::KeyPressed:
		{
			bool esc = evt->key.code == sys::Escape;
			bool enter = evt->key.code == sys::Enter || evt->key.code == sys::NumpadEnter;

			if ((enter && active()) || (textInputMode_ && esc))
			{
				textInputMode_ = !textInputMode_;

				bool closedChat = !textInputMode_;

				if (closedChat && !esc && chatString_.size() > 0)
				{
					msg::Chat msg;
					msg.id = id_;
					hlp::utf8_encode(chatString_, msg.text);
					send(&msg, peer_);

					if (players_[id_].state() == Player::Playing)
					{
						playersText_[id_].time = sys::time<sys::Seconds>(5);
						playersText_[id_].text->value(chatString_);
					}
				}

				chatString_.clear();
				chatText_->value(chatString_ + caret_);

				camera_.zoom(ent::Camera::ZoomNone);

				result = false;
				break;
			}

			if (!textInputMode_)
			{
				input_.onKeyPress(evt->key);

				if (evt->key.code == sys::NumpadAdd)
					camera_.zoom(ent::Camera::ZoomIn);
				else if (evt->key.code == sys::NumpadSubtract)
					camera_.zoom(ent::Camera::ZoomOut);

				break;
			}
		}
		// go through

		case Event::KeyRepeat:
		{
			if (textInputMode_)
			{
				if (evt->key.code == sys::Backspace && chatString_.size() > 0)
				{
					chatString_.erase(chatString_.size() - 1, 1);
					chatText_->value(chatString_ + caret_);
				}

				result = false;
			}
		}
		break;

		case Event::TextEntered:
		{
			if (textInputMode_)
			{
				if (chatString_.size() < 100)
				{
					chatString_ += evt->text.ch;
					chatText_->value(chatString_ + caret_);
				}

				result = false;
			}
		}
		break;

		default: break;
	}

	return result;
}

void Client::onResize(int width, int height)
{
	if (width == 0 || height == 0)
		return;

	camera_.viewport(width, height);

	gfx::ColorVertex vertex[4];

	float w = (float)width;
	float h = (float)height;

	vertex[0] = gfx::color_vertex(0.0f, 0.0f, gfx::Color(0xF9, 0xFE, 0xEE));
	vertex[1] = gfx::color_vertex(w   , 0.0f, gfx::Color(0xF9, 0xFE, 0xEE));
	vertex[2] = gfx::color_vertex(w   , h   , gfx::Color(0xFD, 0xFD, 0x63));
	vertex[3] = gfx::color_vertex(0.0f, h   , gfx::Color(0xFD, 0xFD, 0x63));

	background_->set(vertex, 0, 4);

	vertex[0] = gfx::color_vertex(0.0f, h - 30.0f, gfx::Color(64, 200));
	vertex[1] = gfx::color_vertex(w   , h - 30.0f, gfx::Color(64, 200));
	vertex[2] = gfx::color_vertex(w   , h        , gfx::Color(64, 200));
	vertex[3] = gfx::color_vertex(0.0f, h        , gfx::Color(64, 200));

	chatBackground_->set(vertex, 0, 4);
}

}} // g13::net
