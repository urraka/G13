#pragma once

#include <enet/enet.h>

namespace hlp
{
	extern const ENetCallbacks *enet_callbacks;

	void enet_callbacks_counters();
}
