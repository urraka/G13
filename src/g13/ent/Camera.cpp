#include "Camera.h"

#include <glm/gtx/transform.hpp>

namespace g13 {
namespace ent {

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

	float dts = sys::to_seconds(dt);

	position_.update();
	zoom_.update();

	zoomTarget_ = glm::clamp(zoomTarget_ + zoomType_ * zoomRate_ * dts, -1.0f, 1.0f);
	zoomVelocity_ = zoomTarget_ - zoom_.current;
	zoom_.current += zoomVelocity_ * kVelMultiplier * zoomRate_ * dts;

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
	width_ = width;
	height_ = height;
}

mat4 Camera::matrix(float framePercent, MatrixMode mode)
{
	const float worldUnitsPerPixel = 1.0f;
	const float initialWidth = 1200.0f; // in world units
	const float initialScale = width_ * worldUnitsPerPixel / initialWidth;

	vec2 position = position_.value(framePercent);
	float scale = initialScale * glm::exp(maxZoom_ * zoom_.value(framePercent));

	if (mode == MatrixInverted)
	{
		return glm::translate(position.x, position.y, 0.0f) *
			glm::scale(1.0f / scale, 1.0f / scale, 1.0f) *
			glm::translate(-width_ / 2.0f, -height_ / 2.0f, 0.0f);
	}

	return glm::translate(width_ / 2.0f, height_ / 2.0f, 0.0f) *
		glm::scale(scale, scale, 1.0f) *
		glm::translate(-position.x, -position.y, 0.0f);
}

void Camera::zoom(ZoomType zoomType)
{
	zoomType_ = zoomType;
}

}} // g13::ent
