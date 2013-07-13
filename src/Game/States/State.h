#pragma once

#include <sys/sys.h>

namespace stt {

class State
{
public:
	virtual ~State() {}

	virtual void update(sys::Time dt) = 0;
	virtual void draw(float percent) = 0;
	virtual void event(sys::Event *evt) = 0;
};

} // stt
