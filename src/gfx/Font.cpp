#include "Font.h"
#include "Texture.h"
#include "Context.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H

#include <iostream>

namespace gfx {

// -----------------------------------------------------------------------------
// Font
// -----------------------------------------------------------------------------

Font::Font(const char *filename)
	:	face_(0),
		currentTable_(0),
		currentSize_(0)
{
	FT_Face face;

	if (FT_New_Face(context->freetype, filename, 0, &face) != 0)
	{
		std::cerr << "Failed to load font: " << filename << std::endl;
		return;
	}

	if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0)
	{
		std::cerr << "Font doesn't support unicode: " << filename << std::endl;
		return;
	}

	face_ = face;
}

Font::~Font()
{
	if (face_ != 0)
		FT_Done_Face((FT_Face)face_);

	for (PageTable::iterator i = glyphs_.begin(); i != glyphs_.end(); ++i)
		delete i->second;

	for (size_t i = 0; i < atlases_.size(); i++)
		delete atlases_[i];
}

void Font::size(uint32_t size)
{
	assert(size > 0);

	GlyphTable *&table = glyphs_[size]; // will insert the first time

	if (table == 0)
		table = new GlyphTable();

	currentSize_ = size;
	currentTable_ = table;
}

Texture *Font::texture(int atlas)
{
	assert(atlas >= 0);

	if (atlas < (int)atlases_.size())
		return atlases_[atlas]->texture();

	return 0;
}

const Font::Glyph *Font::glyph(uint32_t codepoint, bool bold)
{
	assert(currentTable_ != 0);

	uint32_t key = codepoint | (bold ? (1U << 31) : 0);

	GlyphTable::iterator i = currentTable_->find(key);

	if (i == currentTable_->end())
		i = currentTable_->insert(std::make_pair(key, load(codepoint, bold))).first;

	return &i->second;
}

Font::Glyph Font::load(uint32_t codepoint, bool bold)
{
	struct GlyphAutoRelease
	{
		FT_Glyph *glyph;
		GlyphAutoRelease(FT_Glyph *g) : glyph(g) {}
		~GlyphAutoRelease() { FT_Done_Glyph(*glyph); }
	};

	Glyph glyph;

	FT_Face face = (FT_Face)face_;

	if (face == 0)
		return Glyph();

	if (currentSize_ != face->size->metrics.x_ppem)
	{
		if (FT_Set_Pixel_Sizes(face, 0, currentSize_) != 0)
			return Glyph();
	}

	if (FT_Load_Char(face, codepoint, FT_LOAD_TARGET_NORMAL) != 0)
		return Glyph();

	FT_Glyph desc;

	if (FT_Get_Glyph(face->glyph, &desc) != 0)
		return Glyph();

	GlyphAutoRelease glyphAutoRelease(&desc);

	FT_Pos weight = 1 << 6;
	bool outline = (desc->format == FT_GLYPH_FORMAT_OUTLINE);

	if (bold && outline)
	{
		FT_OutlineGlyph outlineGlyph = (FT_OutlineGlyph)desc;
		FT_Outline_Embolden(&outlineGlyph->outline, weight);
	}

	FT_Glyph_To_Bitmap(&desc, FT_RENDER_MODE_NORMAL, 0, 1);
	FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)desc;
	FT_Bitmap& bitmap = bitmapGlyph->bitmap;

	if (bold && !outline)
		FT_Bitmap_Embolden(context->freetype, &bitmap, weight, weight);

	if (bitmap.width <= 0 || bitmap.rows <= 0)
		return Glyph();

	const int padding = 1;
	const int width  = bitmap.width;
	const int height = bitmap.rows;
	const int wBounds = width  + padding; // bottom-right padding
	const int hBounds = height + padding;

	Atlas *atlas = 0;
	Atlas::Region region;

	if (atlases_.size() > 0)
	{
		atlas = atlases_.back();
		region = atlas->region(wBounds, hBounds);
	}

	if (region.width == 0)
	{
		atlas = new Atlas();

		if (atlas->texture() == 0)
		{
			delete atlas;
			return Glyph();
		}

		atlases_.push_back(atlas);
		region = atlas->region(wBounds, hBounds);

		if (region.width == 0)
			return Glyph();
	}

	glyph.atlas = (int)atlases_.size() - 1;

	glyph.x =  bitmapGlyph->left - padding;
	glyph.y = -bitmapGlyph->top  - padding;
	glyph.w =  wBounds;
	glyph.h =  hBounds;

	// extend region to contain the top-left padding
	glyph.u0 = region.x - padding;
	glyph.v0 = region.y - padding;
	glyph.u1 = glyph.u0 + region.width  + padding;
	glyph.v1 = glyph.v0 + region.height + padding;

	glyph.advance = (desc->advance.x >> 16) + bold ? (weight >> 6) : 0;

	uint8_t *data = new uint8_t[width * height];

	const uint8_t *src = bitmap.buffer;
	uint8_t *dst = data;

	if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
				dst[x] = ((src[x / 8]) & (1 << (7 - (x % 8)))) ? 255 : 0;

			src += bitmap.pitch;
			dst += width;
		}
	}
	else
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
				dst[x] = src[x];

			src += bitmap.pitch;
			dst += width;
		}
	}

	region.width -= padding;
	region.height -= padding;

	atlas->set(region, data);

	delete[] data;

	return glyph;
}

// -----------------------------------------------------------------------------
// Font::Atlas
// -----------------------------------------------------------------------------

Font::Atlas::Atlas() : texture_(0)
{
	int width  = 128;
	int height = 128;

	texture_ = new Texture(width, height, 1, false);

	if (texture_->id() == 0)
	{
		delete texture_;
		texture_ = 0;
		return;
	}

	uint8_t *data = new uint8_t[width * height];
	memset(data, 0, width * height);

	texture_->update(0, 0, width, height, data);

	delete[] data;

	nodes_.push_back(Node(1, 1, width - 1)); // 1px top-left padding
}

Font::Atlas::~Atlas()
{
	if (texture_ != 0)
		delete texture_;
}

void Font::Atlas::set(const Region &region, uint8_t *data)
{
	assert(texture_ != 0);

	texture_->update(region.x, region.y, region.width, region.height, data);
}

Texture *Font::Atlas::texture()
{
	return texture_;
}

Font::Atlas::Region Font::Atlas::region(int width, int height)
{
	Region reg(0, 0, width, height);

	int y;
	int bestIndex = -1;
	int bestWidth = INT_MAX;
	int bestHeight = INT_MAX;

	for (size_t i = 0; i < nodes_.size(); i++)
	{
		if (fits(i, width, height, &y))
		{
			if (y + height < bestHeight || (y + height == bestHeight && nodes_[i].width < bestWidth))
			{
				bestIndex = i;
				bestWidth = nodes_[i].width;
				bestHeight = y + height;

				reg.x = nodes_[i].x;
				reg.y = y;
			}
		}
	}

	if (bestIndex == -1)
	{
		if (enlarge())
			return region(width, height);
		else
			return Region();
	}

	nodes_.insert(nodes_.begin() + bestIndex, Node(reg.x, reg.y + height, width));

	for (size_t i = bestIndex + 1; i < nodes_.size(); i++)
	{
		Node &node = nodes_[i];
		Node &prev = nodes_[i - 1];

		if (node.x >= prev.x + prev.width)
			break;

		int shrink = prev.x + prev.width - node.x;

		node.x += shrink;
		node.width -= shrink;

		if (node.width > 0)
			break;

		nodes_.erase(nodes_.begin() + i--);
	}

	merge();

	return reg;
}

bool Font::Atlas::fits(int index, int width, int height, int *y)
{
	int x = nodes_[index].x;

	if (x + width > texture_->width())
		return false;

	*y = nodes_[index].y;

	int i = index;
	int widthLeft = width;

	while (widthLeft > 0)
	{
		if (nodes_[i].y > *y)
			*y = nodes_[i].y;

		if (*y + height > texture_->height())
			return false;

		widthLeft -= nodes_[i++].width;
	}

	return true;
}

void Font::Atlas::merge()
{
	for (size_t i = 0; i < nodes_.size() - 1; i++)
	{
		if (nodes_[i].y == nodes_[i + 1].y)
		{
			nodes_[i].width += nodes_[i + 1].width;
			nodes_.erase(nodes_.begin() + (i-- + 1));
		}
	}
}

bool Font::Atlas::enlarge()
{
	int width = texture_->width();
	int height = texture_->height();

	int newWidth = width;
	int newHeight = height;

	if (width > height)
		newHeight *= 2;
	else
		newWidth *= 2;

	if (newWidth > context->maxTextureWidth || newHeight > context->maxTextureHeight)
		return false;

	Texture *tx = new Texture(newWidth, newHeight, 1, false);

	if (tx->id() == 0)
	{
		delete tx;
		return false;
	}

	const size_t bufferSize = width * height;
	uint8_t *buffer = new uint8_t[bufferSize];

	texture_->copy(buffer, bufferSize);
	tx->update(0, 0, width, height, buffer);

	memset(buffer, 0, bufferSize);

	if (width > height)
	{
		tx->update(0, height, width, height, buffer);
	}
	else
	{
		tx->update(width, 0, width, height, buffer);
		nodes_.push_back(Node(width, 1, width));
	}

	delete[] buffer;
	delete texture_;
	texture_ = tx;

	return true;
}

} // gfx
