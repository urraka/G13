#include "Multiplayer.h"
#include "msg.h"

#include <g13/Map.h>

namespace g13 {
namespace net {

Multiplayer::Multiplayer()
	:	tick_(0),
		connection_(0),
		dataPool_(0),
		map_(0)
{
	dataPool_ = new hlp::pool<msg::Storage>();

	for (size_t i = 0; i < MaxPlayers; i++)
		players_[i].id_ = i;
}

Multiplayer::~Multiplayer()
{
	if (connection_ != 0)
		enet_host_destroy(connection_);

	if (map_) delete map_;

	delete dataPool_;
}

void Multiplayer::update(Time dt)
{
	ENetEvent event;

	while (enet_host_service(connection_, &event, 0) > 0)
	{
		switch (event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				onConnect(event.peer);
			}
			break;

			case ENET_EVENT_TYPE_DISCONNECT:
			{
				onDisconnect(event.peer);
			}
			break;

			case ENET_EVENT_TYPE_RECEIVE:
			{
				msg::Message *msg = msg::create(event.packet->data, event.packet->dataLength);

				if (msg != 0)
				{
					onMessage(msg, event.peer);
					msg::destroy(msg);
				}
				else
				{
					debug_log("[Multiplayer] Invalid message received.");
				}

				enet_packet_destroy(event.packet);
			}
			break;

			default: break;
		}

		if (connection_ == 0)
			return;
	}
}

void Multiplayer::loadMap()
{
	if (map_ != 0)
		delete map_;

	map_ = new Map();
	map_->load();
}

void Multiplayer::send(msg::Message *msg, ENetPeer *target)
{
	uint8_t  channel;
	uint32_t packetFlags = ENET_PACKET_FLAG_NO_ALLOCATE;

	if (msg->type() == msg::GameState::Type)
	{
		channel = UnsequencedChannel;
		packetFlags |= ENET_PACKET_FLAG_UNSEQUENCED;
	}
	else
	{
		channel = ReliableChannel;
		packetFlags |= ENET_PACKET_FLAG_RELIABLE;
	}

	msg::Storage *storage = dataPool_->alloc();
	size_t length = msg->serialize(storage->data, sizeof storage->data / sizeof storage->data[0]);

	ENetPacket *packet = enet_packet_create(storage->data, length, packetFlags);

	if (!packet)
	{
		dataPool_->free(storage);
		return;
	}

	packet->freeCallback = free_packet;
	packet->userData = this;

	if (target != 0)
		enet_peer_send(target, channel, packet);
	else
		enet_host_broadcast(connection_, channel, packet);
}

void Multiplayer::free_packet(ENetPacket *packet)
{
	Multiplayer *self = (Multiplayer*)packet->userData;
	self->dataPool_->free((msg::Storage*)packet->data);
}

}} // net
