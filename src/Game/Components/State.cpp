#include <Game/Game.h>
#include <Game/Components/State.h>

namespace component
{

State State::operator - ()
{
	State result;
	result -= *this;
	return result;
}

State State::operator + (const State &operand)
{
	State result = *this;
	return result += operand;
}

State &State::operator += (const State &operand)
{
	position += operand.position;
	velocity += operand.velocity;
	acceleration += operand.acceleration;
	return *this;
}

State State::operator - (const State &operand)
{
	State result = *this;
	return result -= operand;
}

State &State::operator -= (const State &operand)
{
	position -= operand.position;
	velocity -= operand.velocity;
	acceleration -= operand.acceleration;
	return *this;
}

State State::operator * (float operand)
{
	State result = *this;
	return result *= operand;
}

State &State::operator *= (float operand)
{
	position *= operand;
	velocity *= operand;
	acceleration *= operand;
	return *this;
}

State State::operator / (float operand)
{
	State result = *this;
	result *= 1.0f / operand;
	return result;
}

State &State::operator /= (float operand)
{
	*this *= 1.0f / operand;
	return *this;
}

}
