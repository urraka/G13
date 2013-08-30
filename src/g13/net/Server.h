#pragma once

#include "Multiplayer.h"
#include <g13/g13.h>
#include <g13/cmp/BulletParams.h>

namespace g13 {
namespace net {

namespace msg {
	class Login;
	class Ready;
	class Input;
	class Chat;
}

class Server : public Multiplayer
{
public:
	enum State { Running, Stopping, Stopped };

	Server();

	bool start(int port);
	void stop();
	void update(Time dt);

	State state() const;

private:
	State state_;

	std::vector<cmp::BulletParams> createdBullets_;

	void onConnect   (ENetPeer *peer);
	void onDisconnect(ENetPeer *peer);
	void onMessage   (msg::Message *msg, ENetPeer *from);

	void onPlayerLogin(Player *player, msg::Login *login);
	void onPlayerReady(Player *player, msg::Ready *ready);
	void onPlayerInput(Player *player, msg::Input *input);
	void onPlayerChat (Player *player, msg::Chat  *chat);

	void onBulletCreated(const cmp::BulletParams &params);
};

}} // net
