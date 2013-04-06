#include <Game/Game.h>
#include <Game/Entities/Camera.h>

Camera::Camera()
	:	zoom_(),
		maxZoom_(1.0f),
		zoomRate_(2.0f),
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
	vec2 &position = position_[CurrentFrame];
	float &zoom = zoom_[CurrentFrame];

	position_[PreviousFrame] = position_[CurrentFrame];
	zoom_[PreviousFrame] = zoom_[CurrentFrame];

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

	if (target)
	{
		position_[PreviousFrame] = target->position();
		position_[CurrentFrame] = target->position();
	}
}

void Camera::viewport(int width, int height)
{
	viewport_ = vec2(width, height);
}

mat4 Camera::matrix(float framePercent)
{
	// const float initialScale = viewport_.x / 1000.0f;
	const float initialScale = 1.0f;

	vec2 position = glm::mix(position_[PreviousFrame], position_[CurrentFrame], framePercent);
	float scale = initialScale * glm::exp(maxZoom_ * glm::mix(zoom_[PreviousFrame], zoom_[CurrentFrame], framePercent));

	return
		glm::translate(viewport_.x / 2.0f, viewport_.y / 2.0f, 0.0f) *
		glm::scale(scale, scale, 1.0f) *
		glm::translate(-position.x, -position.y, 0.0f);
}

void Camera::zoom(ZoomType zoomType)
{
	zoomType_ = zoomType;
}
