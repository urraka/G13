#include "Text.h"
#include "IBO.h"
#include "VBO.h"
#include "Texture.h"
#include "vertex.h"
#include "gfx.h"
#include "Context.h"

namespace gfx {

Text::Text()
	:	font_(0),
		size_(16),
		color_(255, 255, 255),
		updatedGeometry_(true),
		updatedBounds_(true),
		ibo_(new IBO())
{
}

Text::~Text()
{
	for (size_t i = 0; i < vbos_.size(); i++)
		delete vbos_[i];

	delete ibo_;
}

void Text::value(const string32_t &str)
{
	if (value_ != str)
	{
		value_ = str;
		updatedGeometry_ = false;
		updatedBounds_ = false;
	}
}

void Text::value(const char *str)
{
	bool equal = true;
	size_t i = 0;

	while (str[i] != 0)
	{
		if (i >= value_.size() || value_[i] != (uint32_t)str[i])
		{
			equal = false;
			break;
		}
	}

	if (value_.size() != i)
		equal = false;

	if (!equal)
	{
		value_.clear();

		while (*str != 0)
			value_ += (uint32_t)*(str++);

		updatedGeometry_ = false;
		updatedBounds_ = false;
	}
}

void Text::font(Font *font)
{
	if (font != font_)
	{
		font_ = font;
		updatedGeometry_ = false;
		updatedBounds_ = false;
	}
}

void Text::size(uint32_t size)
{
	if (size != size_)
	{
		size_ = size;
		updatedGeometry_ = false;
		updatedBounds_ = false;
	}
}

void Text::color(const Color &color)
{
	color_ = color;
}

const Text::string32_t &Text::value()
{
	return value_;
}

Font *Text::font() const
{
	return font_;
}

Color Text::color() const
{
	return color_;
}

uint32_t Text::size() const
{
	return size_;
}

const Text::Bounds &Text::bounds()
{
	if (!updatedBounds_)
		update(false);

	return bounds_;
}

void Text::update(bool upload)
{
	if (upload)
		updatedGeometry_ = true;

	updatedBounds_ = true;

	drawData_.clear();
	glyphs_.clear();

	bounds_ = Bounds();

	if (value_.size() == 0 || font_ == 0)
		return;

	font_->size(size_);
	glyphs_.resize(value_.size());

	for (size_t i = 0; i < value_.size(); i++)
	{
		glyphs_[i] = 0;

		if (value_[i] == ' ' || value_[i] == '\n' || value_[i] == '\t' || value_[i] == '\v')
			continue;

		glyphs_[i] = font_->glyph(value_[i]);
	}

	if (upload)
	{
		drawData_.resize(font_->atlases_.size());

		for (size_t i = 0; i < glyphs_.size(); i++)
		{
			if (glyphs_[i] != 0)
				drawData_[glyphs_[i]->atlas].count++;
		}

		int ivbo = 0;
		int maxCount = 0;

		for (size_t i = 0; i < drawData_.size(); i++)
		{
			if (drawData_[i].count > 0)
			{
				if (drawData_[i].count > maxCount)
					maxCount = drawData_[i].count;

				if (ivbo >= (int)vbos_.size())
					vbos_.push_back(new VBO(ibo_));

				VBO *vbo = vbos_[ivbo];

				if (vbo->size() < (size_t)drawData_[i].count * 4)
					vbo->allocate<TextVertex>(drawData_[i].count * 4, Dynamic);

				drawData_[i].vbo = vbo;
				drawData_[i].atlas = i;

				ivbo++;
			}
		}

		if (ibo_->size() < (size_t)maxCount * 6)
		{
			ibo_->allocate(maxCount * 6, Static);

			uint16_t indices[] = {0, 1, 2, 2, 3, 0};

			for (int i = 0; i < maxCount; i++)
			{
				ibo_->set(indices, 6 * i, 6);

				for (int j = 0; j < 6; j++)
					indices[j] += 4;
			}
		}
	}

	float x = 0.0f;
	float y = 0.0f;
	float hspace = (float)font_->glyph(' ')->advance;
	float vspace = (float)font_->linespacing();

	uint32_t prev = 0;

	float xmin = 0.0f;
	float ymin = 0.0f;
	float xmax = 0.0f;
	float ymax = 0.0f;

	for (size_t i = 0; i < glyphs_.size(); i++)
	{
		uint32_t current = value_[i];

		x += font_->kerning(prev, current);
		prev = current;

		switch (current)
		{
			case ' ' : x += hspace;        continue;
			case '\t': x += hspace * 4;    continue;
			case '\v': y += vspace * 4;    continue;
			case '\n': y += vspace, x = 0; continue;
		}

		const Font::Glyph &g = *glyphs_[i];

		xmin = std::min(xmin, x + g.x);
		ymin = std::min(ymin, y + g.y);
		xmax = std::max(xmax, x + g.x + g.w);
		ymax = std::max(ymax, y + g.y + g.h);

		if (upload)
		{
			TextVertex v[4] = {
				{x + g.x      , y + g.y      , (uint16_t)g.u0, (uint16_t)g.v0},
				{x + g.x + g.w, y + g.y      , (uint16_t)g.u1, (uint16_t)g.v0},
				{x + g.x + g.w, y + g.y + g.h, (uint16_t)g.u1, (uint16_t)g.v1},
				{x + g.x      , y + g.y + g.h, (uint16_t)g.u0, (uint16_t)g.v1}
			};

			drawData_[g.atlas].vbo->set(v, drawData_[g.atlas].offset, 4);
			drawData_[g.atlas].offset += 4;
		}

		x += g.advance;
	}

	for (size_t i = 0; i < drawData_.size(); i++)
	{
		if (drawData_[i].count == 0)
			drawData_.erase(drawData_.begin() + i--);
	}

	bounds_.x = xmin;
	bounds_.y = ymin;
	bounds_.width = xmax - xmin;
	bounds_.height = ymax - ymin;
}

void Text::draw()
{
	if (!updatedGeometry_)
		update(true);

	if (drawData_.size() > 0)
	{
		glm::vec4 color(color_.r / 255.f, color_.g / 255.f, color_.b / 255.f, color_.a / 255.f);

		context->shdrtext->uniform(context->shdrtext_color, color);

		for (size_t i = 0; i < drawData_.size(); i++)
		{
			Texture *tx = font_->texture(drawData_[i].atlas);
			glm::vec2 texsize((float)tx->width(), (float)tx->height());

			context->shdrtext->uniform(context->shdrtext_texsize, texsize);

			gfx::bind(tx);
			gfx::draw(drawData_[i].vbo, 0, drawData_[i].count * 6);
		}
	}
}

} // gfx
