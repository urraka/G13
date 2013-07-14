#pragma once

#include "Player.h"

#include <g13/g13.h>
#include <hlp/pool.h>
#include <enet/enet.h>

namespace g13 {
	class Map;

	namespace net {
	namespace msg {
		class  Message;
		struct Storage;
	}}
}

namespace g13 {
namespace net {

class Multiplayer
{
public:
	Multiplayer();
	virtual ~Multiplayer();
	virtual void update(sys::Time dt);

	static const size_t MaxPlayers = 32;

protected:
	uint32_t tick_;
	ENetHost *connection_;
	hlp::pool<msg::Storage> *dataPool_;

	Map *map_;
	Player players_[MaxPlayers];

	void loadMap();
	void send(msg::Message *msg, ENetPeer *target = 0); // target = 0 -> broadcast

	virtual void onConnect   (ENetPeer *peer) = 0;
	virtual void onDisconnect(ENetPeer *peer) = 0;
	virtual void onMessage   (msg::Message *msg, ENetPeer *from) = 0;

	static void free_packet(ENetPacket *packet);

	enum Channels { ReliableChannel = 0, UnsequencedChannel, ChatChannel, ChannelsCount };
};

}} // g13::net
