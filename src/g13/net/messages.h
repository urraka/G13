// Important: this file shall not have #pragma once, it's included multiple times.

namespace g13 {
namespace net {

MESSAGES_BEGIN()

//******************************************************************************
// Client
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
	uint32_t tick;
	uint16_t angle;
	bool rightwards;
	bool left;
	bool right;
	bool jump;
	bool run;
	bool duck;
BEGIN
	Integer(tick)
	Integer(angle)
	Bool(rightwards)
	Bool(left)
	Bool(right)
	Bool(jump)
	Bool(run)
	Bool(duck)
END

MESSAGE(Chat)
	uint8_t id;
	char    text[1024];
BEGIN
	Bits(id, MINBITS(Multiplayer::MaxPlayers - 1))
	String(text, 1)
END

//******************************************************************************
// Server
//******************************************************************************

LIST(PlayerIds, item)
	Bits(item, MINBITS(Multiplayer::MaxPlayers - 1))
LISTEND

MESSAGE(ServerInfo)
	uint32_t tick;
	uint8_t  clientId;
	uint32_t nPlayers;
	uint8_t  players[Multiplayer::MaxPlayers];
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
BEGIN
	Bits(id, MINBITS(Multiplayer::MaxPlayers - 1))
END

MESSAGE(PlayerJoin)
	uint32_t tick;
	uint8_t  id;
	fixvec2  position;
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
		uint8_t tickOffset;
		uint8_t playerId;
		cmp::SoldierState state;
	};

	uint32_t tick;
	uint32_t nSoldiers;
	SoldierState soldiers[Multiplayer::MaxPlayers];
BEGIN
	Integer(tick)
	List(SoldierState, soldiers, nSoldiers, 0)
END

MESSAGES_END()

}} // g13::net
