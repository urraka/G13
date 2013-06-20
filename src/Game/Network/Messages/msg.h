#include "../../../Math/math.h"

#include <assert.h>
#include <stdint.h>

namespace net { namespace msg
{
	class Message;

	Message *create(const uint8_t *data, size_t length);
	void destroy(Message *msg);

	class Message
	{
	public:
		enum Type
		{
			Login = 0,
			SpawnRequest,
			Input,

			Start,
			PlayerJoin,
			PlayerLeave,
			Spawn,
			GameState,

			TypeCount
		};

		virtual Type type() const = 0;
		virtual size_t serialize(uint8_t *data, size_t length) const = 0;
	};
}}

#define MESSAGE(T) \
	class T : public Message \
	{ \
	public: \
		Type type() const { return Message::T; } \
		bool read(const uint8_t *data, size_t length); \
		size_t serialize(uint8_t *data, size_t length) const;

#define END };

#include "msgdecl.h"

#undef MESSAGE
#undef END
