#pragma once

#include <g13/g13.h>
#include <g13/coll/World.h>
#include <g13/cmp/BulletParams.h>
#include <vector>

#include "Connection.h"
#include "Player.h"
#include "ServerPlayer.h"
#include "Ticked.h"
#include "constants.h"

namespace g13 {
namespace net {

class Server : private Connection
{
public:
	enum State
	{
		Running,
		Stopping,
		Stopped
	};

	bool start(int port);
	void stop();
	void update(Time dt);

	State state() const
	{
		return state_;
	}

private:
	int tick_;
	State state_;

	coll::World world_;

	std::vector<ServerPlayer*> players_;
	std::vector<ServerPlayer*> connectingPlayers_;
	std::vector<ServerPlayer*> disconnectingPlayers_; // already disconnected, but wait for bullets, etc
	std::vector<ServerPlayer*> freePlayers_;

	ServerPlayer playersStorage_[MaxPlayers];

	typedef std::vector<Ticked<cmp::BulletParams> > BulletQueue;

	BulletQueue bulletQueue_; // used to broadcast bullet messages

	// helper methods

	void initialize();
	void loadMap(const char *name);

	void updatePlayers(Time dt);
	void updateBullets(Time dt);
	void updateConnectingPlayers();
	void updateDisconnectingPlayers();
	void updatePlayerBullets(Time dt, ServerPlayer *player);

	void sendBullets();
	void sendBulletsTo(const ServerPlayer *targetPlayer);
	void sendGameState();
	void sendGameStateTo(const ServerPlayer *targetPlayer);
	void sendLeaveMessage(const ServerPlayer *player);
	void sendDamageMessage(const ServerPlayer *player, uint16_t amount);
	void sendServerInfoMessage(const ServerPlayer *player);
	void sendPlayerInfoMessages(const ServerPlayer *player);
	void sendBulletInfoMessages(const ServerPlayer *player);
	void sendPlayerConnectMessage(const ServerPlayer *player);
	void sendPlayerDisconnectMessage(const ServerPlayer *player);
	void sendPlayerJoinMessage(const ServerPlayer *player, const fixvec2 &position);

	template<typename T> void sendToConnectedPlayers(const T &msg, const ServerPlayer *exception = 0);

	ServerPlayer *getPlayerById(int id);

	// local events/callbacks

	void onSpawnBullet(void *data);
	void onPlayerBulletCollision(void *data);

	// net events

	void onConnect(Peer peer);
	void onDisconnect(Peer peer);
	void onMessage(const msg::Message *msg, Peer from);
	void onStop();

	void onLogin(ServerPlayer *player, const msg::Login &msg);
	void onPong(ServerPlayer *player, const msg::Pong &msg);
	void onJoinRequest(ServerPlayer *player, const msg::JoinRequest &msg);
	void onInput(ServerPlayer *player, const msg::Input &msg);
	void onPlayerChat(ServerPlayer *player, const msg::PlayerChat &msg);
};

}} // g13::net
