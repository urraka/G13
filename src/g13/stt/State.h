#pragma once

#include <g13/g13.h>

namespace g13 {
namespace stt {

class State
{
public:
	virtual ~State() {}

	virtual void update(Time dt) = 0;
	virtual void draw(float percent) = 0;
	virtual bool event(Event *evt) = 0;
};

}} // g13::stt
