#include "Multiplayer.h"
#include "msg.h"
#include "../Map.h"

namespace net
{
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

					enet_packet_destroy(event.packet);
				}
				break;

				default: break;
			}
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

		enet_peer_send(target, channel, packet);
	}

	void Multiplayer::free_packet(ENetPacket *packet)
	{
		Multiplayer *self = (Multiplayer*)packet->userData;
		self->dataPool_->free((msg::Storage*)packet->data);
	}
}
