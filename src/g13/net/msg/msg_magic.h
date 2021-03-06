#if !defined(MSGMAGIC_UNDEF)

#define NOTHING(x)
#define UNUSED(expr)          do { (void)(expr); } while (0)

//******************************************************************************
#if defined(MSGMAGIC_DECLARE)
//******************************************************************************

#define MESSAGES_BEGIN()                                                                       \
    namespace msg {                                                                            \
    namespace _ {                                                                              \
        static const size_t type_base = __COUNTER__ + 1;                                       \
    }

#define MESSAGES_END()                                                                         \
    namespace _ {                                                                              \
        static const size_t type_count = __COUNTER__ - type_base;                              \
    }} // msg::_

#define MESSAGE(T, Channel)                                                                    \
    class T : public Message                                                                   \
    {                                                                                          \
    public:                                                                                    \
        static const int Type = __COUNTER__ - _::type_base;                                    \
        T() : Message(Type, Channel) {}                                                        \
        bool read(const uint8_t *data, size_t length);                                         \
        size_t serialize(uint8_t *data, size_t length) const;

#define END                                                                                    \
    };

#define BEGIN         NOTHING(x)
#define LIST(x,y)     NOTHING(x)
#define LISTEND       NOTHING(x)
#define Integer(x)    NOTHING(x)
#define Bool(x)       NOTHING(x)
#define String(x,y)   NOTHING(x)
#define Bits(x,y)     NOTHING(x)
#define Fixed(x)      NOTHING(x)
#define List(x,y,z,w) NOTHING(x)

#include "messages.h"
#undef MSGMAGIC_DECLARE
#define MSGMAGIC_UNDEF
#include "msg_magic.h"

//******************************************************************************
#elif defined(MSGMAGIC_CREATE)
//******************************************************************************

#define MESSAGES_BEGIN()                                                                       \
    namespace msg                                                                              \
    {                                                                                          \
        Message *create(const uint8_t *data, size_t length)                                    \
        {                                                                                      \
            uint8_t type;                                                                      \
            hlp::BitReader r(data, length);                                                    \
                                                                                               \
            if (!r.has(MINBITS(_::type_count - 1)))                                            \
                return 0;                                                                      \
                                                                                               \
            r.read(&type, MINBITS(_::type_count - 1));                                         \
                                                                                               \
            switch (type)                                                                      \
            {

#define MESSAGES_END()                                                                         \
                default: return 0;                                                             \
            }                                                                                  \
        }                                                                                      \
    }

#define MESSAGE(T, C)                                                                          \
    case T::Type: return _::create<T>(data, length);                                           \
        {                                                                                      \
            struct {

#define END                                                                                    \
            } _unused;                                                                         \
            UNUSED(_unused);                                                                   \
        }

#define BEGIN         NOTHING(x)
#define LIST(x,y)     NOTHING(x)
#define LISTEND       NOTHING(x)
#define Integer(x)    NOTHING(x)
#define Bool(x)       NOTHING(x)
#define String(x,y)   NOTHING(x)
#define Bits(x,y)     NOTHING(x)
#define Fixed(x)      NOTHING(x)
#define List(x,y,z,w) NOTHING(x)

#include "messages.h"
#undef MSGMAGIC_CREATE
#define MSGMAGIC_UNDEF
#include "msg_magic.h"

//******************************************************************************
#elif defined(MSGMAGIC_DESTROY)
//******************************************************************************

#define MESSAGES_BEGIN()                                                                       \
    namespace msg                                                                              \
    {                                                                                          \
        void destroy(Message *msg)                                                             \
        {                                                                                      \
            if (msg == 0) return;                                                              \
                                                                                               \
            switch (msg->type)                                                                 \
            {

#define MESSAGES_END()                                                                         \
                default: assert(false);                                                        \
            }                                                                                  \
        }                                                                                      \
    }

#define MESSAGE(T, C)                                                                          \
    case T::Type: _::destroy<T>((T*)msg); return;                                              \
        {                                                                                      \
            struct {

#define END                                                                                    \
            } _unused;                                                                         \
            UNUSED(_unused);                                                                   \
        }

#define BEGIN         NOTHING(x)
#define LIST(x,y)     NOTHING(x)
#define LISTEND       NOTHING(x)
#define Integer(x)    NOTHING(x)
#define Bool(x)       NOTHING(x)
#define String(x,y)   NOTHING(x)
#define Bits(x,y)     NOTHING(x)
#define Fixed(x)      NOTHING(x)
#define List(x,y,z,w) NOTHING(x)

#include "messages.h"
#undef MSGMAGIC_DESTROY
#define MSGMAGIC_UNDEF
#include "msg_magic.h"

//******************************************************************************
#elif defined(MSGMAGIC_SERIALIZE)
//******************************************************************************

#define MESSAGES_BEGIN()                                                                       \
    namespace msg                                                                              \
    {

#define MESSAGES_END()                                                                         \
    }

#define LIST(ListType, item)                                                                   \
    namespace                                                                                  \
    {                                                                                          \
        template<class T>                                                                      \
        static void _w_ ## ListType (hlp::BitWriter &_w, const T *_list, int _count,           \
                                    const size_t _sizeBits)                                    \
        {                                                                                      \
            _w.write(_count, _sizeBits);                                                       \
                                                                                               \
            for (int _i = 0; _i < _count; _i++)                                                \
            {                                                                                  \
                const T & item = _list[_i];

#define LISTEND                                                                                \
            }                                                                                  \
        }                                                                                      \
    }

#define MESSAGE(T, C)                                                                          \
    size_t T::serialize(uint8_t *_data, size_t _length) const                                  \
    {                                                                                          \
        hlp::BitWriter _w(_data, _length);                                                     \
        _w.write((uint8_t)Type, MINBITS(_::type_count - 1));                                   \
                                                                                               \
        {                                                                                      \
            struct                                                                             \
            {

#define BEGIN                                                                                  \
            } _unused;                                                                         \
            UNUSED(_unused);                                                                   \
        }

#define END                                                                                    \
        return _w.bitpos() / 8 + (_w.bitpos() % 8 != 0);                                       \
    }

#define Integer(x)                                                                             \
        _w.write(x);

#define Bool(x)                                                                                \
        { const bool &_unused = x; UNUSED(_unused); }                                          \
        _w.write((bool)x);

#define Bits(x, bits)                                                                          \
        _w.write(x, bits);

#define String(str, min)                                                                       \
        assert(str[sizeof(str) - 1] == 0);                                                     \
        _w.write(str, sizeof(str));

#define Fixed(x)                                                                               \
        _w.write(x.value_);

#define List(ListType, list, count, min)                                                       \
        assert(count >= min);                                                                  \
        _w_ ## ListType (_w, list, count, MINBITS(sizeof(list) / sizeof(list[0])));

#include "messages.h"
#undef MSGMAGIC_SERIALIZE
#define MSGMAGIC_UNDEF
#include "msg_magic.h"

//******************************************************************************
#elif defined(MSGMAGIC_UNSERIALIZE)
//******************************************************************************

#define MESSAGES_BEGIN()                                                                       \
    namespace msg                                                                              \
    {

#define MESSAGES_END()                                                                         \
    }

#define LIST(ListType, item)                                                                   \
    namespace                                                                                  \
    {                                                                                          \
        template<class T>                                                                      \
        static bool _r_ ## ListType (hlp::BitReader &_r, T *_list, int &_count,                \
                                    int _min, const size_t _sizeBits)                          \
        {                                                                                      \
            if (!_r.has(_sizeBits))                                                            \
                return false;                                                                  \
                                                                                               \
            _r.read(&_count, _sizeBits);                                                       \
                                                                                               \
            if (_count < _min)                                                                 \
                return false;                                                                  \
                                                                                               \
            for (int _i = 0; _i < _count; _i++)                                                \
            {                                                                                  \
                T & item = _list[_i];

#define LISTEND                                                                                \
            }                                                                                  \
                                                                                               \
            return true;                                                                       \
        }                                                                                      \
    }

#define MESSAGE(T, C)                                                                          \
    bool T::read(const uint8_t *_data, size_t _length)                                         \
    {                                                                                          \
        hlp::BitReader _r(_data, _length);                                                     \
                                                                                               \
        _r.skip(MINBITS(_::type_count - 1));                                                   \
                                                                                               \
        {                                                                                      \
            struct                                                                             \
            {

#define BEGIN                                                                                  \
            } _unused;                                                                         \
            UNUSED(_unused);                                                                   \
        }

#define END                                                                                    \
        if (_r.bitpos() / 8 + (_r.bitpos() % 8 != 0) < _length)                                \
            return false;                                                                      \
        return true;                                                                           \
    }

#define Integer(x)                                                                             \
        if (!_r.has(sizeof(x) * 8)) return false;                                              \
        _r.read(&x);

#define Bool(x)                                                                                \
        { bool &_unused = x; UNUSED(_unused); }                                                \
        if (!_r.has(1)) return false;                                                          \
        _r.read(&x);

#define Bits(x, bits)                                                                          \
        if (!_r.has(bits)) return false;                                                       \
        _r.read(&x, bits);

#define String(str, min)                                                                       \
        {                                                                                      \
            size_t _strlen = _r.read(str, sizeof(str));                                        \
                                                                                               \
            if (_strlen < min || _strlen == sizeof(str))                                       \
                return false;                                                                  \
                                                                                               \
            if (!hlp::utf8_valid(str))                                                         \
                return false;                                                                  \
        }

#define Fixed(x)                                                                               \
        {                                                                                      \
            if (!_r.has(sizeof(int32_t) * 8))                                                  \
                return false;                                                                  \
                                                                                               \
            uint32_t _i32;                                                                     \
            _r.read(&_i32);                                                                    \
            x = fpm::from_value(_i32);                                                         \
        }

#define List(ListType, list, count, min)                                                       \
        if (! _r_ ## ListType (_r, list, count, min, MINBITS(sizeof(list) / sizeof(list[0])))) \
            return false;

#include "messages.h"
#undef MSGMAGIC_UNSERIALIZE
#define MSGMAGIC_UNDEF
#include "msg_magic.h"

#endif

//******************************************************************************
#else
//******************************************************************************

#undef MSGMAGIC_UNDEF
#undef NOTHING
#undef UNUSED
#undef MESSAGES_BEGIN
#undef MESSAGES_END
#undef MESSAGE
#undef BEGIN
#undef END
#undef LIST
#undef LISTEND
#undef Integer
#undef Bool
#undef String
#undef Bits
#undef Fixed
#undef List

#endif
