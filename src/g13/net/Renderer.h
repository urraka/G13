#pragma once

#include <g13/g13.h>
#include <g13/Map.h>
#include <g13/Camera.h>
#include <g13/ui/Bar.h>
#include <gfx/gfx.h>

#include "constants.h"

namespace g13 {
namespace net {

class Renderer
{
public:
	Renderer(Client *client);
	~Renderer();

	// methods

	void initialize();
	void draw(const Frame &frame);
	void drawNickname(Player *player, const vec2 &target);
	bool onEvent(const sys::Event &event);
	void onResize(int width, int height);

	void onPlayerConnect(Player *player);
	void onPlayerJoin(Player *player);
	void onPlayerDamage(Player *attacker, Player *victim);
	void onPlayerKill(Player *attacker, Player *victim);
	void onPlayerChat(Player *player, const char *text);
	void onPlayerChat(Player *player, const string32_t &text);
	void onMatchStart();
	void onPlayerHit(Player *player);

	// structs

	struct ChatBox
	{
		bool       active;
		string32_t value;
		gfx::Text  text;
		gfx::VBO   background;

		void draw(const Frame &frame);
		void resize(float width, float height);

		void show();
		void hide();

		bool onEvent(const sys::Event &event);
	};

	struct PlayerText
	{
		Time time;
		gfx::Text text;
	};

	// member variables

	Client *client;

	g13::Map map;
	g13::Camera camera;

	gfx::SpriteBatch soldiers;
	gfx::SpriteBatch ropes;
	gfx::SpriteBatch bullets;

	gfx::Text connecting;
	gfx::Text clickToPlay;

	ChatBox    chatbox;
	PlayerText chat[MaxPlayers];
	gfx::Text  nicknames[MaxPlayers];

	ui::Bar health;
	gfx::Text kills;

	gfx::VBO damageOverlay;
	float damageTime;

	#ifdef DBG_SHOW_CURRENT_HULL
	gfx::VBO dbg_hull;
	#endif

	gfx::VBO dbg_rope;
};

}} // g13::net
