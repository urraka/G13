#include <Game/Game.h>
#include <Game/Components/CameraState.h>

namespace component
{

CameraState::CameraState() : zoom(0.0f) {}

CameraState CameraState::operator - ()
{
	CameraState result;
	result -= *this;
	return result;
}

CameraState CameraState::operator + (const CameraState &operand)
{
	CameraState result = *this;
	return result += operand;
}

CameraState &CameraState::operator += (const CameraState &operand)
{
	State *self = this;
	(*self) += *(State*)&operand;
	zoom += operand.zoom;
	return *this;
}

CameraState CameraState::operator - (const CameraState &operand)
{
	CameraState result = *this;
	return result -= operand;
}

CameraState &CameraState::operator -= (const CameraState &operand)
{
	State *self = this;
	(*self) -= *(State*)&operand;
	zoom -= operand.zoom;
	return *this;
}

CameraState CameraState::operator * (float operand)
{
	CameraState result = *this;
	return result *= operand;
}

CameraState &CameraState::operator *= (float operand)
{
	State *self = this;
	(*self) *= operand;
	zoom *= operand;
	return *this;
}

CameraState CameraState::operator / (float operand)
{
	CameraState result = *this;
	result *= 1.0f / operand;
	return result;
}

CameraState &CameraState::operator /= (float operand)
{
	*this *= 1.0f / operand;
	return *this;
}

}
