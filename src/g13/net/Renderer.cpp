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

	kills.font(font);
	kills.size(16);
	kills.color(gfx::Color(255, 255, 0));
	kills.outlineColor(gfx::Color(255, 0, 0));
	kills.outlineWidth(1.2f);
	kills.value("Kills 0");

	damageTime = 0.0f;
	damageOverlay.allocate<gfx::ColorVertex>(6, gfx::Stream);
	damageOverlay.mode(gfx::TriangleFan);

	#ifdef DBG_SHOW_CURRENT_HULL
	dbg_hull.allocate<gfx::ColorVertex>(6, gfx::Stream);
	dbg_hull.mode(gfx::Lines);
	#endif

	dbg_rope.allocate<gfx::ColorVertex>(2, gfx::Stream);
	dbg_rope.mode(gfx::Lines);
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
					pos.y -= fpm::to_float(physics.bboxNormal.height());
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

			{
				const gfx::Text::Bounds &bounds = kills.bounds();
				gfx::matrix(mat2d::translate(camera.viewportWidth() - bounds.width - 20.0f, 30.0f));
				gfx::draw(kills);
			}

			if (localPlayer->state != Player::Playing && client->matchInfo_.playing)
			{
				const gfx::Text::Bounds &bounds = clickToPlay.bounds();

				float x = -bounds.x + 0.5f * (camera.viewportWidth() - bounds.width);
				float y = -bounds.y + 0.5f * (camera.viewportHeight() - bounds.height);

				gfx::matrix(mat2d::translate(x, y));
				gfx::draw(clickToPlay);
			}

			gfx::identity();
			chatbox.draw(frame);

			// damage overlay

			if (damageTime > 0.0f)
			{
				const uint8_t alpha = (uint8_t)(128 * damageTime);

				gfx::ColorVertex vertices[] = {
					gfx::color_vertex(0.5f, 0.5f, gfx::Color(255, 0, 0,     0)),
					gfx::color_vertex(0.0f, 0.0f, gfx::Color(255, 0, 0, alpha)),
					gfx::color_vertex(1.0f, 0.0f, gfx::Color(255, 0, 0, alpha)),
					gfx::color_vertex(1.0f, 1.0f, gfx::Color(255, 0, 0, alpha)),
					gfx::color_vertex(0.0f, 1.0f, gfx::Color(255, 0, 0, alpha)),
					gfx::color_vertex(0.0f, 0.0f, gfx::Color(255, 0, 0, alpha))
				};

				damageOverlay.set(vertices, 0, countof(vertices));

				damageTime -= sys::to_seconds(frame.delta) * 2.0f;

				gfx::identity();
				gfx::scale(camera.viewportWidth(), camera.viewportHeight());
				gfx::draw(damageOverlay);
			}

			#ifdef DBG_SHOW_CURRENT_HULL
			if (localPlayer && localPlayer->soldier.physics.segment != 0)
			{
				const coll::Segment (&s)[3] = localPlayer->soldier.physics.hull.segments;

				const gfx::Color color[2] = {
					gfx::Color(255, 0, 0),
					gfx::Color(0, 0, 255)
				};

				gfx::ColorVertex vertices[] = {
					gfx::color_vertex(fpm::to_float(s[0].line.p1.x), fpm::to_float(s[0].line.p1.y), color[s[0].floor]),
					gfx::color_vertex(fpm::to_float(s[0].line.p2.x), fpm::to_float(s[0].line.p2.y), color[s[0].floor]),
					gfx::color_vertex(fpm::to_float(s[1].line.p1.x), fpm::to_float(s[1].line.p1.y), color[s[1].floor]),
					gfx::color_vertex(fpm::to_float(s[1].line.p2.x), fpm::to_float(s[1].line.p2.y), color[s[1].floor]),
					gfx::color_vertex(fpm::to_float(s[2].line.p1.x), fpm::to_float(s[2].line.p1.y), color[s[2].floor]),
					gfx::color_vertex(fpm::to_float(s[2].line.p2.x), fpm::to_float(s[2].line.p2.y), color[s[2].floor])
				};

				dbg_hull.set(vertices, 0, countof(vertices));

				gfx::matrix(camera.matrix());
				gfx::draw(dbg_hull);
			}
			#endif

			if (localPlayer && !localPlayer->soldier.rope.idle())
			{
				const vec2 a = from_fixed(localPlayer->soldier.center());
				const vec2 b = from_fixed(localPlayer->soldier.rope.position);

				gfx::ColorVertex vertices[] = {
					gfx::color_vertex(a.x, a.y, gfx::Color(0, 0, 255)),
					gfx::color_vertex(b.x, b.y, gfx::Color(0, 0, 255))
				};

				dbg_rope.set(vertices, 0, countof(vertices));

				gfx::matrix(camera.matrix());
				gfx::draw(dbg_rope);
			}
		}
		break;

		default: break;
	}
}

void Renderer::drawNickname(Player *player, const vec2 &target)
{
	if (player->state != Player::Playing)
		return;

	const float radius = fpm::to_float(player->soldier.physics.bboxNormal.height());
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

void Renderer::onPlayerDamage(Player *attacker, Player *victim)
{
	if (client->isLocalPlayer(victim))
		health.setPercent(victim->health / (float)MaxHealth);
}

void Renderer::onPlayerKill(Player *attacker, Player *victim)
{
	chat[victim->id].time = MaxChatTime;

	if (client->isLocalPlayer(victim))
	{
		sys::set_cursor(0);
	}
	else if (client->isLocalPlayer(attacker))
	{
		char str[32];
		sprintf(str, "Kills %d", attacker->kills);
		kills.value(hlp::utf8_decode(str));
	}
}

void Renderer::onPlayerChat(Player *player, const char *text)
{
	if (player->state == Player::Playing)
	{
		chat[player->id].time = 0;
		chat[player->id].text.value(hlp::utf8_decode(text));
	}
}

void Renderer::onPlayerChat(Player *player, const string32_t &text)
{
	if (player->state == Player::Playing)
	{
		chat[player->id].time = 0;
		chat[player->id].text.value(text);
	}
}

void Renderer::onMatchStart()
{
	sys::set_cursor(0);
	kills.value("Kills 0");
}

void Renderer::onPlayerHit(Player *player)
{
	if (client->isLocalPlayer(player))
		damageTime = 1.0f;
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
