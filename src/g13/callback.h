#pragma once

#include <assert.h>

#define make_callback(obj, T, M) g13::Callback(obj, g13::Callback::func<T, &T::M>)

namespace g13 {

class Callback
{
	typedef void (*callback_t)(void*, void*);

public:
	Callback() : object_(0), callback_(0) {}

	Callback(void *object, callback_t callback)
		:	object_(object),
			callback_(callback)
	{}

	void fire(void *data)
	{
		assert(object_ != 0 && callback_ != 0);

		callback_(object_, data);
	}

	template<typename T, void (T::*M)(void*)>
	static void func(void *object, void *data)
	{
		(((T*)object)->*M)(data);
	}

private:
	void *object_;
	callback_t callback_;
};

} // g13
