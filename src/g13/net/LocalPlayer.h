#pragma once

#include <g13/g13.h>
#include <g13/cmp/SoldierInput.h>
#include "Player.h"

namespace g13 {
namespace net {

class LocalPlayer : public Player
{
public:
	void initialize();
	void update(Time dt, coll::World &world);
	void updateInput();

	int tick;
	cmp::SoldierInput input;
};

}} // g13::net
