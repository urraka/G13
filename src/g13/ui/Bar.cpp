#include "Bar.h"
#include <gfx/gfx.h>

namespace g13 {
namespace ui {

static const int index_count  = 5 * 6;
static const int vertex_count = 5 * 4;

Bar::Bar()
	:	changed_(true),
		percent_(1.0f),
		size_(100.0f, 10.0f),
		opacity_(1.0f),
		outlineWidth_(2.0f),
		outlineColor_(0),
		fillColor_(255, 0, 0),
		vbo_(0)
{
	vbo_ = new gfx::VBO(new gfx::IBO(index_count, gfx::Static));
	vbo_->allocate<gfx::ColorVertex>(vertex_count, gfx::Static);

	uint16_t indices[index_count];

	for (int i = 0; i < 5; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 3;
		indices[i * 6 + 5] = i * 4 + 0;
	}

	vbo_->ibo()->set(indices, 0, index_count);
}

Bar::~Bar()
{
	delete vbo_->ibo();
	delete vbo_;
}

void Bar::setSize(float width, float height)
{
	changed_ = true;
	size_.x = width;
	size_.y = height;
}

void Bar::setPosition(float x, float y)
{
	changed_ = true;
	position_.x = x;
	position_.y = y;
}

void Bar::setPercent(float percent)
{
	changed_ = true;
	percent_ = percent;
}

void Bar::setOutline(float width, const gfx::Color &color)
{
	changed_ = true;
	outlineWidth_ = width;
	outlineColor_ = color;
}

void Bar::setFill(const gfx::Color &color)
{
	changed_ = true;
	fillColor_ = color;
}

void Bar::setOpacity(float opacity)
{
	changed_ = true;
	opacity_ = opacity;
}

void Bar::draw()
{
	if (changed_)
	{
		changed_ = false;

		// o = outer
		// i = inner
		// l,r,t,b = left, right, top, bottom

		const float lo = position_.x;
		const float li = position_.x + outlineWidth_;
		const float ri = position_.x + outlineWidth_ + size_.x;
		const float ro = position_.x + outlineWidth_ + size_.x + outlineWidth_;

		const float to = position_.y;
		const float ti = position_.y + outlineWidth_;
		const float bi = position_.y + outlineWidth_ + size_.y;
		const float bo = position_.y + outlineWidth_ + size_.y + outlineWidth_;

		const float hx = glm::mix(li, ri, percent_);

		gfx::Color fill = fillColor_;
		gfx::Color outline = outlineColor_;

		fill.a *= opacity_;
		outline.a *= opacity_;

		gfx::ColorVertex vertices[vertex_count] = {
			// top-border
			gfx::color_vertex(lo, to, outline),
			gfx::color_vertex(ro, to, outline),
			gfx::color_vertex(ro, ti, outline),
			gfx::color_vertex(lo, ti, outline),

			// bottom-border
			gfx::color_vertex(lo, bi, outline),
			gfx::color_vertex(ro, bi, outline),
			gfx::color_vertex(ro, bo, outline),
			gfx::color_vertex(lo, bo, outline),

			// left-border
			gfx::color_vertex(lo, ti, outline),
			gfx::color_vertex(li, ti, outline),
			gfx::color_vertex(li, bi, outline),
			gfx::color_vertex(lo, bi, outline),

			// right-border
			gfx::color_vertex(ri, ti, outline),
			gfx::color_vertex(ro, ti, outline),
			gfx::color_vertex(ro, bi, outline),
			gfx::color_vertex(ri, bi, outline),

			// fill
			gfx::color_vertex(li, ti, fill),
			gfx::color_vertex(hx, ti, fill),
			gfx::color_vertex(hx, bi, fill),
			gfx::color_vertex(li, bi, fill)
		};

		vbo_->set(vertices, 0, vertex_count);
	}

	gfx::draw(vbo_);
}

}} // g13::ui
