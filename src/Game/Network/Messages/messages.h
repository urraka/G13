// Important: this file shall not have #pragma once, it's included multiple times.

// Temporary crap.
// Constants will be static memebers of classes (Server::MaxPlayer, etc)
#ifndef MaxPlayers
#define MaxPlayers 32
#define MaxNameLen 32
#define MinNameLen 1
#endif

namespace net
{

MESSAGES_BEGIN()

//******************************************************************************
// Client
//******************************************************************************

MESSAGE(Login)
	char name[MaxNameLen + 1];
BEGIN
	String(name, MinNameLen)
END

MESSAGE(Ready)
BEGIN
END

MESSAGE(Input)
	uint32_t tick;
	uint8_t input;
BEGIN
	Integer(tick)
	Bits(input, 5)
END

//******************************************************************************
// Server
//******************************************************************************

LIST(PlayerIds, item)
	Bits(item, MINBITS(MaxPlayers - 1))
LISTEND

MESSAGE(Info)
	uint32_t tick;
	size_t   nPlayers;
	uint8_t  players[MaxPlayers];
BEGIN
	Integer(tick)
	List(PlayerIds, players, nPlayers, 0)
END

MESSAGE(Join)
	uint8_t id;
	char    name[MaxNameLen + 1];
BEGIN
	Bits(id, MINBITS(MaxPlayers - 1))
	String(name, MinNameLen)
END

MESSAGE(Leave)
	uint8_t id;
BEGIN
	Bits(id, MINBITS(MaxPlayers - 1))
END

MESSAGE(Spawn)
	uint32_t tick;
	uint8_t  playerId;
BEGIN
	Integer(tick)
	Bits(playerId, MINBITS(MaxPlayers - 1))
END

MESSAGE(State)
BEGIN
END

MESSAGES_END()

}
