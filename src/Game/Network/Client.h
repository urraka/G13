#pragma once

#include "Multiplayer.h"
#include "../Entities/Camera.h"

#include <sys/sys.h>
#include <gfx/forward.h>
#include <string>

namespace net {

class Client : public Multiplayer
{
public:
	enum State { Disconnected, Connecting, Connected };

	Client();
	~Client();

	bool connect(const char *host, int port);
	void disconnect();
	void update(sys::Time dt);
	void draw(float framePercent);
	void event(sys::Event *evt);

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

	void onConnect   (ENetPeer *peer);
	void onDisconnect(ENetPeer *peer);
	void onMessage   (msg::Message *msg, ENetPeer *from);

	void onServerInfo(msg::Message *msg);
	void onPlayerConnect(msg::Message *msg);
	void onPlayerDisconnect(msg::Message *msg);
	void onPlayerJoin(msg::Message *msg);
	void onGameState(msg::Message *msg);

	void onResize(int width, int height);
	void onKeyPressed(int key);
};

} // net
