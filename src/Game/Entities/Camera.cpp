#include "Camera.h"

namespace ent
{
	Camera::Camera()
		:	zoom_(0.0f),
			maxZoom_(1.2f),
			zoomRate_(1.5f),
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

		position_.update();
		zoom_.update();

		zoomTarget_ = glm::clamp(zoomTarget_ + zoomType_ * zoomRate_ * dts, -1.0f, 1.0f);
		zoomVelocity_ = zoomTarget_ - zoom_.current;
		zoom_.current += zoomVelocity_ * kVelMultiplier * zoomRate_ * dts;
		zoomType_ = ZoomNone;

		if (target_ == 0)
			return;

		vec2 distance = *target_ - position_.current;

		velocity_ = distance * kVelMultiplier;
		position_.current += velocity_ * dts;
	}

	void Camera::target(const vec2 *target)
	{
		target_ = target;

		if (target)
			position_.set(*target);
	}

	void Camera::viewport(int width, int height)
	{
		viewport_ = vec2(width, height);
	}

	mat4 Camera::matrix(float framePercent, MatrixMode mode)
	{
		const float worldUnitsPerPixel = 1.0f;
		const float initialWidth = 1200.0f; // in world units
		const float initialScale = viewport_.x * worldUnitsPerPixel / initialWidth;

		vec2 position = position_.value(framePercent);
		float scale = initialScale * glm::exp(maxZoom_ * zoom_.value(framePercent));

		if (mode == MatrixInverted)
		{
			return glm::translate(position.x, position.y, 0.0f) *
				glm::scale(1.0f / scale, 1.0f / scale, 1.0f) *
				glm::translate(-viewport_.x / 2.0f, -viewport_.y / 2.0f, 0.0f);
		}

		return glm::translate(viewport_.x / 2.0f, viewport_.y / 2.0f, 0.0f) *
			glm::scale(scale, scale, 1.0f) *
			glm::translate(-position.x, -position.y, 0.0f);
	}

	void Camera::zoom(ZoomType zoomType)
	{
		zoomType_ = zoomType;
	}
}
