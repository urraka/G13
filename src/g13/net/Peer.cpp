#include "Peer.h"
#include <enet/enet.h>
#include <assert.h>

namespace g13 {
namespace net {

void Peer::disconnect(bool graceful)
{
	assert(handle != 0);

	if (graceful)
		enet_peer_disconnect((ENetPeer*)handle, 0);
	else
		enet_peer_disconnect_now((ENetPeer*)handle, 0);
}

int Peer::rtt()
{
	assert(handle != 0);
	return ((ENetPeer*)handle)->roundTripTime;
}

void *Peer::data()
{
	assert(handle != 0);
	return ((ENetPeer*)handle)->data;
}

void Peer::setData(void *data)
{
	assert(handle != 0);
	((ENetPeer*)handle)->data = data;
}

}} // g13::net
