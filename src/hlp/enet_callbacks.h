#pragma once

#include <enet/enet.h>

namespace hlp
{
	extern const ENetCallbacks enet_callbacks;

	#ifdef DEBUG
	void enet_callbacks_counters();
	#endif
}
