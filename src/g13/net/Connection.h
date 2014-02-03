#pragma once

#include <g13/g13.h>
#include "Peer.h"

namespace g13 {
namespace net {

class Connection
{
public:
	Connection();
	virtual ~Connection();

	bool listen(int port, int channels, int maxPeers);
	void stop();
	void connect(const char *host, int port, int channels);
	void disconnect();
	void reset();
	void timeout(int timeout);
	void poll();
	void flush();

	template<typename T> void send(const T &msg)              { sendMessage(&msg, Peer(server_)); }
	template<typename T> void send(const T &msg, Peer target) { sendMessage(&msg, target);        }
	template<typename T> void broadcast(const T &msg)         { sendMessage(&msg, Peer(0));       }

	virtual void onConnect(Peer peer) = 0;
	virtual void onDisconnect(Peer peer) = 0;
	virtual void onMessage(const msg::Message *msg, Peer from) = 0;
	virtual void onStop() = 0;

private:
	void *connection_;
	void *server_;
	int   timeout_;
	bool  stopping_;

	void sendMessage(const msg::Message *msg, Peer target);
};

}} // g13::net
