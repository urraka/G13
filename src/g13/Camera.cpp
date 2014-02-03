#include "Camera.h"

#include <glm/gtx/transform.hpp>

namespace g13 {

static const float VelMultiplier = 20.0f;
static const float ZoomMultiplier = 5.0f * 1.8f;
static const float ZoomRate = 1.8f;
static const float MaxZoom = 1.2f;

Camera::Camera()
	:	zoom_(0.0f),
		zoomTarget_(0.0f),
		zoomType_(ZoomNone),
		width_(0.0f),
		height_(0.0f)
{
}

void Camera::clampToBounds()
{
	float W = brBounds_.x - tlBounds_.x;
	float H = brBounds_.y - tlBounds_.y;
	float w = width_;
	float h = height_;

	float minScale = (w / h) < (W / H) ? (h / H) : (w / W);
	float defScale = defaultScale();
	float minZoom = glm::log(minScale / defScale) / MaxZoom;

	zoom_ = glm::max(minZoom, zoom_);
	zoomTarget_ = glm::max(minZoom, zoomTarget_);

	float scale = defScale * glm::exp(MaxZoom * zoom_);

	vec2 min = tlBounds_ + 0.5f * viewport() / scale;
	vec2 max = brBounds_ - 0.5f * viewport() / scale;

	position_ = glm::clamp(position_, min, max);
}

void Camera::frame(const Frame &frame)
{
	float dt = sys::to_seconds(frame.delta);

	zoomTarget_ = glm::clamp(zoomTarget_ + zoomType_ * ZoomRate * dt, -1.0f, 1.0f);

	zoom_     += (zoomTarget_ - zoom_) * ZoomMultiplier * ZoomRate * dt;
	position_ += (target_ - position_) * VelMultiplier * dt;

	clampToBounds();

	float scale = defaultScale() * glm::exp(MaxZoom * zoom_);

	matrix_ = mat2d::translate(width_ / 2.0f, height_ / 2.0f);
	matrix_ *= mat2d::scale(scale, scale);
	matrix_ *= mat2d::translate(-position_.x, -position_.y);

	matrixinv_ = mat2d::translate(position_.x, position_.y);
	matrixinv_ *= mat2d::scale(1.0f / scale, 1.0f / scale);
	matrixinv_ *= mat2d::translate(-width_ / 2.0f, -height_ / 2.0f);
}

void Camera::setPosition(const vec2 &position)
{
	target_ = position;
	position_ = position;

	clampToBounds();
}

void Camera::target(const vec2 &target)
{
	target_ = target;
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
	const float initialWidth = 2000.0f; // in world units

	return width_ * worldUnitsPerPixel / initialWidth;
}

} // g13
