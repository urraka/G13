#include <Game/Game.h>

const vec2 &Entity::position() const
{
	return position_[CurrentFrame];
}
