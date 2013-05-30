#include "Message.h"
#include "NickMessage.h"
#include "StartMessage.h"

namespace net
{
	Message::Message(const uint8_t *dat, uint16_t len) : data(dat), length(len) {}

	uint8_t Message::type() const { return length > 0 ? data[0] : TypeCount; }

	bool Message::validate()
	{
		switch (type())
		{
			case Nick:         return NickMessage::validate(this);
			case SpawnRequest: return false;
			case Input:        return false;
			case Start:        return StartMessage::validate(this);
			case PlayerJoin:   return false;
			case PlayerLeave:  return false;
			case Spawn:        return false;
			case GameState:    return false;
			default:           return false;
		}
	}
}
