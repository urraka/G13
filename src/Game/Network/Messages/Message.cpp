#include "Message.h"
#include "NickMessage.h"
#include "InputMessage.h"
#include "StartMessage.h"
#include "PlayerJoinMessage.h"
#include "PlayerLeaveMessage.h"
#include "SpawnMessage.h"
#include "GameStateMessage.h"

namespace net
{
	Message::Message(const uint8_t *dat, uint16_t len) : data(dat), length(len) {}

	uint8_t Message::type() const { return length > 0 ? data[0] : TypeCount; }

	bool Message::validate()
	{
		switch (type())
		{
			case Nick:         return NickMessage::validate(this);
			case SpawnRequest: assert(false); return false;
			case Input:        return InputMessage::validate(this);
			case Start:        return StartMessage::validate(this);
			case PlayerJoin:   return PlayerJoinMessage::validate(this);
			case PlayerLeave:  return PlayerLeaveMessage::validate(this);
			case Spawn:        return SpawnMessage::validate(this);
			case GameState:    return GameStateMessage::validate(this);
			default:           return false;
		}
	}
}
