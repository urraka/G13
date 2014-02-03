#include "Renderer.h"
#include "Client.h"
#include "constants.h"

#include <g13/g13.h>
#include <g13/res.h>
#include <g13/cmp/SoldierGraphics.h>
#include <hlp/utf8.h>
#include <glm/gtx/norm.hpp>

namespace g13 {
namespace net {

static const Time MaxChatTime = sys::time<sys::Milliseconds>(ChatTime);

Renderer::Renderer(Client *client)
{
	this->client = client;

	soldiers.texture(res::texture(res::Soldier));
	soldiers.resize(MaxPlayers * cmp::SoldierGraphics::SpriteCount, gfx::Stream);

	bullets.texture(res::texture(res::Bullet));
	bullets.resize(512, gfx::Stream);

	gfx::Font *font = res::font(res::DefaultFont);

	connecting.font(font);
	connecting.size(16);
	connecting.color(gfx::Color(0xFF));
	connecting.value("Connecting...");

	clickToPlay.font(font);
	clickToPlay.size(16);
	clickToPlay.color(gfx::Color(0xFF));
	clickToPlay.outlineColor(gfx::Color(0x00));
	clickToPlay.outlineWidth(0.5f);
	clickToPlay.value("Click To Play");

	const int size = 11;

	{
		font->size(size);
		font->outlineWidth(0);

		for (int i = 32; i < 127; i++)
			font->glyph(i);

		font->outlineWidth(0.5f);

		for (int i = 32; i < 127; i++)
			font->glyph(i);
	}

	chatbox.text.font(font);
	chatbox.text.size(size);
	chatbox.text.color(gfx::Color(0xFF));
	chatbox.background.allocate<gfx::ColorVertex>(4, gfx::Static);
	chatbox.background.mode(gfx::TriangleFan);

	for (size_t i = 0; i < countof(chat); i++)
	{
		chat[i].time = MaxChatTime;
		chat[i].text.font(font);
		chat[i].text.size(size);
		chat[i].text.color(gfx::Color(0xFF));
		chat[i].text.outlineColor(gfx::Color(0x00));
		chat[i].text.outlineWidth(0.5f);
	}

	for (size_t i = 0; i < countof(nicknames); i++)
	{
		nicknames[i].font(font);
		nicknames[i].size(size);
		nicknames[i].color(gfx::Color(0xFF));
		nicknames[i].outlineColor(gfx::Color(0x00));
		nicknames[i].outlineWidth(0.5f);
	}

	health.setPosition(15.0f, 15.0f);
	health.setSize(100.0f, 15.0f);
	health.setOutline(1.0f, gfx::Color(0));
	health.setOpacity(0.7f);
}

Renderer::~Renderer()
{
	sys::set_cursor(0);
}

void Renderer::initialize()
{
	sys::set_cursor(0);

	onResize(sys::framebuffer_width(), sys::framebuffer_height());
}

void Renderer::draw(const Frame &frame)
{
	gfx::clear();

	switch (client->state())
	{
		case Client::Connecting:
		{
			const gfx::Text::Bounds &bounds = connecting.bounds();

			float x = -bounds.x + 0.5f * (camera.viewportWidth() - bounds.width);
			float y = -bounds.y + 0.5f * (camera.viewportHeight() - bounds.height);

			float intensity = 0.5f * std::sin(M_PI * sys::to_seconds(frame.time)) + 0.5f;

			connecting.color(gfx::Color(0xFF, glm::mix(0xA0, 0xFF, intensity)));

			gfx::identity();
			gfx::translate(x, y);
			gfx::draw(connecting);
		}
		break;

		case Client::Connected:
		{
			LocalPlayer *localPlayer = client->localPlayer_;

			// update camera and target

			vec2 target = camera.matrixinv() * vec2(sys::mousex(), sys::mousey());

			if (localPlayer->state == Player::Playing)
			{
				localPlayer->soldier.graphics.frame(frame, target);
				camera.target(glm::mix(localPlayer->soldier.graphics.position(), target, 0.5f));
			}

			camera.frame(frame);

			// draw map

			map.draw(frame, camera);

			// draw soldiers

			soldiers.clear();

			for (size_t i = 0; i < client->remotePlayers_.size(); i++)
			{
				if (client->remotePlayers_[i]->state == Player::Playing)
				{
					ent::Soldier &soldier = client->remotePlayers_[i]->soldier;

					soldier.graphics.frame(frame);
					soldiers.add(soldier.graphics.sprites());
				}
			}

			if (localPlayer->state == Player::Playing)
				soldiers.add(localPlayer->soldier.graphics.sprites());

			gfx::draw(soldiers);

			// draw bullets

			bullets.clear();

			size_t bulletCount = localPlayer->bullets.size();

			for (size_t i = 0; i < client->remotePlayers_.size(); i++)
				bulletCount += client->remotePlayers_[i]->bullets.size();

			if (bullets.capacity() < bulletCount)
				bullets.resize(bulletCount);

			for (size_t i = 0; i < client->remotePlayers_.size(); i++)
			{
				RemotePlayer *player = client->remotePlayers_[i];

				for (size_t j = 0; j < player->bullets.size(); j++)
				{
					player->bullets[j].graphics.frame(frame);
					bullets.add(player->bullets[j].graphics.sprite());
				}
			}

			for (size_t i = 0; i < localPlayer->bullets.size(); i++)
			{
				localPlayer->bullets[i].graphics.frame(frame);
				bullets.add(localPlayer->bullets[i].graphics.sprite());
			}

			gfx::draw(bullets);

			// chat text

			for (size_t id = 0; id < countof(chat); id++)
			{
				Player *player = client->getPlayerById(id);

				if (player->state == Player::Playing && chat[id].time < MaxChatTime)
				{
					const gfx::Text::Bounds &bounds = chat[id].text.bounds();

					const cmp::SoldierPhysics &physics = player->soldier.physics;
					const cmp::SoldierGraphics &graph = player->soldier.graphics;

					vec2 pos = graph.position();
					pos += vec2(2.0f, -20.0f);
					pos.y -= physics.bboxNormal.height().to_float();
					pos = camera.matrix() * pos;

					gfx::identity();
					gfx::translate(pos.x - 0.5f * bounds.width, pos.y);
					gfx::draw(chat[id].text);

					chat[id].time += frame.delta;
				}
			}

			// nicknames

			drawNickname(localPlayer, target);

			for (size_t i = 0; i < client->remotePlayers_.size(); i++)
				drawNickname(client->remotePlayers_[i], target);

			// ui

			gfx::identity();
			health.draw();

			if (localPlayer->state != Player::Playing)
			{
				// TODO: click to play

				const gfx::Text::Bounds &bounds = clickToPlay.bounds();

				float x = -bounds.x + 0.5f * (camera.viewportWidth() - bounds.width);
				float y = -bounds.y + 0.5f * (camera.viewportHeight() - bounds.height);

				gfx::matrix(mat2d::translate(x, y));
				gfx::draw(clickToPlay);
			}

			gfx::identity();
			chatbox.draw(frame);
		}
		break;

		default: break;
	}
}

void Renderer::drawNickname(Player *player, const vec2 &target)
{
	if (player->state != Player::Playing)
		return;

	const float radius = player->soldier.physics.bboxNormal.height().to_float();
	const vec2 &position = player->soldier.graphics.position();
	const vec2 center = position + from_fixed(player->soldier.bodyOffset());

	if (glm::distance2(center, target) < radius * radius)
	{
		const gfx::Text::Bounds &bounds = nicknames[player->id].bounds();

		vec2 pos = camera.matrix() * (position + vec2(0.0f, 20.0f));

		gfx::identity();
		gfx::translate(pos.x - 0.5f * bounds.width, pos.y);
		gfx::draw(nicknames[player->id]);
	}
}

bool Renderer::onEvent(const sys::Event &event)
{
	if (!chatbox.onEvent(event))
		return false;

	if (event.type == sys::Resize)
		onResize(event.size.fboWidth, event.size.fboHeight);

	return true;
}

void Renderer::onResize(int width, int height)
{
	if (width == 0 || height == 0)
		return;

	camera.viewport(width, height);
	chatbox.resize(width, height);
}

void Renderer::onPlayerConnect(Player *player)
{
	player->soldier.graphics.setBodyColor(player->color);
	nicknames[player->id].value(hlp::utf8_decode(player->nickname.c_str()));
}

void Renderer::onPlayerJoin(Player *player)
{
	if (client->isLocalPlayer(player))
	{
		health.setPercent(player->health / (float)MaxHealth);
		camera.setPosition(player->soldier.graphics.position());

		sys::set_cursor(res::cursor(res::Crosshair));
	}
}

void Renderer::onPlayerDie(Player *player)
{
	chat[player->id].time = MaxChatTime;

	if (client->isLocalPlayer(player))
		sys::set_cursor(0);
}

void Renderer::onPlayerChat(Player *player, const char *text)
{
	chat[player->id].time = 0;
	chat[player->id].text.value(hlp::utf8_decode(text));
}

void Renderer::onPlayerChat(Player *player, const string32_t &text)
{
	chat[player->id].time = 0;
	chat[player->id].text.value(text);
}

void Renderer::onPlayerDamage(Player *player)
{
	if (client->isLocalPlayer(player))
		health.setPercent(player->health / (float)MaxHealth);
}


// ChatBox

void Renderer::ChatBox::draw(const Frame &frame)
{
	if (active)
	{
		gfx::draw(background);
		gfx::translate(10.0f, sys::framebuffer_height() - 10.0f);
		gfx::draw(text);
	}
}

void Renderer::ChatBox::resize(float w, float h)
{
	gfx::ColorVertex vertices[] = {
		gfx::color_vertex(0.0f, h - 30.0f, gfx::Color(64, 200)),
		gfx::color_vertex(w   , h - 30.0f, gfx::Color(64, 200)),
		gfx::color_vertex(w   , h        , gfx::Color(64, 200)),
		gfx::color_vertex(0.0f, h        , gfx::Color(64, 200))
	};

	background.set(vertices, 0, countof(vertices));
}

void Renderer::ChatBox::show()
{
	active = true;

	value.clear();
	text.value("_");
}

void Renderer::ChatBox::hide()
{
	active = false;
}

bool Renderer::ChatBox::onEvent(const sys::Event &event)
{
	if (!active)
		return true;

	if (event.type == sys::Text)
	{
		if (value.size() < 100)
		{
			value += event.text.ch;
			text.value(value + (uint32_t)'_');
		}

		return false;
	}

	if (event.type == sys::KeyPress || event.type == sys::KeyRepeat)
	{
		if (event.key.code == sys::Backspace && value.size() > 0)
		{
			value.resize(value.size() - 1);
			text.value(value + (uint32_t)'_');
		}

		if (event.key.code != sys::Enter && event.key.code != sys::Escape)
			return false;
	}

	return true;
}

}} // g13::net
