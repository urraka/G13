#include <Game/Game.h>
#include <Game/Entities/Camera.h>

void Camera::update(Time dt)
{
	prevState_ = state_;
	camera_.update(dt, state_);
}

void Camera::target(const vec2 *target)
{
	camera_.target(target);
}

void Camera::viewport(int width, int height)
{
	camera_.viewport(width, height);
}

mat4 Camera::matrix(float percent)
{
	return camera_.matrix(prevState_ + (state_ - prevState_) * percent);
}
