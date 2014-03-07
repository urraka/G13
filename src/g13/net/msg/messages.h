// Important: this file shall not have #pragma once, it's included multiple times.

namespace g13 {
namespace net {

MESSAGES_BEGIN()

//******************************************************************************
// Client <-> Server
//******************************************************************************

MESSAGE(PlayerChat, ChatChannel)
	uint8_t id;
	char    text[1024];
BEGIN
	Bits(id, MINBITS(MaxPlayers - 1))
	String(text, 1)
END

//******************************************************************************
// Client -> Server
//******************************************************************************

MESSAGE(Login, ReliableChannel)
	char name[MaxNickLength * 4 + 1];
	uint8_t color[3];
BEGIN
	String(name, MinNickLength)
	Integer(color[0])
	Integer(color[1])
	Integer(color[2])
END

MESSAGE(Pong, ReliableChannel)
BEGIN
END

MESSAGE(JoinRequest, ReliableChannel)
BEGIN
END

MESSAGE(Input, ReliableChannel)
	int32_t tick;
	uint16_t angle;
	bool rightwards;
	bool left;
	bool right;
	bool jump;
	bool run;
	bool duck;
	bool shoot;
	bool rope;
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
	Bool(rope)
END

//******************************************************************************
// Server -> Client
//******************************************************************************

MESSAGE(ServerInfo, ReliableChannel)
	int32_t tick;
	uint8_t clientId;
	uint8_t nPlayers;
	int32_t matchStartTick;
	bool    matchPlaying;

	vars::Vars vars;
BEGIN
	Integer(tick)
	Bits(clientId, MINBITS(MaxPlayers - 1))
	Bits(nPlayers, MINBITS(MaxPlayers - 1))
	Integer(matchStartTick)
	Bool(matchPlaying)

	Fixed(vars.Gravity)
	Fixed(vars.JumpVel)
	Fixed(vars.WalkVel)
	Fixed(vars.RunVel)
	Fixed(vars.AirMoveVel)
	Fixed(vars.MoveAcc)
	Fixed(vars.AirMoveAcc)
	Fixed(vars.BreakAcc)
	Fixed(vars.AirBreakAcc)
	Fixed(vars.LimitAcc)
	Fixed(vars.MaxFloorSlope)
	Fixed(vars.MaxRopeLength)
	Fixed(vars.RopeVel)
END

MESSAGE(PlayerInfo, ReliableChannel)
	uint8_t  id;
	char     name[MaxNickLength * 4 + 1];
	uint8_t  color[3];
	bool     playing;
	uint16_t health;
	uint16_t kills;
	uint16_t deaths;
	uint32_t connectTick;
	uint32_t currentTick;
	cmp::SoldierState soldierState;
BEGIN
	Bits(id, MINBITS(MaxPlayers - 1))
	String(name, MinNickLength)
	Integer(color[0])
	Integer(color[1])
	Integer(color[2])
	Bool(playing)
	Integer(health)
	Integer(kills)
	Integer(deaths)
	Integer(connectTick)
	Integer(currentTick)
	Fixed(soldierState.position.x)
	Fixed(soldierState.position.y)
	Fixed(soldierState.velocity.x)
	Fixed(soldierState.velocity.y)
	Integer(soldierState.angle)
	Bool(soldierState.rightwards)
	Bool(soldierState.duck)
	Bool(soldierState.floor)
	Bool(soldierState.hooked)
	Fixed(soldierState.hook.x)
	Fixed(soldierState.hook.y)
END

MESSAGE(PlayerConnect, ReliableChannel)
	int32_t tick;
	uint8_t id;
	char    name[MaxNickLength * 4 + 1];
	uint8_t color[3];
BEGIN
	Integer(tick)
	Bits(id, MINBITS(MaxPlayers - 1))
	String(name, MinNickLength)
	Integer(color[0])
	Integer(color[1])
	Integer(color[2])
END

MESSAGE(PlayerDisconnect, ReliableChannel)
	int32_t tick;
	uint8_t id;
BEGIN
	Integer(tick)
	Bits(id, MINBITS(MaxPlayers - 1))
END

MESSAGE(PlayerJoin, ReliableChannel)
	int32_t tick;
	uint8_t id;
	fixvec2 position;
BEGIN
	Integer(tick)
	Bits(id, MINBITS(MaxPlayers - 1))
	Fixed(position.x)
	Fixed(position.y)
END

MESSAGE(PlayerLeave, ReliableChannel)
	int32_t tick;
	uint8_t id;
BEGIN
	Integer(tick)
	Bits(id, MINBITS(MaxPlayers - 1))
END

LIST(SoldierState, soldier)
	Bits(soldier.tickOffset, MINBITS(MaxTickOffset))
	Bits(soldier.playerId, MINBITS(MaxPlayers - 1))
	Fixed(soldier.state.position.x)
	Fixed(soldier.state.position.y)
	Fixed(soldier.state.velocity.x)
	Fixed(soldier.state.velocity.y)
	Integer(soldier.state.angle)
	Bool(soldier.state.rightwards)
	Bool(soldier.state.duck)
	Bool(soldier.state.floor)
	Bool(soldier.state.hooked)
	Fixed(soldier.state.hook.x)
	Fixed(soldier.state.hook.y)
LISTEND

MESSAGE(GameState, UnsequencedChannel)
	struct SoldierState {
		int tickOffset;
		uint8_t playerId;
		cmp::SoldierState state;
	};

	int32_t tick;
	int nSoldiers;
	SoldierState soldiers[MaxPlayers];
BEGIN
	Integer(tick)
	List(SoldierState, soldiers, nSoldiers, 0)
END

LIST(BulletInfo, bullet)
	Bits(bullet.tickOffset, MINBITS(MaxTickOffset))
	Bits(bullet.params.playerid, MINBITS(MaxPlayers - 1))
	Fixed(bullet.params.position.x)
	Fixed(bullet.params.position.y)
	Fixed(bullet.params.speed)
	Fixed(bullet.params.angle)
LISTEND

MESSAGE(Bullet, UnsequencedChannel)
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

MESSAGE(Damage, ReliableChannel)
	int32_t  tick;
	bool     hasAttacker;
	uint8_t  attacker;
	uint8_t  victim;
	uint16_t amount;
BEGIN
	Integer(tick)
	Bool(hasAttacker)
	Bits(attacker, MINBITS(MaxPlayers - 1))
	Bits(victim, MINBITS(MaxPlayers - 1))
	Integer(amount)
END

MESSAGE(MatchStart, ReliableChannel)
	int32_t tick;
BEGIN
END

MESSAGE(MatchEnd, ReliableChannel)
	int32_t tick;
BEGIN
END

MESSAGES_END()

}} // g13::net
