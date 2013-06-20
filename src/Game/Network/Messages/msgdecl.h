// This file is included from msg.h, where MESSAGE/END are #defined
// to make each message type a class derived from msg::Message.

namespace net { namespace msg
{
	// client messages

	MESSAGE(Login)
		char nick[20];
	END

	MESSAGE(SpawnRequest)
		/* empty */
	END

	MESSAGE(Input)
		uint32_t tick;
		uint8_t input;
	END

	// server messages
}}
