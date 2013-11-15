#pragma once

#include "Player.h"

#include <g13/g13.h>
#include <g13/ent/Bullet.h>
#include <hlp/pool.h>
#include <enet/enet.h>
#include <vector>

namespace g13 {
namespace net {

class Multiplayer
{
public:
	Multiplayer();
	virtual ~Multiplayer();

	static const int MaxPlayers = 32;

protected:
	enum Channels
	{
		ReliableChannel = 0,
		UnsequencedChannel,
		ChatChannel,
		ChannelsCount
	};

	int tick_;
	ENetHost *connection_;
	hlp::pool<msg::Storage> *dataPool_;

	Map *map_;
	Player players_[MaxPlayers];
	std::vector<ent::Bullet> bullets_;

	enum Callback
	{
		CreateBullet = 0,
		PlayerBulletCollision,
		CallbackCount
	};

	void setCallback(Callback id, cbk::Callback *callback)
	{
		callbacks_[id] = callback;
	}

	cbk::Callback *getCallback(Callback id)
	{
		return callbacks_[id];
	}

	void pollEvents();
	void loadMap();
	void send(msg::Message *msg, ENetPeer *target = 0); // target = 0 -> broadcast

	virtual void onConnect      (ENetPeer *peer) = 0;
	virtual void onDisconnect   (ENetPeer *peer) = 0;
	virtual void onMessage      (msg::Message *msg, ENetPeer *from) = 0;

	static void free_packet(ENetPacket *packet);

	void updateBullets(Time dt);

private:
	cbk::Callback *callbacks_[CallbackCount];
};

}} // g13::net
