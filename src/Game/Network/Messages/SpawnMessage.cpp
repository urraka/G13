#include "Message.h"
#include "SpawnMessage.h"
#include "../Server.h"
#include "../DataStream.h"
#include <assert.h>

namespace net
{
	bool SpawnMessage::validate(const Message *msg)
	{
		assert(msg->type() == Message::Spawn);

		if (msg->length != sizeof(SpawnMessage::data_))
			return false;

		if (msg->data[1] > Server::MaxPeers)
			return false;

		return true;
	}

	void SpawnMessage::serialize(Message *msg)
	{
		DataWriter w(data_, sizeof(data_));

		w << (uint8_t)Message::Spawn;
		w << id;
		w << tick;
		w << position.x.value();
		w << position.y.value();

		msg->data = data_;
		msg->length = w.tell();

		assert(validate(msg));
	}

	void SpawnMessage::unserialize(const Message *msg)
	{
		assert(validate(msg));

		DataReader r(msg->data, msg->length);

		int32_t x, y;

		r.seek(1, DataStream::Begin);
		r >> id;
		r >> tick;
		r >> x;
		r >> y;

		position.x = fixed::from_value(x);
		position.y = fixed::from_value(y);
	}
}
