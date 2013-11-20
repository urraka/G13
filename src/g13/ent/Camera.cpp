#include "Camera.h"

#include <glm/gtx/transform.hpp>

namespace g13 {
namespace ent {

Camera::Camera()
	:	zoom_(0.0f),
		maxZoom_(1.2f),
		zoomRate_(1.8f),
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

	position_.previous = position_.current;
	zoom_.previous = zoom_.current;

	zoomTarget_ = glm::clamp(zoomTarget_ + zoomType_ * zoomRate_ * dts, -1.0f, 1.0f);
	zoomVelocity_ = zoomTarget_ - zoom_.current;
	zoom_.current += zoomVelocity_ * kVelMultiplier * zoomRate_ * dts;

	if (target_ == 0)
		return;

	vec2 distance = *target_ - position_.current;

	velocity_ = distance * kVelMultiplier;
	position_.current += velocity_ * dts;

	clampToBounds();
}

void Camera::clampToBounds()
{
	float W = brBounds_.x - tlBounds_.x;
	float H = brBounds_.y - tlBounds_.y;
	float w = width_;
	float h = height_;

	float minScale = (w / h) < (W / H) ? (h / H) : (w / W);
	float defScale = defaultScale();
	float minZoom = glm::log(minScale / defScale) / maxZoom_;

	zoom_.current = glm::max(minZoom, zoom_.current);
	zoomTarget_ = glm::max(minZoom, zoomTarget_);

	float scale = defScale * glm::exp(maxZoom_ * zoom_.current);

	vec2 min = tlBounds_ + 0.5f * viewport() / scale;
	vec2 max = brBounds_ - 0.5f * viewport() / scale;

	position_.current = glm::clamp(position_.current, min, max);
}

void Camera::frame(const Frame &frame)
{
	zoom_.interpolate(frame.percent);
	position_.interpolate(frame.percent);

	float scale = defaultScale() * glm::exp(maxZoom_ * (float)zoom_);
	const vec2 &pos = position_;

	matrix_ = mat2d::translate(width_ / 2.0f, height_ / 2.0f);
	matrix_ *= mat2d::scale(scale, scale);
	matrix_ *= mat2d::translate(-pos.x, -pos.y);

	matrixinv_ = mat2d::translate(pos.x, pos.y);
	matrixinv_ *= mat2d::scale(1.0f / scale, 1.0f / scale);
	matrixinv_ *= mat2d::translate(-width_ / 2.0f, -height_ / 2.0f);
}

void Camera::target(const vec2 *target)
{
	target_ = target;

	if (target)
	{
		position_.set(*target);

		clampToBounds();

		position_.set(position_.current);
		zoom_.set(zoom_.current);
	}
}

void Camera::viewport(int width, int height)
{
	width_ = width;
	height_ = height;
}

void Camera::zoom(ZoomType zoomType)
{
	zoomType_ = zoomType;
}

void Camera::bounds(const vec2 &tl, const vec2 &br)
{
	tlBounds_ = tl;
	brBounds_ = br;
}

float Camera::defaultScale() const
{
	const float worldUnitsPerPixel = 1.0f;
	const float initialWidth = 1500.0f; // in world units

	return width_ * worldUnitsPerPixel / initialWidth;
}

}} // g13::ent
