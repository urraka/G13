#include "Connection.h"
#include "Peer.h"
#include "constants.h"
#include "msg/msg.h"

#include <hlp/pool.h>
#include <enet/enet.h>

namespace g13 {
namespace net {

static hlp::pool<msg::Storage> storage_pool;

static void free_packet(ENetPacket *packet)
{
	storage_pool.free((msg::Storage*)packet->data);
}

static const uint32_t *packet_flags()
{
	static uint32_t flags[ChannelCount];

	flags[ReliableChannel]    = ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE;
	flags[UnsequencedChannel] = ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_UNSEQUENCED;
	flags[ChatChannel]        = ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE;

	return flags;
}

Connection::Connection()
	:	connection_(0),
		server_(0),
		timeout_(0),
		stopping_(false)
{
}

Connection::~Connection()
{
	reset();
}

bool Connection::listen(int port, int channels, int maxPeers)
{
	assert(connection_ == 0);

	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;

	connection_ = enet_host_create(&address, maxPeers, channels, 0, 0);

	if (connection_ != 0)
		enet_host_compress_with_range_coder((ENetHost*)connection_);

	return connection_ != 0;
}

void Connection::stop()
{
	assert(connection_ != 0 && server_ == 0);

	if (stopping_)
		return;

	stopping_ = true;

	ENetHost *conn = (ENetHost*)connection_;

	for (size_t i = 0; i < conn->peerCount; i++)
	{
		if (conn->peers[i].state != ENET_PEER_STATE_DISCONNECTED)
			enet_peer_disconnect(&conn->peers[i], 0);
	}
}

void Connection::connect(const char *host, int port, int channels)
{
	assert(connection_ == 0);

	connection_ = enet_host_create(0, 1, channels, 0, 0);
	enet_host_compress_with_range_coder((ENetHost*)connection_);

	ENetAddress address;
	enet_address_set_host(&address, host);
	address.port = port;

	server_ = enet_host_connect((ENetHost*)connection_, &address, channels, 0);
}

void Connection::disconnect()
{
	if (server_ != 0)
		enet_peer_disconnect((ENetPeer*)server_, 0);
}

void Connection::reset()
{
	if (connection_ != 0)
		enet_host_destroy((ENetHost*)connection_);

	connection_ = 0;
	server_ = 0;
	timeout_ = 0;
	stopping_ = false;
}

void Connection::timeout(int timeout)
{
	assert(connection_ != 0);

	timeout_ = timeout;

	if (server_ != 0)
	{
		enet_peer_timeout((ENetPeer*)server_, 0, timeout, timeout);
	}
	else
	{
		ENetHost *conn = (ENetHost*)connection_;

		for (size_t i = 0; i < conn->peerCount; i++)
		{
			if (conn->peers[i].state != ENET_PEER_STATE_DISCONNECTED)
				enet_peer_timeout(&conn->peers[i], 0, timeout, timeout);
		}
	}
}

void Connection::poll()
{
	assert(connection_ != 0);

	ENetEvent event;

	while (enet_host_service((ENetHost*)connection_, &event, 0) > 0)
	{
		switch (event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				if (stopping_)
				{
					enet_peer_disconnect_now(event.peer, 0);
					break;
				}

				if (server_ == 0)
					enet_peer_timeout(event.peer, 0, timeout_, timeout_);

				onConnect(Peer(event.peer));
			}
			break;

			case ENET_EVENT_TYPE_DISCONNECT:
			{
				onDisconnect(Peer(event.peer));
			}
			break;

			case ENET_EVENT_TYPE_RECEIVE:
			{
				msg::Message *msg = msg::create(event.packet->data, event.packet->dataLength);

				if (msg != 0)
				{
					onMessage(msg, Peer(event.peer));
					msg::destroy(msg);
				}
				else
				{
					debug_log("[Connection] Invalid message received.");
				}

				enet_packet_destroy(event.packet);
			}
			break;

			default: break;
		}

		if (connection_ == 0)
			return;
	}

	if (stopping_ && ((ENetHost*)connection_)->connectedPeers == 0)
	{
		onStop();
		reset();
	}
}

void Connection::sendMessage(const msg::Message *msg, Peer target)
{
	assert(connection_ != 0);

	static const uint32_t *packet_flags = net::packet_flags();

	msg::Storage *storage = storage_pool.alloc();
	size_t        length = msg->serialize(storage->data, sizeof(storage->data));
	ENetPacket   *packet = enet_packet_create(storage->data, length, packet_flags[msg->channel]);

	if (packet == 0)
	{
		storage_pool.free(storage);
		return;
	}

	packet->freeCallback = free_packet;

	if (target.handle != 0)
		enet_peer_send((ENetPeer*)target.handle, msg->channel, packet);
	else
		enet_host_broadcast((ENetHost*)connection_, msg->channel, packet);
}

void Connection::flush()
{
	assert(connection_ != 0);
	enet_host_flush((ENetHost*)connection_);
}

}} // net
