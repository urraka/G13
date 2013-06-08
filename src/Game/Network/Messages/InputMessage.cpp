#include "Message.h"
#include "InputMessage.h"
#include "../DataStream.h"
#include <assert.h>

namespace net
{
	bool InputMessage::validate(const Message *msg)
	{
		assert(msg->type() == Message::Input);

		if (msg->length != sizeof(InputMessage::data_))
			return false;

		return true;
	}

	void InputMessage::serialize(Message *msg)
	{
		DataWriter w(data_, sizeof(data_));

		w << (uint8_t)Message::Input;
		w << tick;
		w << input;

		msg->data = data_;
		msg->length = w.tell();

		assert(validate(msg));
	}

	void InputMessage::unserialize(const Message *msg)
	{
		assert(validate(msg));

		DataReader r(msg->data, msg->length);

		r.seek(1, DataStream::Begin);

		r >> tick;
		r >> input;
	}
}
