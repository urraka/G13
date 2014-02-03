#pragma once

#include <g13/g13.h>
#include <g13/coll/World.h>
#include <gfx/Color.h>
#include <string>
#include <vector>

#include "Player.h"
#include "LocalPlayer.h"
#include "RemotePlayer.h"
#include "Connection.h"
#include "Renderer.h"
#include "constants.h"

namespace g13 {
namespace net {

class Client : private Connection
{
public:
	Client();

	// enums

	enum State
	{
		Disconnected,
		Connecting,
		Connected,
		Disconnecting
	};

	// methods

	void connect(const char *host, int port);
	void disconnect();
	void update(Time dt);
	bool onEvent(const sys::Event &event);
	void draw(const Frame &frame);

	void setNick(const char *nick)         { nickname_ = nick; }
	void setNick(const std::string &nick)  { nickname_ = nick; }
	void setColor(const gfx::Color &color) { color_ = color;   }

	State state() const { return state_; }

private:
	int tick_;
	State state_;

	std::string nickname_;
	gfx::Color  color_;

	coll::World world_;

	LocalPlayer *localPlayer_;
	LocalPlayer  localPlayerStorage_;

	std::vector<RemotePlayer*> remotePlayers_;
	RemotePlayer remotePlayersStorage_[MaxPlayers];

	Renderer renderer_;

	struct ConnectingProgress
	{
		int remainingPlayers;
		bool ready() { return remainingPlayers == 0; }
	};

	ConnectingProgress connectingProgress_;

	bool joinRequestSent_;

	// helper methods

	void initialize();
	void loadMap(const char *name);

	void updateLocalPlayer(Time dt);
	void updateRemotePlayers(Time dt);
	void updateBullets(Time dt);

	void sendInputMessage();
	void sendLoginMessage();
	void sendPongMessage();
	void sendJoinRequestMessage();
	void sendPlayerChatMessage(const string32_t &text);

	Player *getPlayerById(int id);
	bool isLocalPlayer(int id) const;
	bool isLocalPlayer(const Player *player) const;

	// local events/callbacks

	void onSpawnBullet(void *data);
	void onPlayerBulletCollision(void *data);

	// net events

	void onConnect(Peer);
	void onDisconnect(Peer);
	void onMessage(const msg::Message *msg, Peer);
	void onStop() {}

	void onConnectionReady();
	void onServerInfo(const msg::ServerInfo &msg);
	void onPlayerInfo(const msg::PlayerInfo &msg);
	void onPlayerConnect(const msg::PlayerConnect &msg);
	void onPlayerDisconnect(const msg::PlayerDisconnect &msg);
	void onPlayerJoin(const msg::PlayerJoin &msg);
	void onPlayerLeave(const msg::PlayerLeave &msg);
	void onPlayerChat(const msg::PlayerChat &msg);
	void onGameState(const msg::GameState &msg);
	void onBullet(const msg::Bullet &msg);
	void onDamage(const msg::Damage &msg);

	friend class Renderer;
};

}} // g13::net
