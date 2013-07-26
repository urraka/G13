#pragma once

#include "Multiplayer.h"

#include <g13/g13.h>
#include <g13/ent/Camera.h>

#include <gfx/forward.h>
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
	void draw(float framePercent);
	bool event(Event *evt);

	bool active() const;
	State state() const;

private:
	State state_;
	ENetPeer *peer_;

	uint8_t id_;
	char name_[Player::MaxNameLength + 1];
	cmp::SoldierInput input_;

	int connectingCount_;

	ent::Camera camera_;
	gfx::VBO *background_;
	gfx::Texture *texture_;
	gfx::SpriteBatch *spriteBatch_;
	gfx::Text *chatText_;
	gfx::VBO *chatBackground_;

	bool textInputMode_;
	std::basic_string<uint32_t> chatString_;
	std::basic_string<uint32_t> caret_;

	struct PlayerText
	{
		PlayerText() : time(0), text(0) {}
		Time time;
		gfx::Text *text;
	};

	PlayerText playersText_[MaxPlayers];

	void onConnect   (ENetPeer *peer);
	void onDisconnect(ENetPeer *peer);
	void onMessage   (msg::Message *msg, ENetPeer *from);

	void onServerInfo(msg::Message *msg);
	void onPlayerConnect(msg::Message *msg);
	void onPlayerDisconnect(msg::Message *msg);
	void onPlayerJoin(msg::Message *msg);
	void onPlayerChat(msg::Message *msg);
	void onGameState(msg::Message *msg);

	void onResize(int width, int height);
};

}} // g13::net
