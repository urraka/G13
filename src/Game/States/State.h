#pragma once

#include "../../System/Clock.h"
#include "../../System/Event.h"

namespace stt {

class State
{
public:
	virtual ~State() {}

	virtual void update(Time dt) = 0;
	virtual void draw(float percent) = 0;
	virtual void event(const Event &evt) = 0;
};

} // stt
