#pragma once

namespace g13 {
namespace net
{

template<typename T> class Ticked
{
public:
	Ticked() {}

	Ticked(int t, const T &d) : tick(t), data(d) {}

	int tick;
	T   data;
};

}} // g13::net
