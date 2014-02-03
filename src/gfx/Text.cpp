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
		color_(255),
		outlineWidth_(0.0f),
		outlineColor_(0),
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

		i++;
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

void Text::outlineColor(const Color &color)
{
	outlineColor_ = color;
}

void Text::outlineWidth(float width)
{
	if (outlineWidth_ != width)
	{
		outlineWidth_ = width;
		updatedGeometry_ = false;
		updatedBounds_ = false;
	}
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

typedef std::vector<const Font::Glyph*> glyphs_t;
typedef std::vector<Text::DrawData> buffers_t;
typedef std::vector<VBO*> vbos_t;

static void get_glyphs(const Text::string32_t &text, Font *font, glyphs_t &glyphs)
{
	glyphs.resize(text.size());

	for (size_t i = 0; i < text.size(); i++)
	{
		glyphs[i] = 0;

		if (text[i] == ' ' || text[i] == '\n' || text[i] == '\t' || text[i] == '\v')
			continue;

		glyphs[i] = font->glyph(text[i]);
	}
}

static void prepare(Font *font, const glyphs_t &glyphs, buffers_t &data, vbos_t &vbos, int &ivbo, int &max)
{
	data.resize(font->textureCount());

	for (size_t i = 0; i < glyphs.size(); i++)
	{
		if (glyphs[i] != 0)
			data[glyphs[i]->atlas].count++;
	}

	for (size_t i = 0; i < data.size(); i++)
	{
		if (data[i].count > 0)
		{
			if (data[i].count > max)
				max = data[i].count;

			if (ivbo >= (int)vbos.size())
				vbos.push_back(new VBO());

			VBO *vbo = vbos[ivbo];

			if (vbo->size() < (size_t)data[i].count * 4)
				vbo->allocate<TextVertex>(data[i].count * 4, Dynamic);

			data[i].vbo = vbo;
			data[i].atlas = i;

			ivbo++;
		}
	}
}

void Text::update(bool upload)
{
	if (upload)
		updatedGeometry_ = true;

	updatedBounds_ = true;

	drawData_.clear();
	glyphs_.clear();
	outlineDrawData_.clear();
	outlineGlyphs_.clear();

	bounds_ = Bounds();

	if (value_.size() == 0 || font_ == 0)
		return;

	font_->size(size_);
	font_->outlineWidth(0.0f);
	get_glyphs(value_, font_, glyphs_);

	if (outlineWidth_ != 0.0f)
	{
		font_->outlineWidth(outlineWidth_);
		get_glyphs(value_, font_, outlineGlyphs_);
	}

	if (upload)
	{
		int base = vbos_.size();
		int ivbo = 0;
		int maxCount = 0;

		prepare(font_, glyphs_, drawData_, vbos_, ivbo, maxCount);

		if (outlineWidth_ != 0.0f)
			prepare(font_, outlineGlyphs_, outlineDrawData_, vbos_, ivbo, maxCount);

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

		for (int i = base; i < ivbo; i++)
			vbos_[i]->ibo(ibo_);
	}

	font_->outlineWidth(0.0f);

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

		if (outlineWidth_ != 0.0f)
		{
			const Font::Glyph &g = *outlineGlyphs_[i];

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

				outlineDrawData_[g.atlas].vbo->set(v, outlineDrawData_[g.atlas].offset, 4);
				outlineDrawData_[g.atlas].offset += 4;
			}
		}

		x += g.advance;
	}

	for (size_t i = 0; i < drawData_.size(); i++)
	{
		if (drawData_[i].count == 0)
			drawData_.erase(drawData_.begin() + i--);
	}

	for (size_t i = 0; i < outlineDrawData_.size(); i++)
	{
		if (outlineDrawData_[i].count == 0)
			outlineDrawData_.erase(outlineDrawData_.begin() + i--);
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

	if (outlineDrawData_.size() > 0)
	{
		glm::vec4 color(
			outlineColor_.r / 255.f,
			outlineColor_.g / 255.f,
			outlineColor_.b / 255.f,
			outlineColor_.a / 255.f
		);

		context->shdrtext->uniform(context->shdrtext_color, color);

		for (size_t i = 0; i < outlineDrawData_.size(); i++)
		{
			Texture *tx = font_->texture(outlineDrawData_[i].atlas);
			glm::vec2 texsize((float)tx->width(), (float)tx->height());

			context->shdrtext->uniform(context->shdrtext_texsize, texsize);

			gfx::bind(tx);
			gfx::draw(outlineDrawData_[i].vbo, 0, outlineDrawData_[i].count * 6);
		}
	}

	if (drawData_.size() > 0)
	{
		glm::vec4 color(
			color_.r / 255.f,
			color_.g / 255.f,
			color_.b / 255.f,
			color_.a / 255.f
		);

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
