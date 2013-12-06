#pragma once

#include "Multiplayer.h"
#include <g13/g13.h>
#include <g13/cmp/BulletParams.h>

#include "Ticked.h"

namespace g13 {
namespace net {

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
	typedef Ticked<cmp::BulletParams> BulletParams;

	State state_;

	std::vector<BulletParams> createdBullets_;

	void onConnect   (ENetPeer *peer);
	void onDisconnect(ENetPeer *peer);
	void onMessage   (msg::Message *msg, ENetPeer *from);

	void onPlayerLogin(Player *player, msg::Login *login);
	void onPlayerPong (Player *player, msg::Pong  *pong);
	void onPlayerReady(Player *player, msg::Ready *ready);
	void onPlayerInput(Player *player, msg::Input *input);
	void onPlayerChat (Player *player, msg::Chat  *chat);

	void createBullet(void *data);
	void playerBulletCollision(void *data);
};

}} // net
