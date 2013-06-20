// This file is included twice from msg.cpp, where all the weird syntax
// is defined through #defines. One pass is to implement serialization
// and the other to implement unserialization (Msg::serialize() and
// Msg::read(), respectively)

namespace net { namespace msg
{
	// client messages

	MESSAGE(Login)
		String(nick, 1)
	END

	MESSAGE(SpawnRequest)
		/* empty */
	END

	MESSAGE(Input)
		Integer(tick)
		Bits(input, 5)
	END

	// TODO: server messages
}}
