#include <Game/Game.h>
#include <Game/Components/Camera.h>

namespace component
{

Camera::Camera() : target_(0) {}

void Camera::update(Time dt, CameraState &state)
{
	if (target_ != 0)
		state.position = *target_;
}

void Camera::target(const vec2 *target)
{
	target_ = target;
}

void Camera::viewport(int width, int height)
{
	viewport_ = ivec2(width, height);
}

mat4 Camera::matrix(const CameraState &state)
{
	return mat4(1.0f);
}

}
