#pragma once

namespace g13 {
namespace net {

class Peer
{
public:
	Peer() : handle(0) {}
	Peer(void *h) : handle(h) {}

	void  disconnect(bool graceful = true);
	int   rtt();
	void *data();
	void  setData(void *data);

	void *handle;
};

}} // g13::net
