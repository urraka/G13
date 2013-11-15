#pragma once

namespace g13 {
namespace cbk {

class Callback
{
public:
	virtual ~Callback() {}
	virtual void fire(void *data) = 0;
};

template<typename T> class callback_t : public Callback
{
public:
	typedef void (T::*method_t)(void*);

	callback_t(T *object, method_t method) : object_(object), method_(method) {}

	void fire(void *data)
	{
		(object_->*method_)(data);
	}

private:
	T *object_;
	method_t method_;
};

template<typename T> Callback *callback(T *object, typename callback_t<T>::method_t method)
{
	return new callback_t<T>(object, method);
}

}} // g13::cbk
