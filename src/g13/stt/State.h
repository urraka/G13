#pragma once

#include <g13/g13.h>

namespace g13 {
namespace stt {

class State
{
public:
	virtual ~State() {}

	virtual void update(Time dt) = 0;
	virtual void draw(const Frame &frame) = 0;
	virtual bool onEvent(const sys::Event &event) = 0;
};

}} // g13::stt
