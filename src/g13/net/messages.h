// Important: this file shall not have #pragma once, it's included multiple times.

namespace g13 {
namespace net {

MESSAGES_BEGIN()

//******************************************************************************
// Client <-> Server
//******************************************************************************

MESSAGE(Chat)
	uint8_t id;
	char    text[1024];
BEGIN
	Bits(id, MINBITS(Multiplayer::MaxPlayers - 1))
	String(text, 1)
END

//******************************************************************************
// Client -> Server
//******************************************************************************

MESSAGE(Login)
	char name[Player::MaxNameLength * 4 + 1];
BEGIN
	String(name, Player::MinNameLength)
END

MESSAGE(Ready)
BEGIN
END

MESSAGE(Input)
	int32_t tick;
	uint16_t angle;
	bool rightwards;
	bool left;
	bool right;
	bool jump;
	bool run;
	bool duck;
	bool shoot;
BEGIN
	Integer(tick)
	Integer(angle)
	Bool(rightwards)
	Bool(left)
	Bool(right)
	Bool(jump)
	Bool(run)
	Bool(duck)
	Bool(shoot)
END

//******************************************************************************
// Server -> Client
//******************************************************************************

LIST(PlayerIds, item)
	Bits(item, MINBITS(Multiplayer::MaxPlayers - 1))
LISTEND

MESSAGE(ServerInfo)
	int32_t tick;
	uint8_t clientId;
	int     nPlayers;
	uint8_t players[Multiplayer::MaxPlayers];
BEGIN
	Integer(tick)
	Bits(clientId, MINBITS(Multiplayer::MaxPlayers - 1))
	List(PlayerIds, players, nPlayers, 0)
END

MESSAGE(PlayerConnect)
	uint8_t id;
	char    name[Player::MaxNameLength * 4 + 1];
BEGIN
	Bits(id, MINBITS(Multiplayer::MaxPlayers - 1))
	String(name, Player::MinNameLength)
END

MESSAGE(PlayerDisconnect)
	uint8_t id;
	int32_t tick;
BEGIN
	Bits(id, MINBITS(Multiplayer::MaxPlayers - 1))
	Integer(tick)
END

MESSAGE(PlayerJoin)
	int32_t tick;
	uint8_t id;
	fixvec2 position;
BEGIN
	Integer(tick)
	Bits(id, MINBITS(Multiplayer::MaxPlayers - 1))
	Fixed(position.x)
	Fixed(position.y)
END

LIST(SoldierState, soldier)
	Bits(soldier.tickOffset, MINBITS(Player::MaxTickOffset))
	Bits(soldier.playerId, MINBITS(Multiplayer::MaxPlayers - 1))
	Fixed(soldier.state.position.x)
	Fixed(soldier.state.position.y)
	Fixed(soldier.state.velocity.x)
	Fixed(soldier.state.velocity.y)
	Integer(soldier.state.angle)
	Bool(soldier.state.rightwards)
	Bool(soldier.state.duck)
	Bool(soldier.state.floor)
LISTEND

MESSAGE(GameState)
	struct SoldierState {
		int tickOffset;
		uint8_t playerId;
		cmp::SoldierState state;
	};

	int32_t tick;
	int nSoldiers;
	SoldierState soldiers[Multiplayer::MaxPlayers];
BEGIN
	Integer(tick)
	List(SoldierState, soldiers, nSoldiers, 0)
END

LIST(BulletInfo, bullet)
	Bits(bullet.tickOffset, MINBITS(Player::MaxTickOffset))
	Bits(bullet.params.playerid, MINBITS(Multiplayer::MaxPlayers - 1))
	Fixed(bullet.params.position.x)
	Fixed(bullet.params.position.y)
	Fixed(bullet.params.speed)
	Fixed(bullet.params.angle)
LISTEND

MESSAGE(Bullet)
	struct BulletInfo {
		int tickOffset;
		cmp::BulletParams params;
	};

	int32_t tick;
	int nBullets;
	BulletInfo bullets[32];
BEGIN
	Integer(tick)
	List(BulletInfo, bullets, nBullets, 1)
END

MESSAGE(Damage)
	int32_t  tick;
	uint8_t  playerId;
	uint16_t amount;
BEGIN
	Integer(tick)
	Bits(playerId, MINBITS(Multiplayer::MaxPlayers - 1))
	Integer(amount)
END

MESSAGES_END()

}} // g13::net
