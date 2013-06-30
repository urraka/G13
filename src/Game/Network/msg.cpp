#include "msg.h"
#include "BitStream.h"

namespace net {
namespace msg {
namespace _
{
	// This stuff here holds an instance of each message type to avoid allocating.
	// Sort of a 1 object pool, which can be done since only one message lives
	// at a time. If this fact ever changes ASSERT!

	template<class T> class msgcont
	{
	public:
		msgcont() : used(false) {}
		bool used;
		T msg;

		static msgcont<T> &get()
		{
			static msgcont<T> container;
			return container;
		}
	};

	template<class T> static net::msg::Message *create(const uint8_t *data, size_t length)
	{
		msgcont<T> &container = msgcont<T>::get();

		assert(!container.used);

		if (container.msg.read(data, length))
		{
			container.used = true;
			return &container.msg;
		}

		return 0;
	}

	template<class T> static void destroy(T*)
	{
		msgcont<T>::get().used = false;
	}
}}}

#define MSGMAGIC_CREATE
#include "msg_magic.h"

#define MSGMAGIC_DESTROY
#include "msg_magic.h"

#define MSGMAGIC_SERIALIZE
#include "msg_magic.h"

#define MSGMAGIC_UNSERIALIZE
#include "msg_magic.h"
