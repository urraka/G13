#pragma once

#include "Multiplayer.h"

#include <g13/g13.h>
#include <g13/ent/Camera.h>
#include <g13/ui/Bar.h>
#include <gfx/gfx.h>
#include <string>

namespace g13 {
namespace net {

class Client : public Multiplayer
{
public:
	enum State { Disconnected, Connecting, Connected };

	Client();
	~Client();

	bool connect(const char *host, int port);
	void disconnect();
	void update(Time dt);
	void draw(const Frame &frame);
	bool event(Event *evt);

	bool active() const;
	State state() const;

	void nick(const string32_t &nick);
	void soldierColor(const gfx::Color &color) { soldierColor_ = color; }

private:
	State state_;
	ENetPeer *peer_;

	uint8_t id_;
	char name_[Player::MaxNameLength * 4 + 1];
	gfx::Color soldierColor_;
	cmp::SoldierInput input_;
	vec2 target_;
	vec2 cameraTarget_;

	int connectingCount_;

	ent::Camera camera_;
	gfx::VBO *background_;
	gfx::SpriteBatch *soldiersBatch_;
	gfx::SpriteBatch *bulletsBatch_;
	gfx::Text *chatText_;
	gfx::VBO *chatBackground_;
	gfx::Text connectingText_;

	ui::Bar healthBar_;

	bool textInputMode_;
	string32_t chatString_;
	string32_t caret_;

	struct PlayerText
	{
		PlayerText() : time(0), text(0) {}
		Time time;
		gfx::Text *text;
	};

	PlayerText playersText_[MaxPlayers];
	gfx::Text nicknamesText_[MaxPlayers];

	void onConnect(ENetPeer *peer);
	void onDisconnect(ENetPeer *peer);
	void onMessage(msg::Message *msg, ENetPeer *from);

	void onServerInfo(msg::ServerInfo *info);
	void onPlayerConnect(msg::PlayerConnect *playerConnect);
	void onPlayerDisconnect(msg::PlayerDisconnect *playerDisconnect);
	void onPlayerJoin(msg::PlayerJoin *playerJoin);
	void onPlayerChat(msg::Chat *chat);
	void onGameState(msg::GameState *gameState);
	void onBullet(msg::Bullet *bullet);
	void onDamage(msg::Damage *damage);

	void onResize(int width, int height);

	void createBullet(void *data);
	void playerBulletCollision(void *data);
};

}} // g13::net
