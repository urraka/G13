#include <Game/Game.h>
#include <Game/Entities/Camera.h>

Camera::Camera()
	:	zoom_(),
		maxZoom_(2.0f),
		zoomRate_(0.5f),
		zoomVelocity_(0.0f),
		zoomTarget_(0.0f),
		zoomType_(ZoomNone),
		target_(0)
{
}

void Camera::update(Time dt)
{
	const float kVelMultiplier = 2.5f;

	float dts = Clock::toSeconds<float>(dt);

	position_[PreviousFrame] = position_[CurrentFrame];
	zoom_[PreviousFrame] = zoom_[CurrentFrame];

	vec2 &position = position_[CurrentFrame];
	float &zoom = zoom_[CurrentFrame];

	zoomTarget_ = glm::clamp(zoomTarget_ + zoomType_ * zoomRate_ * dts, -1.0f, 1.0f);
	zoomVelocity_ = zoomTarget_ - zoom;
	zoom += zoomVelocity_ * kVelMultiplier * dts;
	zoomType_ = ZoomNone;

	if (target_ == 0)
		return;

	vec2 distance = target_->position() - position;

	velocity_ = distance * kVelMultiplier;
	position += velocity_ * dts;
}

void Camera::target(const Entity *target)
{
	target_ = target;
}

void Camera::viewport(int width, int height)
{
	viewport_ = vec2(width, height);
}

mat4 Camera::matrix(float framePercent)
{
	vec2 position = glm::mix(position_[CurrentFrame], position_[PreviousFrame], framePercent);
	float scale = glm::exp(maxZoom_ * glm::mix(zoom_[CurrentFrame], zoom_[PreviousFrame], framePercent));

	return
		glm::translate(viewport_.x / 2.0f, viewport_.y / 2.0f, 0.0f) *
		glm::scale(scale, scale, 1.0f) *
		glm::translate(-position.x, -position.y, 0.0f);
}

void Camera::zoom(ZoomType zoomType)
{
	zoomType_ = zoomType;
}
