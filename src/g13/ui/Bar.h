#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <gfx/forward.h>
#include <gfx/Color.h>

namespace g13 {
namespace ui {

class Bar
{
public:
	Bar();
	~Bar();

	void setSize(float width, float height);
	void setPosition(float x, float y);
	void setPercent(float percent);
	void setOutline(float width, const gfx::Color &color);
	void setFill(const gfx::Color &color);
	void setOpacity(float opacity);

	float percent() const { return percent_; }
	float opacity() const { return opacity_; }
	float outlineWidth() const { return outlineWidth_; }
	const vec2 &size() const { return size_; }
	const vec2 &position() const { return position_; }
	const gfx::Color &fillColor() const { return fillColor_; }
	const gfx::Color &outlineColor() const { return outlineColor_; }

	void draw();

private:
	bool changed_;
	float percent_;
	vec2 position_;
	vec2 size_;

	float opacity_;
	float outlineWidth_;

	gfx::Color outlineColor_;
	gfx::Color fillColor_;

	gfx::VBO *vbo_;
};

}} // g13::ui
