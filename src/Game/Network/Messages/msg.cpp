#include "msg.h"
#include "../BitStream.h"

namespace
{
	// This stuff here holds an instance of each message type to avoid allocating.
	// Sort of a 1 object pool, which can be done since only one message lives
	// at a time. If this fact ever changes it will assert.

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

	template<class T> net::msg::Message *msg_create(const uint8_t *data, size_t length)
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

	template<class T> void msg_destroy(T*)
	{
		msgcont<T>::get().used = false;
	}
}

namespace net { namespace msg
{
	// Message "factory" to get an instance of the message type from packet data.

	Message *create(const uint8_t *data, size_t length)
	{
		if (length == 0)
			return 0;

		switch (data[0] >> (8 - MINBITS(Message::TypeCount - 1)))
		{
			case Message::Login:        return msg_create <Login>        (data, length);
			case Message::SpawnRequest: return msg_create <SpawnRequest> (data, length);
			case Message::Input:        return msg_create <Input>        (data, length);
			/*case Message::Start:        return msg_create <Start>        (data, length);
			case Message::PlayerJoin:   return msg_create <PlayerJoin>   (data, length);
			case Message::PlayerLeave:  return msg_create <PlayerLeave>  (data, length);
			case Message::Spawn:        return msg_create <Spawn>        (data, length);
			case Message::GameState:    return msg_create <GameState>    (data, length);*/

			default:           return 0;
		}
	}

	void destroy(Message *msg)
	{
		if (msg == 0)
			return;

		switch (msg->type())
		{
			case Message::Login:        msg_destroy((Login       *) msg); return;
			case Message::SpawnRequest: msg_destroy((SpawnRequest*) msg); return;
			case Message::Input:        msg_destroy((Input       *) msg); return;
			/*case Message::Start:        msg_destroy((Start       *) msg); return;
			case Message::PlayerJoin:   msg_destroy((PlayerJoin  *) msg); return;
			case Message::PlayerLeave:  msg_destroy((PlayerLeave *) msg); return;
			case Message::Spawn:        msg_destroy((Spawn       *) msg); return;
			case Message::GameState:    msg_destroy((GameState   *) msg); return;*/

			default:           assert(false);
		}
	}
}}

// from here everything that follows is black magic

#define POSSIBLY_UNUSED(expr) do { (void)(expr); } while (0)

// reading

#define R_LISTDEF(_ListType, _item) \
	namespace \
	{ \
		template<class T> bool _r_ ## _ListType (BitReader &_r, T *_list, size_t &_count, size_t _min, const size_t _sizeBits) \
		{ \
			size_t _strlen = 0; \
			int32_t _i32; \
			POSSIBLY_UNUSED(_strlen); \
			POSSIBLY_UNUSED(_i32); \
			\
			if (!_r.has(_sizeBits)) \
				return false; \
			\
			_r.read(&_count, _sizeBits); \
			\
			if (_count < _min) \
				return false;\
			\
			for (size_t _i = 0; _i < _count; _i++) \
			{ \
				T & _item = _list[_i];

#define R_ENDLIST \
			} \
			\
			return true; \
		} \
	}

#define R_MESSAGE(_T) \
	bool _T::read(const uint8_t *_data, size_t _length) \
	{ \
		BitReader _r(_data, _length); \
		size_t _strlen = 0; \
		int32_t _i32; \
		POSSIBLY_UNUSED(_strlen); \
		POSSIBLY_UNUSED(_i32); \
		_r.skip(MINBITS(Message::TypeCount - 1));

#define R_NUMBER(_varname) \
		if (!_r.has(sizeof(_varname) * 8)) return false; \
		_r.read(&_varname);

#define R_BITS(_varname, _bits) \
		if (!_r.has(_bits)) return false; \
		_r.read(&_varname, _bits);

#define R_STRING(_varname, _min) \
		_strlen = _r.read(_varname, sizeof(_varname)); \
		if (_strlen < _min || _strlen == sizeof(_varname)) return false;

#define R_FIXED(_varname) \
		if (!r.has(sizeof(int32_t) * 8)) return false; \
		_r.read(&_i32); \
		_varname = fixed::from_value(_i32);

#define R_LIST(_ListType, _listvar, _countvar, _min) \
		if (! _r_ ## _ListType (_r, _listvar, _countvar, _min, MINBITS(sizeof(_listvar) / sizeof(_listvar[0])))) return false;

#define R_END \
		if (_r.bitpos() + (_r.bitpos() % 8 != 0) < _length) \
			return false; \
		return true; \
	}

// writing/serializing

#define W_LISTDEF(_ListType, _item) \
	namespace \
	{ \
		template<class T> void _w_ ## _ListType (BitWriter &_w, const T *_list, size_t _count, const size_t _sizeBits) \
		{ \
			_w.write(_count, _sizeBits); \
			\
			for (size_t _i = 0; _i < _count; _i++) \
			{ \
				const T & _item = _list[_i];

#define W_ENDLIST \
			} \
		} \
	}

#define W_MESSAGE(_T) \
	size_t _T::serialize(uint8_t *_data, size_t _length) const \
	{ \
		BitWriter _w(_data, _length); \
		_w.write((uint8_t)type(), MINBITS(Message::TypeCount - 1));

#define W_NUMBER(_varname) \
		_w.write(_varname);

#define W_BITS(_varname, _bits) \
		_w.write(_varname, _bits);

#define W_STRING(_varname, _min) \
		assert(_varname[sizeof(_varname) - 1] == 0); \
		_w.write(_varname);

#define W_FIXED(_varname) \
		_w.write(_varname.value());

#define W_LIST(_ListType, _listvar, _countvar, _min) \
		_w_ ## _ListType (_w, _listvar, _countvar, MINBITS(sizeof(_listvar) / sizeof(_listvar[0])));

#define W_END \
		return _w.bitpos() + (_w.bitpos() % 8 != 0); \
	}

#define LIST    R_LISTDEF
#define ENDLIST R_ENDLIST
#define MESSAGE R_MESSAGE
#define END     R_END
#define Integer R_NUMBER
#define Bits    R_BITS
#define String  R_STRING
#define Fixed   R_FIXED
#define List    R_LIST

#include "msgimpl.inl"

#undef LIST
#undef ENDLIST
#undef MESSAGE
#undef END
#undef Integer
#undef Bits
#undef String
#undef Fixed
#undef List

#define LIST    W_LISTDEF
#define ENDLIST W_ENDLIST
#define MESSAGE W_MESSAGE
#define END     W_END
#define Integer W_NUMBER
#define Bits    W_BITS
#define String  W_STRING
#define Fixed   W_FIXED
#define List    W_LIST

#include "msgimpl.inl"
