#pragma once

namespace g13 {
namespace net {

template<typename T> class Ticked
{
public:
	Ticked() {}

	Ticked(int t, const T &d) : tick(t), data(d) {}

	int tick;
	T   data;
};

template<typename T> Ticked<T> make_ticked(int tick, const T &data)
{
	return Ticked<T>(tick, data);
}

}} // g13::net
